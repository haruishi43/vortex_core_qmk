/*******************************************************************************
**                                  LibChaos                                  **
**                                 zrandom.cpp                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zrandom.h"
#include "zstring.h"
#include "zerror.h"
#include "zlog.h"

#include <assert.h>

#if LIBCHAOS_PLATFORM == LIBCHAOS_PLATFORM_WINDOWS
    #include <windows.h>
    #define RAND_DEV "ZRANDOM_GENERATOR"
#elif LIBCHAOS_PLATFORM == LIBCHAOS_PLATFORM_MACOSX
    // OSX's /dev/random is equivalent to UNIX's /dev/urandom
    #define RAND_DEV "/dev/random"
#else
    // Use /dev/urandom for reliability
    #define RAND_DEV "/dev/urandom"
#endif

namespace LibChaos {

ZRandom::ZRandom(){
#if LIBCHAOS_PLATFORM == LIBCHAOS_PLATFORM_WINDOWS
    _cryptprov = new HCRYPTPROV;
    ZString keyset = RAND_DEV;
    // Try to use existing named keyset
    BOOL ret = CryptAcquireContextA((HCRYPTPROV*)_cryptprov, keyset.cc(), NULL, PROV_RSA_FULL, 0);
    if(ret == FALSE){
        if(GetLastError() == 0x80090016){
            // Create new named keyset
            ret = CryptAcquireContextA((HCRYPTPROV*)&_cryptprov, keyset.cc(), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
            if(ret == FALSE){
                ELOG(ZError::getSystemError());
            }
        } else {
            ELOG(ZError::getSystemError());
        }
    }
#else
    // Use OS filesystem random device
    _devrandom = fopen(RAND_DEV, "r");
    if(_devrandom == NULL)
        ELOG("Failed to open " RAND_DEV ":" << ZError::getSystemError());
#endif
}

ZRandom::~ZRandom(){
#if LIBCHAOS_PLATFORM == LIBCHAOS_PLATFORM_WINDOWS
    delete (HCRYPTPROV*)_cryptprov;
#else
    fclose(_devrandom);
    _devrandom = NULL;
#endif
}

zu64 ZRandom::genzu(zu64 min, zu64 max){
    ZBinary buff = generate(8);
    zu64 rand = (zu64)buff[0] | (zu64)buff[1] << 8 | (zu64)buff[2] << 16 | (zu64)buff[3] << 24 | (zu64)buff[4] << 32 | (zu64)buff[5] << 40 | (zu64)buff[6] << 48 | (zu64)buff[7] << 56;
    // TODO: Imperfect random distribution
    rand = rand % (max - min) + min;
    return rand;
}

ZBinary ZRandom::generate(zu64 size){
    ZBuffer buffer(size);
#if LIBCHAOS_PLATFORM == LIBCHAOS_PLATFORM_WINDOWS
    BOOL ret = CryptGenRandom(*(HCRYPTPROV*)_cryptprov, size, buffer.raw());
    if(ret == FALSE)
        ELOG(ZError::getSystemError());
#else
    if(_devrandom == NULL)
        throw ZException("Random device is not open");
    else
        fread(buffer.raw(), 1, size, _devrandom);
#endif
    return buffer;
}

bool ZRandom::chance(double probability){
    probability = MAX(probability, (double)0.0f);
    probability = MIN(probability, (double)1.0f);
    if(probability == 0.0f)
        return false;
    zu64 rand = genzu();
    return ((double)rand / (double)ZU64_MAX) <= probability;
}

}
