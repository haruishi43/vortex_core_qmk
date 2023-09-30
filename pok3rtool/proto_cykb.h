#ifndef PROTO_CYKB_H
#define PROTO_CYKB_H

#include "kbproto.h"
#include "proto_qmk.h"
#include "rawhid/hiddevice.h"

#include "zstring.h"
#include "zbinary.h"
using namespace LibChaos;

class ProtoCYKB : public ProtoQMK {
public:
    enum pok3r_rgb_cmd {
        CMD_16      = 0x10,
        CMD_16_ARG  = 2,

        RESET       = 0x11, //!< Reset command.
        RESET_BL    = 0,    //!< Reset to bootloader.
        RESET_FW    = 1,    //!< Reset to firmware.
        RESET_DIS   = 2,    //!< Disconnect USB.

        READ        = 0x12, //!< Read command.
        READ_400    = 0,
        READ_3C00   = 1,
        READ_MODE   = 2,    //!< Get firmware mode. 0 is bootloader, 1 is firmware.
        READ_VER1   = 0x20, //!< Read version string.
        READ_VER2   = 0x22, //!< Read version data.
        READ_ADDR   = 0xff, //!< Patched command, read arbitrary address.

        FW          = 0x1d, //!< Firmware management command.
        FW_ERASE    = 0,    //!< Erase firmware.
        FW_SUM      = 1,    //!< Firwmare sum.
        FW_CRC      = 2,    //!< Firmware CRC.

        ADDR        = 0x1e, //! Write address command.
        ADDR_GET    = 0,    //! Get write address.
        ADDR_SET    = 1,    //! Set write address.

        WRITE       = 0x1f, //!< Write command.
    };

public:
    //! Construct unopened device.
    ProtoCYKB(zu16 vid, zu16 pid, zu16 boot_pid);
    //! Construct open device from open HIDDevice.
    ProtoCYKB(zu16 vid, zu16 pid, zu16 boot_pid, bool builtin, ZPointer<HIDDevice> dev, zu32 fw_addr);

    ProtoCYKB(const ProtoCYKB &) = delete;
    ~ProtoCYKB();

    //! Find and open POK3R RGB device.
    bool open();
    void close();
    bool isOpen() const;

    bool isBuiltin();

    //! Reset and re-open device.
    bool rebootFirmware(bool reopen = true);
    //! Reset to bootloader and re-open device.
    bool rebootBootloader(bool reopen = true);

    bool getInfo();

    //! Read the firmware version from the keyboard.
    ZString getVersion();

    KBStatus clearVersion();
    KBStatus setVersion(ZString version);

    //! Dump the contents of flash.
    ZBinary dumpFlash();
    //! Update the firmware.
    bool writeFirmware(const ZBinary &fwbin);

    bool eraseAndCheck();

    void test();

    //! Erase flash pages starting at \a start, ending on the page of \a end.
    bool eraseFlash(zu32 start, zu32 length);
    //! Read 64 bytes at \a addr.
    bool readFlash(zu32 addr, ZBinary &bin);
    //! Write 52 bytes at \a addr.
    bool writeFlash(zu32 addr, ZBinary bin);

    //! Get CRC of firmware.
    zu32 crcFlash(zu32 addr, zu32 len);

private:
    zu32 baseFirmwareAddr() const;
    //! Send command
    bool sendCmd(zu8 cmd, zu8 a1, ZBinary data = ZBinary());
    //! Recv command.
    bool recvCmd(ZBinary &data);
    //! Send command and recv response.
    bool sendRecvCmd(zu8 cmd, zu8 a1, ZBinary &data);

public:
    static void decode_firmware(ZBinary &bin);
    static void encode_firmware(ZBinary &bin);
    
    static void info_section(ZBinary data);

private:
    bool builtin;
    bool debug;
    bool nop;
    zu16 vid;
    zu16 pid;
    zu16 boot_pid;
    zu32 fw_addr;
};

#endif // PROTO_CYKB_H
