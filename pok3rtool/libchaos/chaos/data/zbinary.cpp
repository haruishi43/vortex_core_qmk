#include "zbinary.h"
#include "zerror.h"
//#include "zlog.h"

namespace LibChaos {

ZBinary &ZBinary::fill(ZBinary::bytetype dat){
    return fill(dat, size());
}

ZBinary &ZBinary::fill(ZBinary::bytetype dat, zu64 size){
    reserve(size);
    _size = size;
    memset(_data, dat, _size);
    return *this;
}

ZBinary &ZBinary::concat(const ZBinary &other){
    reserve(_size + other._size);
    _alloc->rawcopy(other._data, _data + _size, other._size);
    _size = _size + other._size;
    return *this;
}

ZBinary &ZBinary::append(ZBinary::bytetype byte){
    resize(_size + 1);
    at(_size - 1) = byte;
    return *this;
}

void ZBinary::reverse(){
    bytetype *buffer = _alloc->alloc(_realsize);
    for(zu64 i = 0; i < _size; ++i){
        buffer[i] = _data[_size - i - 1];
    }
    _alloc->dealloc(buffer);
    _data = buffer;
}

zu64 ZBinary::findFirst(const ZBinary &find) const {
    if(find.size() > _size){
        return NONE;
    }
    zu64 j = 0;
    zu64 start = NONE;
    for(zu64 i = 0; i < _size; ++i){
        if(_data[i] == find[j]){
            if(j == find.size() - 1)
                return start;
            if(j == 0)
                start = i;
            ++j;
        } else {
            if(j){
                j = 0;
                i = start + 1;
            }
        }
    }
    return start;
}

ZBinary ZBinary::getSub(zu64 start, zu64 len) const {
    if(start >= _size)
        return ZBinary();
    if(start + len >= _size)
        len = _size - start;
    return ZBinary(_data + start, len);
}

zsize ZBinary::subDiff(const ZBinary &in, ZBinary &out){
    if(!size() || !in.size() || size() != in.size())
        return ZU64_MAX;

    zsize start = 0;
    // Find first different byte
    for(zsize i = 0; i < size() && i < in.size(); ++i){
        if(at(i) != in[i]){
            start = i;
            break;
        }
    }

    zsize end = 0;
    // Find last different byte
    for(zsize i = 0; i < size() && i < in.size(); ++i){
        if(at(size()-1-i) != in[size()-1-i]){
            end = size()-1-i;
            break;
        }
    }

    //LOG("start " << ZString::ItoS(start, 16) << " end " << ZString::ItoS(end, 16));
    out = in.getSub(start, end-start+1);
    return start;
}

ZBinary ZBinary::fromHex(ZString str){
    ZBinary bin;
    str.replace(" ", "");
    str.replace("\t", "");
    str.replace("\n", "");
    str.replace("\r", "");
    str.replace(":", "");
    if((str.size() % 2) != 0){
        return bin;
    }
    for(zu64 i = 0; i < str.size(); ++i){
        char ch = str[i];
        if(!(ch >= 48 && ch <= 57) && !(ch >= 97 && ch <= 102))
            return bin;
    }
    str.toLower();
    bin.resize(str.size() / 2);
    for(zu64 i = 0; i < bin.size(); ++i){
        char ch = str[(i*2)];
        char cl = str[(i*2)+1];
        bin[i] = (ch > 57 ? ch - 87 : ch - 48) << 4 | (cl > 57 ? cl - 87 : cl - 48);
    }
    return bin;
}

ZBinary ZBinary::fromzu8(zu8 num){
    ZBinary bin(1);
    encu8(bin._data, num);
    return bin;
}

ZBinary ZBinary::fromzu16(zu16 num){
    ZBinary bin(2);
    enczu16(bin._data, num);
    return bin;
}

ZBinary ZBinary::fromzu32(zu32 num){
    ZBinary bin(4);
    enczu32(bin._data, num);
    return bin;
}

ZBinary ZBinary::fromzu64(zu64 num){
    ZBinary bin(8);
    enczu64(bin._data, num);
    return bin;
}

zu8 ZBinary::tozu8() const {
    zassert(_size == 1);
    return decu8(_data);
}

zu16 ZBinary::tozu16() const {
    zassert(_size == 2);
    return deczu16(_data);
}

zu32 ZBinary::tozu32() const {
    zassert(_size == 4);
    return deczu32(_data);
}

zu64 ZBinary::tozu64() const {
    zassert(_size == 8);
    return deczu64(_data);
}

ZBinary &ZBinary::nullTerm(){
    if(_size && _data[_size - 1] != 0){
        resize(_size + 1);
        _data[_size - 1] = 0;
    }
    return *this;
}

ZBinary ZBinary::printable() const {
    ZBinary tmp = *this;
    if(_size){
        tmp.nullTerm();
        for(zu64 i = 0; i < _size - 1; ++i){
            if(_data[i] == 0){
                _data[i] = '0';
            } else if(_data[i] > 127){
                _data[i] = '!';
            }
        }
    }
    return tmp;
}

ZString ZBinary::strBytes(zu16 groupsize, zu16 linesize, bool upper) const {
    ZString str;
    for(zu64 i = 1; i < size()+1; ++i){
        str += ZString::ItoS(_data[i-1], 16, 2, upper) + (groupsize != 0 && i % groupsize == 0 ? " " : "");
        if(linesize != 0 && i % ((zu32)linesize * groupsize) == 0){
            str += "\n";
        }
    }
    return str;
}

ZString ZBinary::dumpBytes(zu16 groupsize, zu16 linesize, zu64 offset, bool upper) const{
    const zu64 linelen = (zu64)groupsize * linesize;
    ZString str;
    str += ZString::ItoS(offset, 16, 4) + "  ";
    zu64 i = 1;
    for(i = 1; i < size()+1; ++i){
        str += ZString::ItoS(_data[i-1], 16, 2, upper) + (groupsize != 0 && i % groupsize == 0 ? " " : "");
        if(linesize != 0 && i % linelen == 0){
            ZString asc;
            for(zu64 j = i - linelen; j < i; ++j)
                asc += displayByte(_data[j]);
            str += ZString("| ") + asc + "\n";
            if(i < size())
                str += ZString::ItoS(offset + i, 16, 4) + "  ";
        }
    }
    --i;
    if(linesize != 0 && i % linelen != 0){
        ZString asc;
        for(zu64 j = i - (i % linelen); j < i; ++j)
            asc += displayByte(_data[j]);
        str += ZString(' ', (linelen - (i % linelen)) * 2 + linesize - (i % linelen) / groupsize);
        str += ZString("| ") + asc + ZString(' ', linelen - (i % linelen)) + "\n";
    }
    if(linesize == 0 || size() == 0)
        str += "\n";
    return str;
}

ZString ZBinary::displayByte(zbyte byte){
    if(byte > 31 && byte < 126)
        return ZString((char)byte);
    else
        return ".";
}

zu64 ZBinary::read(zbyte *dest, zu64 length){
    length = MIN(length, _size - _rwpos);
    if(dest != nullptr && length != 0)
        memcpy(dest, _data + _rwpos, length);
    _rwpos += length;
    return length;
}

zu64 ZBinary::read(ZBinary &dest, zu64 length){
    if(dest.size() < length)
        dest.resize(length);
    zu64 len = read(dest.raw(), length);
    dest.resize(len);
    return len;
}

zu64 ZBinary::write(const zbyte *data, zu64 size){
    if(size > _size - _rwpos)
        resize(_rwpos + size);
    memcpy(_data + _rwpos, data, size);
    _rwpos += size;
    return size;
}

zu8 ZBinary::decu8(const zbyte *bin){
    return (zu8)bin[0];
}
void ZBinary::encu8(zbyte *bin, zu8 num){
    bin[0] = (zbyte)num;
}

zu16 ZBinary::decbeu16(const zbyte *bin){
    return ((zu16)bin[0] << 8) |
           ((zu16)bin[1]);
}
zu32 ZBinary::decbeu32(const zbyte *bin){
    return ((zu32)bin[0] << 24) |
           ((zu32)bin[1] << 16) |
           ((zu32)bin[2] << 8)  |
           ((zu32)bin[3]);
}
zu64 ZBinary::decbeu64(const zbyte *bin){
    return ((zu64)bin[0] << 56) |
           ((zu64)bin[1] << 48) |
           ((zu64)bin[2] << 40) |
           ((zu64)bin[3] << 32) |
           ((zu64)bin[4] << 24) |
           ((zu64)bin[5] << 16) |
           ((zu64)bin[6] << 8)  |
           ((zu64)bin[7]);
}

void ZBinary::encbeu16(zbyte *bin, zu16 num){
    bin[0] = (zbyte)((num >> 8) & 0xFF);
    bin[1] = (zbyte)((num)      & 0xFF);
}
void ZBinary::encbeu32(zbyte *bin, zu32 num){
    bin[0] = (zbyte)((num >> 24) & 0xFF);
    bin[1] = (zbyte)((num >> 16) & 0xFF);
    bin[2] = (zbyte)((num >> 8)  & 0xFF);
    bin[3] = (zbyte)((num)       & 0xFF);
}
void ZBinary::encbeu64(zbyte *bin, zu64 num){
    bin[0] = (zbyte)((num >> 56) & 0xFF);
    bin[1] = (zbyte)((num >> 48) & 0xFF);
    bin[2] = (zbyte)((num >> 40) & 0xFF);
    bin[3] = (zbyte)((num >> 32) & 0xFF);
    bin[4] = (zbyte)((num >> 24) & 0xFF);
    bin[5] = (zbyte)((num >> 16) & 0xFF);
    bin[6] = (zbyte)((num >> 8)  & 0xFF);
    bin[7] = (zbyte)((num)       & 0xFF);
}

zu16 ZBinary::decleu16(const zbyte *bin){
    return ((zu16)bin[1] << 8) |
           ((zu16)bin[0]);
}
zu32 ZBinary::decleu32(const zbyte *bin){
    return ((zu32)bin[3] << 24) |
           ((zu32)bin[2] << 16) |
           ((zu32)bin[1] << 8)  |
           ((zu32)bin[0]);
}
zu64 ZBinary::decleu64(const zbyte *bin){
    return ((zu64)bin[7] << 56) |
           ((zu64)bin[6] << 48) |
           ((zu64)bin[5] << 40) |
           ((zu64)bin[4] << 32) |
           ((zu64)bin[3] << 24) |
           ((zu64)bin[2] << 16) |
           ((zu64)bin[1] << 8)  |
           ((zu64)bin[0]);
}

void ZBinary::encleu16(zbyte *bin, zu16 num){
    bin[1] = (zbyte)((num >> 8) & 0xFF);
    bin[0] = (zbyte)((num)      & 0xFF);
}
void ZBinary::encleu32(zbyte *bin, zu32 num){
    bin[3] = (zbyte)((num >> 24) & 0xFF);
    bin[2] = (zbyte)((num >> 16) & 0xFF);
    bin[1] = (zbyte)((num >> 8)  & 0xFF);
    bin[0] = (zbyte)((num)       & 0xFF);
}
void ZBinary::encleu64(zbyte *bin, zu64 num){
    bin[7] = (zbyte)((num >> 56) & 0xFF);
    bin[6] = (zbyte)((num >> 48) & 0xFF);
    bin[5] = (zbyte)((num >> 40) & 0xFF);
    bin[4] = (zbyte)((num >> 32) & 0xFF);
    bin[3] = (zbyte)((num >> 24) & 0xFF);
    bin[2] = (zbyte)((num >> 16) & 0xFF);
    bin[1] = (zbyte)((num >> 8)  & 0xFF);
    bin[0] = (zbyte)((num)       & 0xFF);
}

float ZBinary::decfloat(const zbyte *bin){
    zu32 n = deczu32(bin);
    if(LIBCHAOS_BIG_ENDIAN){
        return *(float *)&n;
    } else {
        zbyte *rawbe = (zbyte *)&n;
        float out;
        zbyte *raw = (zbyte *)&out;
        raw[0] = rawbe[3];
        raw[1] = rawbe[2];
        raw[2] = rawbe[1];
        raw[3] = rawbe[0];
        return out;
    }
}

double ZBinary::decdouble(const zbyte *bin){
    zu32 n = deczu64(bin);
    if(LIBCHAOS_BIG_ENDIAN){
        return *(double *)&n;
    } else {
        zbyte *rawbe = (zbyte *)&n;
        double out;
        zbyte *raw = (zbyte *)&out;
        raw[0] = rawbe[7];
        raw[1] = rawbe[6];
        raw[2] = rawbe[5];
        raw[3] = rawbe[4];
        raw[4] = rawbe[3];
        raw[5] = rawbe[2];
        raw[6] = rawbe[1];
        raw[7] = rawbe[0];
        return out;
    }
}

void ZBinary::encfloat(zbyte *bin, float flt){
    zbyte *raw;
    if(LIBCHAOS_BIG_ENDIAN){
        raw = (zbyte *)&flt;
    } else {
        zbyte *rawbe = (zbyte *)&flt;
        float out;
        raw = (zbyte *)&out;
        raw[0] = rawbe[3];
        raw[1] = rawbe[2];
        raw[2] = rawbe[1];
        raw[3] = rawbe[0];
    }
    enczu32(bin, *(zu32 *)raw);
}

void ZBinary::encdouble(zbyte *bin, double dbl){
    zbyte *raw;
    if(LIBCHAOS_BIG_ENDIAN){
        raw = (zbyte *)&dbl;
    } else {
        zbyte *rawbe = (zbyte *)&dbl;
        double out;
        raw = (zbyte *)&out;
        raw[0] = rawbe[7];
        raw[1] = rawbe[6];
        raw[2] = rawbe[5];
        raw[3] = rawbe[4];
        raw[4] = rawbe[3];
        raw[5] = rawbe[2];
        raw[6] = rawbe[1];
        raw[7] = rawbe[0];
    }
    enczu64(bin, *(zu64 *)raw);
}

}
