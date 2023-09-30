/*******************************************************************************
**                                  LibChaos                                  **
**                                   zuid.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"
#include "zbinary.h"
#include "zlist.h"
#include "zhash.h"

#define ZUID_NIL LibChaos::ZUID()
#define ZUID_SIZE 16

namespace LibChaos {

//! Generates IETF RFC 4122 UUIDs.
class ZUID {
public:
    enum uuidtype {
        NIL         = 0,    //!< Nil UUID (00000000-0000-0000-0000-000000000000).
        TIME        = 1,    //!< Date-time-MAC-based Version 1 UUID.
        RANDOM      = 4,    //!< Random-based Version 4 UUID.
#ifdef ZHASH_HAS_MD5
        NAME_MD5    = 3,    //!< Name-MD5-based Version 3 UUID.
#endif
#ifdef ZHASH_HAS_SHA1
        NAME_SHA    = 5,    //!< Name-SHA-based Version 5 UUID.
        NAME = NAME_SHA,    //!< Same as NAME_SHA.
#endif
        UNINIT,             //!< Uninitialized UUID. For internal use.
        UNKNOWN,            //!< Error value.
    };

public:
    //! Default NIL UUID.
    ZUID();
    //! Generate new UUID of \a type.
    ZUID(uuidtype type, ZUID namespce = ZUID(), ZString name = ZString());
    /*! Parse existing UUID string.
     *  String must contain 32 hexadecimal characters,
     *  any number of ' ', '-' or ':' characters are ignored.
     */
    ZUID(ZString str);

    //! Compare two ZUIDs, -1, 0 or 1.
    int compare(const ZUID &uid);

    //! Compare UUIDs.
    bool operator==(const ZUID &uid);

    //! Algebraic comparison for ZUID trees.
    bool operator<(const ZUID &uid);

    //! Read 16 bytes into this UUID.
    ZUID &fromRaw(zbyte *bytes);

    //! Get the type of the UUID.
    uuidtype getType() const;

    //! Get hexadecimal UUID string.
    ZString str(ZString delim = "-") const;
    //! Get binary container object.
    ZBinary bin() const;
    //! Get pointer to raw 16-octet UUID.
    const zoctet *raw() const { return _id_octets; }

public:
    //! Get an acceptable timestamp.
    static zu64 getTimestamp();
    //! Get all MAC addresses.
    static ZList<ZBinary> getMACAddresses();
    //! Get an acceptable MAC address. Caches the first found for quick lookup.
    static ZBinary getMACAddress(bool cache = false);

private:
    //! Check if MAC address is acceptable.
    static bool validMAC(const zoctet *addr);

    static ZBinary nameHashData(ZUID namesp, ZString name);
    void nameHashSet(ZBinary hash);

private:
    //! UUID octets.
    zoctet _id_octets[ZUID_SIZE];
};

// ZUID specialization ZHash
ZHASH_USER_SPECIALIAZATION(ZUID, (const ZUID &uid), (uid.raw(), ZUID_SIZE), {})

} // namespace LibChaos

#endif // ZUID_H
