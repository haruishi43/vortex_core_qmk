#include "proto_pok3r.h"
#include "keycodes.h"
#include "zlog.h"

#define UPDATE_PKT_LEN      64

#define VER_ADDR            0x2800
#define FW_ADDR             0x2c00

#define FLASH_LEN           0x20000
#define EEPROM_LEN          0x80000

#define REBOOT_SLEEP        5
#define ERASE_SLEEP         2

#define HEX(A) (ZString::ItoS((zu64)(A), 16))

ProtoPOK3R::ProtoPOK3R(zu16 vid_, zu16 pid_, zu16 boot_pid_) :
    ProtoQMK(PROTO_POK3R, new HIDDevice),
    builtin(false), debug(false), nop(false),
    vid(vid_), pid(pid_), boot_pid(boot_pid_)
{

}

ProtoPOK3R::ProtoPOK3R(zu16 vid_, zu16 pid_, zu16 boot_pid_, bool builtin_, ZPointer<HIDDevice> dev_) :
    ProtoQMK(PROTO_POK3R, dev_),
    builtin(builtin_), debug(false), nop(false),
    vid(vid_), pid(pid_), boot_pid(boot_pid_)
{
    /*
    if(dev.get() && dev.get()->isOpen()){
        if(!sendCmd(QMK_INFO, 0, 0, 0))
            return;

        ZBinary data(64);
        if(!dev->recv(data)){
            ELOG("recv error");
            return;
        }
        DLOG("recv:");
        LOG(ZLog::RAW << data.dumpBytes(4, 8));
    }
    */

}

bool ProtoPOK3R::open(){
    // Try firmware vid and pid
    if(dev->open(vid, pid, UPDATE_USAGE_PAGE, UPDATE_USAGE)){
        builtin = false;
        return true;
    }
    // Try builtin vid and pid
    if(dev->open(vid, boot_pid, UPDATE_USAGE_PAGE, UPDATE_USAGE)){
        builtin = true;
        return true;
    }
    return false;
}

void ProtoPOK3R::close(){
    dev->close();
}

bool ProtoPOK3R::isOpen() const {
    return dev->isOpen();
}

bool ProtoPOK3R::isBuiltin() {
    return builtin;
}

bool ProtoPOK3R::rebootFirmware(bool reopen){
    if(!builtin){
//        LOG("In Firmware");
        return true;
    }

    LOG("Reset to Firmware");
    if(!sendCmd(RESET_CMD, RESET_BOOT_SUBCMD))
        return false;
    close();

    if(reopen){
        ZThread::sleep(REBOOT_SLEEP);

        // Find device with new vid and pid
        if(!open()){
            ELOG("open error");
            return false;
        }

        if(builtin)
            return false;
    }
    return true;
}

bool ProtoPOK3R::rebootBootloader(bool reopen){
    if(builtin){
//        LOG("In Bootloader");
        return true;
    }

    LOG("Reset to Bootloader");
    if(!sendCmd(RESET_CMD, RESET_BUILTIN_SUBCMD))
        return false;
    close();

    if(reopen){
        ZThread::sleep(REBOOT_SLEEP);

        // Find device with new vid and pid
        if(!open()){
            ELOG("open error");
            return false;
        }

        if(!builtin)
            return false;
    }
    return true;
}

bool ProtoPOK3R::getInfo(){
    ZBinary data;
    if(!sendRecvCmd(UPDATE_START_CMD, 0, data))
        return false;

    RLOG(data.dumpBytes(4, 8));

    zu32 a = data.readleu32();
    zu16 fw_addr = data.readleu16();
    zu16 page_size = data.readleu16();
    zu16 e = data.readleu16() + 10;
    zu16 f = data.readleu16() + 10;
    zu32 ver_addr = data.readleu32();

    LOG(ZString::ItoS((zu64)a, 16));
    LOG("firmware address: 0x" << HEX(fw_addr));
    LOG("page size?: 0x" << HEX(page_size));
    LOG(e);
    LOG(f);
    LOG("version_address: 0x" << HEX(ver_addr));

    return true;
}

ZString ProtoPOK3R::getVersion(){
    ZBinary bin;
    if(!readFlash(VER_ADDR, bin))
        return "ERROR";

    ZBinary tst;
    tst.fill(0xFF, 64);
    if(bin == tst)
        return "CLEARED";

    bin.rewind();
    zu32 len = MIN(bin.readleu32(), 64U);
    ZString ver = ZString(bin.raw() + 4, len);
    return ver;
}

KBStatus ProtoPOK3R::clearVersion(){
    DLOG("clearVersion");
    if(!rebootBootloader())
        return ERR_IO;

    LOG("Clear Version");
    if(!eraseFlash(VER_ADDR, VER_ADDR + 8))
        return ERR_IO;

    ZBinary bin;
    if(!readFlash(VER_ADDR, bin))
        return ERR_IO;

    ZBinary tst;
    tst.fill(0xFF, 64);
    if(bin != tst)
        return ERR_IO;

    return SUCCESS;
}

KBStatus ProtoPOK3R::setVersion(ZString version){
    DLOG("setVersion " << version);
    auto status = clearVersion();
    if(status != SUCCESS)
        return status;

    LOG("Writing Version: " << version);

    ZBinary vdata;
    zu64 vlen = version.size() + 4;
    vdata.fill(0, vlen + (4 - (vlen % 4)));
    vdata.writeleu32(version.size());
    vdata.write(version.bytes(), version.size());

    // write version
    if(!writeFlash(VER_ADDR, vdata)){
        LOG("write error");
        return ERR_FAIL;
    }

    // check version
    ZString nver = getVersion();
//    LOG("New Version: " << nver);

    if(nver != version){
        ELOG("failed to set version");
        return ERR_FLASH;
    }

    return SUCCESS;
}

ZBinary ProtoPOK3R::dumpFlash(){
    ZBinary dump;
    zu32 cp = FLASH_LEN / 10;
    int perc = 0;
    RLOG(perc << "%...");
    for(zu32 addr = 0; addr < FLASH_LEN; addr += 64){
        if(!readFlash(addr, dump))
            break;

        if(addr >= cp){
            perc += 10;
            RLOG(perc << "%...");
            cp += FLASH_LEN / 10;
        }
    }
    RLOG("100%" << ZLog::NEWLN);

    return dump;
}

bool ProtoPOK3R::writeFirmware(const ZBinary &fwbinin){
    ZBinary fwbin = fwbinin;
    // Encode the firmware for the POK3R
    encode_firmware(fwbin);

    // update reset
    ZBinary tmp1;
    if(!sendRecvCmd(UPDATE_START_CMD, 0, tmp1))
        return false;

    LOG("Erase...");
    if(!eraseFlash(FW_ADDR, FW_ADDR + fwbin.size())){
//    if(!eraseFlash(VER_ADDR, 0xA108)){
        ELOG("erase error");
        return false;
    }

    ZThread::sleep(ERASE_SLEEP);

    // Write firmware
    LOG("Write...");
    for(zu64 o = 0; o < fwbin.size(); o += 52){
        ZBinary packet;
        fwbin.read(packet, 52);
        if(!writeFlash(FW_ADDR + o, packet)){
            LOG("error writing: 0x" << ZString::ItoS(FW_ADDR + o, 16));
            return false;
        }
    }

    fwbin.rewind();

    LOG("Check...");
    for(zu64 o = 0; o < fwbin.size(); o += 52){
        ZBinary packet;
        fwbin.read(packet, 52);
        if(!checkFlash(FW_ADDR + o, packet)){
            LOG("error checking: 0x" << ZString::ItoS(FW_ADDR + o, 16));
            return false;
        }
    }

    // update reset?
    ZBinary tmp2;
    if(!sendRecvCmd(UPDATE_START_CMD, 0, tmp2))
        return false;
    return true;
}

bool ProtoPOK3R::readFlash(zu32 addr, ZBinary &bin){
    DLOG("readFlash " << HEX(addr));
    // Send command
    ZBinary data;
    data.writeleu32(addr);
    data.writeleu32(addr + 64);
    if(!sendRecvCmd(FLASH_CMD, FLASH_READ_SUBCMD, data))
        return false;
    bin.write(data);
    return true;
}

bool ProtoPOK3R::writeFlash(zu32 addr, ZBinary bin){
    DLOG("writeFlash " << HEX(addr) << " " << bin.size());
    if(!bin.size())
        return false;
    // Send command
    ZBinary arg;
    arg.writeleu32(addr);
    arg.writeleu32(addr + bin.size() - 1);
    arg.write(bin);
    if(!sendCmd(FLASH_CMD, FLASH_WRITE_SUBCMD, arg))
        return false;
    return true;
}

bool ProtoPOK3R::checkFlash(zu32 addr, ZBinary bin){
    DLOG("checkFlash " << HEX(addr) << " " << bin.size());
    if(!bin.size())
        return false;
    // Send command
    ZBinary arg;
    arg.writeleu32(addr);
    arg.writeleu32(addr + bin.size() - 1);
    arg.write(bin);
    if(!sendCmd(FLASH_CMD, FLASH_CHECK_SUBCMD, arg))
        return false;
    return true;
}

bool ProtoPOK3R::eraseFlash(zu32 start, zu32 end){
    DLOG("eraseFlash " << HEX(start) << " " << end);
    // Send command
    ZBinary arg;
    arg.writeleu32(start);
    arg.writeleu32(end);
    if(!sendCmd(ERASE_CMD, 8, arg))
        return false;
    return true;
}

zu16 ProtoPOK3R::crcFlash(zu32 addr, zu32 len){
    // Send command
    ZBinary arg;
    arg.writeleu32(addr);
    arg.writeleu32(len);
    sendCmd(CRC_CMD, 0, arg);
    return 0;
}

zu32 ProtoPOK3R::baseFirmwareAddr() const {
    return FW_ADDR;
}

bool ProtoPOK3R::sendCmd(zu8 cmd, zu8 subcmd, ZBinary bin){
    if(bin.size() > 60){
        ELOG("bad data size");
        return false;
    }

    ZBinary packet(UPDATE_PKT_LEN);
    packet.fill(0);
    packet.writeu8(cmd);    // command
    packet.writeu8(subcmd); // subcommand
    packet.seek(4);
    packet.write(bin);      // data

    packet.seek(2);
    zu16 crc = ZHash<ZBinary, ZHashBase::CRC16>(packet).hash();
    packet.writeleu16(crc); // CRC

    DLOG("send:");
    DLOG(ZLog::RAW << packet.dumpBytes(4, 8));

    // Send command (interrupt write)
    if(!dev->send(packet, (cmd == RESET_CMD ? true : false))){
        ELOG("send error");
        return false;
    }
    return true;
}

bool ProtoPOK3R::sendRecvCmd(zu8 cmd, zu8 subcmd, ZBinary &data){
    if(!sendCmd(cmd, subcmd, data))
        return false;

    // Recv packet
    data.resize(UPDATE_PKT_LEN);
    if(!dev->recv(data)){
        ELOG("recv error");
        return false;
    }

    DLOG("recv:");
    DLOG(ZLog::RAW << data.dumpBytes(4, 8));

    if(data.size() != UPDATE_PKT_LEN){
        DLOG("bad recv size");
        return false;
    }

    data.rewind();
    return true;
}

// POK3R firmware XOR encryption/decryption key
// Found at 0x2188 in Pok3r flash
static const zu32 xor_key[] = {
    0x55aa55aa,
    0xaa55aa55,
    0x000000ff,
    0x0000ff00,
    0x00ff0000,
    0xff000000,
    0x00000000,
    0xffffffff,
    0x0f0f0f0f,
    0xf0f0f0f0,
    0xaaaaaaaa,
    0x55555555,
    0x00000000,
};

// This array was painstakingly translated from a switch with a lot of shifts in the firmware.
// I noticed after the fact that it was identical to the array that Sprite used in his hack,
// but the groups of offsets were in a rotated order. Oh well.
const zu8 swap_key[] = {
    0,1,2,3,
    1,2,3,0,
    2,1,3,0,
    3,2,1,0,
    3,1,0,2,
    1,2,0,3,
    2,3,1,0,
    0,2,1,3,
};

void decode_firmware_packet(zbyte *data, zu32 num){
    zu32 *words = (zu32*)data;

    // XOR decryption
    for(int i = 0; i < 13; ++i){
        words[i] = words[i] ^ xor_key[i];
    }

    // Swap decryption
    zu8 f = (num & 7) << 2;
    for(int i = 0; i < 52; i+=4){
        zbyte a = data[i + swap_key[f + 0]];
        zbyte b = data[i + swap_key[f + 1]];
        zbyte c = data[i + swap_key[f + 2]];
        zbyte d = data[i + swap_key[f + 3]];

        data[i + 0] = a;
        data[i + 1] = b;
        data[i + 2] = c;
        data[i + 3] = d;
    }
}

// Decode the encryption scheme used by the POK3R firmware
// Ripped from the pok3r builtin firmware
void ProtoPOK3R::decode_firmware(ZBinary &bin){
    zu32 count = 0;
    for(zu32 offset = 0; offset < bin.size(); offset += 52){
        if(count >= 10 && count <= 100){
            decode_firmware_packet(bin.raw() + offset, count);
        }
        count++;
    }
}

void encode_firmware_packet(zbyte *data, zu32 num){
    zu32 *words = (zu32*)data;

    // Swap encryption
    zu8 f = (num & 7) << 2;
    for(int i = 0; i < 52; i+=4){
        zbyte a = data[i + 0];
        zbyte b = data[i + 1];
        zbyte c = data[i + 2];
        zbyte d = data[i + 3];

        data[i + swap_key[f + 0]] = a;
        data[i + swap_key[f + 1]] = b;
        data[i + swap_key[f + 2]] = c;
        data[i + swap_key[f + 3]] = d;
    }

    // XOR encryption
    for(int i = 0; i < 13; ++i){
        words[i] = words[i] ^ xor_key[i];
    }
}

// Encode using the encryption scheme used by the POK3R firmware
// Reverse engineered from the above
void ProtoPOK3R::encode_firmware(ZBinary &bin){
    zu32 count = 0;
    for(zu32 offset = 0; offset < bin.size(); offset += 52){
        if(count >= 10 && count <= 100){
            encode_firmware_packet(bin.raw() + offset, count);
        }
        count++;
    }
}
