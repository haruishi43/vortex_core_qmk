/*******************************************************************************
**                                  LibChaos                                  **
**                                 zencrypt.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZENCRYPT_H
#define ZENCRYPT_H

#include "ztypes.h"
#include "zbinary.h"

namespace LibChaos {

class ZEncrypt {
public:
    ZEncrypt();
    ~ZEncrypt();

    bool keyLoad(ZBinary key);
    bool encrypt(const ZBinary &plain, ZBinary &ciper);
    bool decrypt(const ZBinary &ciper, ZBinary &plain);
};

}

#endif // ZENCRYPT_H
