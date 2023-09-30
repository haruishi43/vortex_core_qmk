/*******************************************************************************
**                                  LibChaos                                  **
**                                  zwriter.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZWRITER_H
#define ZWRITER_H

#include "ztypes.h"

#include "zreader.h"
#include "zallocator.h"

namespace LibChaos {

class ZWriter {
public:
    virtual ~ZWriter(){}

    // Pure Virtual

    //! Write \a size bytes from \a src to object at current position.
    virtual zu64 write(const zbyte *src, zu64 size) = 0;

    // Virtual

    /*! Read \a size bytes from \a reader and write to this in blocks of \a blocksize.
     *  Returns number of bytes written to this.
     */
    virtual zu64 writeFrom(ZReader *reader, zu64 size, zu64 blocksize = (1 << 15)){
        zbyte *buffer = new zbyte[blocksize];
        zu64 totalwritten = 0;
        while(totalwritten < size){
            // Read into buffer
            zu64 readsize = MAX(blocksize, size - totalwritten);
            if(reader->read(buffer, readsize) != readsize){
                break;
            }
            // Write
            zu64 written = write(buffer, readsize);
            if(written != readsize){
                break;
            }
            totalwritten += written;
        }
        delete[] buffer;
        return totalwritten;
    }

    // Non-Virtual

    // Single Byte
    //! Write 8-bit unsigned integer.
    bool writeu8(zu8 num){
        return (write(&num, 1) == 1);
    }
    //! Write 8-bit signed integer.
    bool writes8(zu8 num){
        return (write(&num, 1) == 1);
    }

    // Little-endian Unsigned
    //! Write 16-bit little-endian unsigned integer.
    bool writeleu16(zu16 num){
        zbyte tmp[2];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        return (write(tmp, 2) == 2);
    }
    //! Write 32-bit little-endian unsigned integer.
    bool writeleu32(zu32 num){
        zbyte tmp[4];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        return (write(tmp, 4) == 4);
    }
    //! Write 64-bit little-endian unsigned integer.
    bool writeleu64(zu64 num){
        zbyte tmp[8];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        tmp[4] = (num >> 32) & 0xFF;
        tmp[5] = (num >> 40) & 0xFF;
        tmp[6] = (num >> 48) & 0xFF;
        tmp[7] = (num >> 56) & 0xFF;
        return (write(tmp, 8) == 8);
    }

    // Little-endian Signed
    //! Write 16-bit little-endian signed integer.
    bool writeles16(zu16 num){
        zbyte tmp[2];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        return (write(tmp, 2) == 2);
    }
    //! Write 32-bit little-endian signed integer.
    bool writeles32(zu32 num){
        zbyte tmp[4];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        return (write(tmp, 4) == 4);
    }
    //! Write 64-bit little-endian signed integer.
    bool writeles64(zu64 num){
        zbyte tmp[8];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        tmp[4] = (num >> 32) & 0xFF;
        tmp[5] = (num >> 40) & 0xFF;
        tmp[6] = (num >> 48) & 0xFF;
        tmp[7] = (num >> 56) & 0xFF;
        return (write(tmp, 8) == 8);
    }

    // Big-endian Unsigned
    //! Write 16-bit big-endian unsigned integer.
    bool writebeu16(zu16 num){
        zbyte tmp[2];
        tmp[0] = (num >> 8) & 0xFF;
        tmp[1] = num & 0xFF;
        return (write(tmp, 2) == 2);
    }
    //! Write 32-bit big-endian unsigned integer.
    bool writebeu32(zu32 num){
        zbyte tmp[4];
        tmp[0] = (num >> 24) & 0xFF;
        tmp[1] = (num >> 16) & 0xFF;
        tmp[2] = (num >> 8) & 0xFF;
        tmp[3] = num & 0xFF;
        return (write(tmp, 4) == 4);
    }
    //! Write 64-bit big-endian unsigned integer.
    bool writebeu64(zu64 num){
        zbyte tmp[8];
        tmp[0] = (num >> 56) & 0xFF;
        tmp[1] = (num >> 48) & 0xFF;
        tmp[2] = (num >> 40) & 0xFF;
        tmp[3] = (num >> 32) & 0xFF;
        tmp[4] = (num >> 24) & 0xFF;
        tmp[5] = (num >> 16) & 0xFF;
        tmp[6] = (num >> 8) & 0xFF;
        tmp[7] = num & 0xFF;
        return (write(tmp, 8) == 8);
    }

    // Big-endian Signed
    //! Write 16-bit big-endian signed integer.
    bool writebes16(zs16 num){
        zbyte tmp[2];
        tmp[0] = (num >> 8) & 0xFF;
        tmp[1] = num & 0xFF;
        return (write(tmp, 2) == 2);
    }
    //! Write 32-bit big-endian signed integer.
    bool writebes32(zs32 num){
        zbyte tmp[4];
        tmp[0] = (num >> 24) & 0xFF;
        tmp[1] = (num >> 16) & 0xFF;
        tmp[2] = (num >> 8) & 0xFF;
        tmp[3] = num & 0xFF;
        return (write(tmp, 4) == 4);
    }
    //! Write 64-bit big-endian signed integer.
    bool writebes64(zs64 num){
        zbyte tmp[8];
        tmp[0] = (num >> 56) & 0xFF;
        tmp[1] = (num >> 48) & 0xFF;
        tmp[2] = (num >> 40) & 0xFF;
        tmp[3] = (num >> 32) & 0xFF;
        tmp[4] = (num >> 24) & 0xFF;
        tmp[5] = (num >> 16) & 0xFF;
        tmp[6] = (num >> 8) & 0xFF;
        tmp[7] = num & 0xFF;
        return (write(tmp, 8) == 8);
    }

    // Floating point
    // Implementation may change
    //! Write single-precision floating point number.
    bool writefloat(float num){
        zu32 raw = *(zu32 *)&num;
        zbyte tmp[4];
        tmp[0] = (raw >> 24) & 0xFF;
        tmp[1] = (raw >> 16) & 0xFF;
        tmp[2] = (raw >> 8) & 0xFF;
        tmp[3] = raw & 0xFF;
        return (write(tmp, 4) == 4);
    }
    //! Write double-precision floating point number.
    bool writedouble(double num){
        zu64 raw = *(zu64 *)&num;
        zbyte tmp[8];
        tmp[0] = (raw >> 56) & 0xFF;
        tmp[1] = (raw >> 48) & 0xFF;
        tmp[2] = (raw >> 40) & 0xFF;
        tmp[3] = (raw >> 32) & 0xFF;
        tmp[4] = (raw >> 24) & 0xFF;
        tmp[5] = (raw >> 16) & 0xFF;
        tmp[6] = (raw >> 8) & 0xFF;
        tmp[7] = raw & 0xFF;
        return (write(tmp, 8) == 8);
    }

};

}

#endif // ZWRITER_H
