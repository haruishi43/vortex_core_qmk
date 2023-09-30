/*******************************************************************************
**                                  LibChaos                                  **
**                                 zstring.cpp                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstring.h"
#include "zarray.h"
#include "zlist.h"
#include "zmath.h"

// std::stringstream
#include <sstream>
// std::reverse
#include <algorithm>
// Variable arguments lists
#include <cstdarg>
// pow
#include <math.h>

#include "zlog.h"

namespace LibChaos {

ZString::ZString(ZAllocator<codeunit> *alloc) : _alloc(alloc), _size(0), _realsize(0), _data(nullptr){
    clear(); // Empty string with null terminator
}

ZString::ZString(const ZString &other) : ZString(){
    _resize(other._size);
    if(other._size && other._data)
        _alloc->rawcopy(other._data, _data, other._size);
}

ZString::~ZString(){
    _alloc->dealloc(_data);
    delete _alloc;
}

//
// Specialized Constructors
//

ZString::ZString(char ch, zu64 len) : ZString(){
    _resize(len);
    if(len){
        for(zu64 i = 0; i < len; ++i){
            at(i) = ch;
        }
    }
}

ZString::ZString(const char *str, zu64 max) : ZString(){
    parseUTF8((const codeunit*)str, max);
}

ZString::ZString(const ZArray<char> &array) : ZString(array.raw()){
    // Forwarded
}

ZString::ZString(const codeunit *str, zu64 max) : ZString(){
    parseUTF8(str, max);
}

ZString::ZString(const ZArray<codeunit> &array) : ZString(array.raw()){
    // Forwarded
}

ZString::ZString(std::string str) : ZString(str.c_str()){
    // Forwarded
}

ZString::ZString(const wchar_t *wstr, zu64 max) : ZString(){
    if(wstr && max){
        ZArray<codeunit16> units;
        for(zu64 i = 0; i < max && *wstr; ++i){
            units.push((codeunit16)(*wstr & 0xFFFF));
            ++wstr;
        }
        parseUTF16(units.raw(), units.size());
    }
}

ZString::ZString(const ZArray<wchar_t> &array) : ZString(array.raw()){
    // Forwarded
}

ZString::ZString(std::wstring wstr) : ZString(wstr.c_str()){
    // Forwarded
}

ZString::ZString(double num, unsigned places) : ZString(){
    std::stringstream stream;
    stream << num;
    if(places){
        ArZ arr = ZString(stream.str()).split('.');
        assign(arr[0] + '.' + arr[1].substr(0, places));
    } else {
        assign(stream.str());
    }
}

//
// Operators
//

ZString &ZString::assign(const ZString &other){
    _resize(other.size());
    if(other.size())
        _alloc->rawcopy(other._data, _data, other.size());
    return *this;
}

//
// String Conversions
//

std::string ZString::str() const {
    return std::string(cc(), size());
}

std::wstring ZString::wstr() const {
    std::wstring str;
    zu64 max = size();
    const codeunit *units = _data;
    // Read and add code points
    while(max && *units){
        _appendUTF16(str, _nextUTF8(&units, &max));
    }
    return str;
}

zu64 ZString::readUTF16(codeunit16 *dest, zu64 maxsize) const {
    zu64 max = size();
    const codeunit *units = _data;
    zu64 len = 0;
    // Read and add code points
    while(max && *units && maxsize >= 2){
        zu8 sz = _encodeUTF16(dest, _nextUTF8(&units, &max));
        // Increment len and dest, decrement maxsize
        len += sz;
        maxsize -= sz;
        dest += sz;
    }
    return len;
}

zu64 ZString::readUTF32(codeunit32 *dest, zu64 maxsize) const {
    zu64 max = size();
    const codeunit *units = _data;
    zu64 len = 0;
    // Read and add code points
    while(max && *units){
        codepoint cp = _nextUTF8(&units, &max);
        dest[0] = cp;
        len++;
        maxsize--;
        dest++;
    }
    return len;
}

ZString::codepoint ZString::nextCodePoint(zsize &pos) const {
    zu64 max = size() - pos;
    const codeunit *units = _data + pos;
    codepoint cp = _nextUTF8(&units, &max);
    if(cp)
        pos = (zsize)(units - _data);
    return cp;
}

//
// Numerical Conversions
//

ZString ZString::ItoS(zu64 value, zu8 base, zu64 pad, bool upper){
    ZString buffer;
    if(base < 2 || base > 16)
        return buffer;
    buffer._reserve(35);
    zu64 quotient = value;
    const char *digits = (upper ? "0123456789ABCDEF" : "0123456789abcdef");
    do {
        buffer += digits[ZMath::abs((zs64)(quotient % base))];
        quotient /= base;
    } while(quotient);
    buffer.reverse();
    if(buffer.size() < pad)
        buffer.prepend(ZString('0', pad - buffer.size()));
    return buffer;
}

ZString ZString::ItoS(zs64 value, zu8 base){
    return (value < 0 ? "-" : "") + ItoS((zu64)ZMath::abs(value), base);
}

bool ZString::isInteger(zu8 base) const {
    if(isEmpty())
        return false;
    // Only supports up to hexadecimal
    if(base < 2 || base > 16)
        return false;

    zu64 i = 0;
    // Skip hexadecimal prefix
    if(base == 16 && beginsWith("0x"))
        i+=2;

    for(; i < size(); ++i){
        char ch = tolower(at(i));
        if(i == 0 && ch == '-')
            continue;
        if((ch < '0' || ch > '9') && (ch < 'a' || ch > 'z'))
            return false;
    }
    return true;
}

int ZString::tint() const {
    return atoi(cc());
}

zs64 ZString::toSint(zu8 base) const {
    zu64 unum;
    if(beginsWith("-")){
        unum = ZString::substr(*this, 1).toUint();
        if((zs64)unum < 0)
            return ZS64_MIN;
        return -(zs64)unum;
    } else {
        unum = toUint();
        if((zs64)unum < 0)
            return ZS64_MAX;
        return (zs64)unum;
    }
}

zu64 ZString::toUint(zu8 base) const {
    if(!isInteger(base))
        return 0;
    // Only supports up to hexadecimal
    if(base < 2 || base > 16)
        return false;

    ZString tmp = *this;
    // Skip hexadecimal prefix
    if(base == 16 && tmp.beginsWith("0x"))
        tmp.substr(2);
    tmp.reverse();
    zu64 out = 0;
    for(zu64 i = 0; i < tmp.size(); ++i){
        char ch = tolower(tmp[i]);
        if((ch < '0' || ch > '9') && (ch < 'a' || ch > 'z'))
            return 0;
        char digit = (ch < 58 ? (ch - '0') : (ch - 'a' + 10));
        zu64 add = ((zu64)(digit) * (zu64)pow(base, i));
        if(out + add < out)
            return ZU64_MAX;
        out += add;
    }
    return out;
}

bool ZString::isFloat() const {
    if(isEmpty())
        return false;
    ZString tmp = *this;
    if(tmp[0] == '-')
        tmp.substr(1);
    ArZ parts = tmp.explode('.');
    if(parts.size() < 2)
        return tmp.isInteger();
    if(parts.size() > 2)
        return false;
    if(!parts[0].isInteger() || !parts[1].isInteger())
        return false;
    return true;
}

// TODO: Fix ZString float conversion
float ZString::toFloat() const {
    if(!isFloat())
        return 0.0f;
    float out = 0.0f;
    ZString tmp = *this;
    bool neg = false;
    if(tmp[0] == '-'){
        neg = true;
        tmp.substr(1);
    }
    ArZ parts = split('.');
    ZString whole = reverse(parts[0]);
    for(zu64 i = 0; i < whole.size(); ++i){
        out += ((float)(whole[i] - 48) * pow(10, i));
    }
    if(parts.size() > 1){
        for(zu64 i = 0; i < parts[1].size(); ++i){
            out += ((float)(parts[1][i] - 48) * pow(10, -(zs64)i));
        }
    }
    if(neg)
        out = -out;
    return out;
}

//
// Basic String Manipulation
//

ZString &ZString::append(const ZString &str){
    if(str.size()){
        zu64 oldsize = size();
        // Resize buffer
        _resize(oldsize + str.size());
        // Raw copy other string to end
        _alloc->rawcopy(str._data, _data + oldsize, str.size());
    }
    return *this;
}

ZString ZString::concat(const ZString &str) const {
    ZString out = *this;
    out.append(str);
    return out;
}

ZString &ZString::prepend(const ZString &str){
    if(str.size()){
        zu64 oldsize = size();
        // Resize buffer
        _resize(str.size() + oldsize);
        // Raw move string to end
        _alloc->rawmove(_data, _data + str.size(), oldsize);
        // Raw copy other string to beginning
        _alloc->rawcopy(str._data, _data, str.size());
    }
    return *this;
}

ZString &ZString::substr(zu64 pos, zu64 len){
    if(pos < size()){
        len = MIN(len, size() - pos);
        // Construct new string from range and assign
        _alloc->rawmove(_data + pos, _data, len);
        _resize(len);
        //assign(ZString(_data + pos, len));
    } else {
        clear();
    }
    return *this;
}

ZString ZString::substr(ZString str, zu64 pos, zu64 len){
    if(pos >= str.size() || len == 0)
        return ZString();
    return str.substr(pos, len);
}

ZString &ZString::substr(zu64 pos){
    substr(pos, NONE);
    return *this;
}

ZString ZString::substr(ZString str, zu64 pos){
    return str.substr(pos);
}

ZString &ZString::insert(zu64 pos, const ZString &txt){
    if(txt.size()){
        pos = MIN(pos, size());
        // Get end of string after pos
        ZString after = substr(*this, pos);
        // Truncate to pos
        _resize(pos);
        // Append other string
        append(txt);
        // Append end of string
        append(after);
    }
    return *this;
}

ZString ZString::insert(ZString str, zu64 pos, const ZString &txt){
    return str.insert(pos, txt);
}

ZString &ZString::substitute(zu64 pos, zu64 len, const ZString &after){
    len = MIN(len, size() - pos);
    // Get end of string after range
    ZString part2 = substr(*this, pos + len);
    // Truncate to pos
    _resize(pos);
    // Append other string
    append(after);
    // Append end of string
    append(part2);
    return *this;
}

ZString ZString::substitute(ZString str, zu64 pos, zu64 len, const ZString &after){
    return str.substitute(pos, len, after);
}

// BUG: ZString reverse does not support UTF-8
ZString &ZString::reverse(){
    ZString buff;
    // Size temporary buffer
    buff._resize(size());
    // Copy characters in reverse order
    for(zu64 i = size(), j = 0; i > 0; --i, ++j){
        buff[j] = at(i-1);
    }
    // Swap buffers
    swap(buff);
    return *this;
}

ZString ZString::reverse(ZString str){
    return str.reverse();
}

//
// Basic String Parsing
//

zu64 ZString::count(ZString test) const {
    ZString haystack = _data;
    zu64 cnt = 0;
    for(zu64 i = 0; i < haystack.size(); ++i){
        if(haystack[i] == test[0]){
            bool good = true;
            for(zu64 g = i, j = 0; j < test.size(); ++g, ++j){
                if(haystack[g] != test[j]){
                    good = false;
                    break;
                }
            }
            if(good)
                ++cnt;
        }
    }
    return cnt;
}

bool ZString::beginsWith(const ZString &test, bool ignorews) const {
    zu64 f = findFirst(test, 0);
    if(f == 0)
        return true;
    if(ignorews){
        for(zu64 i = 0; i < f-1; ++i){
            if(!_charIsWhitespace(at(i)))
                return false;
        }
        return true;
    }
    return false;
}

bool ZString::beginsWith(const ZString &str, const ZString &test, bool ignorews){
    return str.beginsWith(test, ignorews);
}

bool ZString::endsWith(ZString test) const {
    if(test.size() > size())
        return false;
    ZString end = substr(*this, size() - test.size(), test.size());
    return test == end;
}

zu64 ZString::findFirst(const ZString &find, zu64 start) const {
    if(find.size() && find.size() <= size() && start < size()){
        zu64 startpos = 0;
        zu64 j = 0;
        for(zu64 i = start; i < size(); ++i){
            if(_data[i] == find[j]){
                if(j == 0)
                    // Find started
                    startpos = i;
                ++j;
                if(j == find.size())
                    // Find complete
                    return startpos;
            } else if(j != 0){
                // Restart find after startpos
                i = startpos;
                j = 0;
            }
        }
    }
    return NONE;
}

zu64 ZString::findFirst(const ZString &str, const ZString &find, zu64 start){
    return str.findFirst(find, start);
}

ZArray<zu64> ZString::findAll(const ZString &findstr) const {
    ZArray<zu64> finds;
    zu64 next = 0;
    do {
        zu64 pos = findFirst(findstr, next);
        if(pos == NONE){
            break;
        }
        finds.push(pos);
        next = pos + findstr.size();
    } while(next < size());
    return finds;
}

ZArray<zu64> ZString::findAll(const ZString &str, const ZString &find){
    return str.findAll(find);
}

//
// Advanced String Manipulation
//

ZString &ZString::replaceFirst(const ZString &before, const ZString &after, zu64 start){
    if(before.size() > size() || before == after)
        return *this;
    _strReplace(before, after, start);
    return *this;
}

ZString ZString::replaceFirst(ZString str, const ZString &before, const ZString &after, zu64 start){
    return str.replaceFirst(before, after, start);
}

ZString &ZString::replace(const ZString &before, const ZString &after, zu64 max){
    // No replacement to do
    if(before.size() > size() || before == after)
        return *this;

    zu64 count = 0;
    zu64 next = 0;
    while(max == 0 || count < max){
        // Replace first occurrence
        next = _strReplace(before, after, next);
        if(next == NONE)
            break;
        ++count;
    }
    return *this;
}

ZString ZString::replace(ZString str, const ZString &before, const ZString &after, zu64 max){
    return str.replace(before, after, max);
}

ZString &ZString::replaceRecursive(const ZString &before, const ZString &after, zu64 max){
    if(before.size() > size() || before == after)
        return *this;

    bool unlim = max == 0 ? true : false;
    zu64 count = 0;
    while(unlim || count < max){
        if(_strReplace(before, after, 0) == NONE)
            break;
        ++count;
    }
    return *this;
}

ZString ZString::replaceRecursive(ZString str, const ZString &before, const ZString &after, zu64 max){
    return str.replaceRecursive(before, after, max);
}

ZString ZString::getUntil(ZString str, const ZString &find){
    zu64 loc = findFirst(str, find);
    return str.substr(0, loc);
}

ZString ZString::findFirstBetween(ZString pre, ZString post){
    if(pre.size() <= 0 || post.size() <= 0)
        return ZString();
    ZString tmp = *this;
    ZString found;
    enum pos_type {
        outside, sopen, eopen, inside, sclose, eclose
    };
    pos_type pos = outside;
    zu64 j = 0;
    zu64 jl = 0;
    zu64 k = 0;
    zu64 kl = 0;
    ZString tmpbuff;
    for(zu64 i = 0; i < tmp.size(); ++i){
        char c = tmp[i];
        switch(pos){
        case outside:
            if(c == pre[pre.size()-1] && jl == pre.size()-1){
                pos = eopen;
            } else if(c == pre[0]){
                j = 1;
                jl = 1;
                pos = sopen;
            }
            break;
        case sopen:
            if(c == pre[pre.size()-1] && jl == pre.size()-1){
                pos = eopen;
            } else if(c == pre[j]){
                ++j;
                ++jl;
                pos = sopen;
            } else if(c == pre[0]){
                j = 1;
                jl = 1;
                pos = sopen;
            } else {
                j = 0;
                jl = 0;
                pos = outside;
            }
            break;
        case eopen:
            if(c == post[post.size()-1] && kl == post.size()-1){
                return found;
            } else if(c == post[0]){
                tmpbuff << c;
                k = 1;
                kl = 1;
                pos = sclose;
            } else {
                found << c;
            }
            break;
        case inside:
            break;
        case sclose:
            if(c == post[post.size()-1] && kl == post.size()-1){
                return found;
            } else if(c == post[k]){
                tmpbuff << c;
                ++k;
                ++kl;
                pos = sclose;
            } else if(c == post[0]){
                found << tmpbuff;
                tmpbuff.clear();
                tmpbuff << c;
                k = 1;
                kl = 1;
                pos = sclose;
            } else {
                found << tmpbuff;
                tmpbuff.clear();
                found << c;
                k = 0;
                kl = 0;
                pos = eopen;
            }
            break;
        case eclose:
            break;
        default:
            // Bad, Probable Memory Corruption
            break;
        }
    }
    found << tmpbuff;
    return found;
}

ZString ZString::replaceBetween(ZString opening_string, ZString closing_string, ZString after_string){
    ZString tmp = _data;
    bool done = false;
    while(!done){
        ZString inside = tmp.findFirstBetween(opening_string, closing_string);
        if(inside.isEmpty()){
            done = true;
            break;
        }
        ZString old = opening_string;
        old << inside << closing_string;
        tmp.replace(old, after_string);
    }
    operator=(tmp);
    return tmp;
}

ZString ZString::findFirstXmlTagCont(ZString tag){
    ZString open;
    open << "<" << tag << ">";
    ZString close;
    close << "</" << tag << ">";
    return findFirstBetween(open, close);
}

ZString ZString::replaceXmlTagCont(ZString tag, ZString after){
    ZString open;
    open << "<" << tag << ">";
    ZString close;
    close << "</" << tag << ">";
    return replaceBetween(open, close, after);
}

ZString &ZString::lpad(char ch, zu64 len){
    if(len > size())
        prepend(ZString(ch, len - size()));
    return *this;

}

ZString &ZString::rpad(char ch, zu64 len){
    if(len > size())
        append(ZString(ch, len - size()));
    return *this;
}

ZString &ZString::stripFront(char target){
    zu64 clen = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(_data[i] == target)
            ++clen;
        else
            break;
    }
    if(clen > 0)
        substr(clen, size());
    return *this;
}

ZString ZString::stripFront(ZString str, char target){
    return str.stripFront(target);
}

ZString &ZString::stripBack(char target){
    zu64 clen = 0;
    for(zu64 i = 0; i < size(); ++i){
        zu64 curr = size() - 1 - i;
        if(_data[curr] == target)
            ++clen;
        else
            break;
    }
    if(clen > 0)
        substr(0, size() - clen);
    return *this;
}

ZString ZString::stripBack(ZString str, char target){
    return str.stripBack(target);
}

ZString &ZString::strip(char target){
    stripFront(target);
    stripBack(target);
    return *this;
}

ZString ZString::strip(ZString str, char target){
    return str.strip(target);
}

ArZ ZString::split(ZString delim) const {
    ArZ out;
    zu64 pos = findFirst(*this, delim);
    if(pos == NONE){
        out.push(*this);
        return out;
    }
    out.push(substr(_data, 0, pos));
    out.push(substr(_data, pos + delim.size()));
    return out;
}

ArZ ZString::explode(char delim) const {
    ArZ out;
    zu64 counter = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(operator[](i) == delim){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}

ArZ ZString::strExplode(const ZString &delim) const {
    ZArray<zu64> pos = findAll(delim);
    ArZ out;
    zu64 startpos = 0;
    for(zu64 i = 0; i < pos.size(); ++i){
        if(startpos < pos[i])
            out.push(substr(*this, startpos, pos[i] - startpos));
        startpos = pos[i] + delim.size();
    }
    if(startpos < size())
        out.push(substr(*this, startpos));
    return out;
}

ArZ ZString::quotedExplode(char delim) const {
    ArZ out;
    zu64 counter = 0;
    bool inquotes = false;
    for(zu64 i = 0; i < size(); ++i){
        if(operator[](i) == '\"'){
            if(counter)
                out.push(substr(*this, i - counter, counter));
            inquotes = !inquotes;
            counter = 0;
            continue;
        } else if(!inquotes && operator[](i) == delim){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}

ArZ ZString::escapedExplode(char delim) const {
    ArZ out;
    zu64 counter = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(operator[](i) == delim && i > 0 && operator[](i - 1) != '\\'){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}

// Fixes syntax highlighting with va_args
#ifndef BUILDING
    #define VAARGTYPE NULL
#else
    #define VAARGTYPE int
#endif
ArZ ZString::explodeList(unsigned nargs, ...) const {
    va_list args;
    va_start(args, nargs);
    ZArray<char> delims;
    for(unsigned i = 0; i < nargs; ++i){
        delims.push(va_arg(args, VAARGTYPE));
    }
    va_end(args);

    ArZ out;
    zu64 counter = 0;
    for(zu64 i = 0; i < size(); ++i){
        // Test for any delimiter
        bool delimfound = false;
        for(zu64 j = 0; j < delims.size(); ++j){
            if(operator[](i) == delims[j]){
                delimfound = true;
                break;
            }
        }
        if(delimfound){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}
#undef VAARGTYPE

ZString ZString::join(ArZ parts, ZString delim){
    ZString name;
    for(zu64 i = 0; i < parts.size(); ++i){
        name += parts[i];
        if(i < parts.size() - 1)
            name += delim;
    }
    return name;
}

ZString ZString::removeWhitespace(){
    replace("  ", "");
    replace("\n", "");
    replace("\r", "");
    return *this;
}

ZString &ZString::toLower(){
    for(zu64 i = 0; i < size(); ++i){
        if(65 <= _data[i] && _data[i] <= 90)
            _data[i] += 32;
    }
    return *this;
}

ZString ZString::toLower(ZString str){
    return str.toLower();
}

ZString &ZString::toUpper(){
    for(zu64 i = 0; i < size(); ++i){
        if(97 <= _data[i] && _data[i] <= 122)
            _data[i] -= 32;
    }
    return *this;
}

ZString ZString::toUpper(ZString str){
    return str.toUpper();
}

ZString &ZString::duplicate(zu64 iter){
    ZString tmp;
    for(zu64 i = 0; i < iter; ++i){
        tmp += _data;
    }
    operator=(tmp);
    return *this;
}

ZString &ZString::format(ZList<ZString> args){
    ZString str = *this;
    // Find all '%' control characters
    ZArray<zu64> fpos = findAll("%");
    zu64 off = 0;
    for(zu64 i = 0; i < fpos.size(); ++i){
        zu64 pos = fpos[i];
        // Insert each string argument
        if(pos + 1 < size() && at(pos + 1) == 's'){
            ZString arg = args.front();
            args.popFront();
            // Replace control characters with argument
            str.substitute(pos + off, 2, arg);
            off += arg.size();
            off -= 2;
        }
    }
    operator=(str);
    return *this;
}

ZString ZString::format(ZString fmtstr, ZList<ZString> args){
    return fmtstr.format(args);
}

bool ZString::charIsNumeric(char ch){
    return (ch >= '0' && ch <= '9');
}

bool ZString::charIsAlphabetic(char ch){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool ZString::charIsAlphanumeric(char ch){
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool ZString::charIsHexadecimal(char ch){
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool ZString::alphaTest(ZString str1, ZString str2){
    for(zu64 k = 0; k < str1.size() && k < str2.size(); ++k){
        if(str1[k] == str2[k])
            continue;
        if(str1[k] < str2[k])
            return true;
        return false;
    }
    if(str1.size() <= str2.size())
        return true;
    return false;
}

void ZString::swap(ZString &other){
    zu64 size = _size;
    zu64 realsize = _realsize;
    codeunit *data = _data;
    _size = other._size;
    _realsize = other._realsize;
    _data = other._data;
    other._size = size;
    other._realsize = realsize;
    other._data = data;
}

zu64 ZString::length() const {
    zu64 max = size();
    const codeunit *units = _data;
    zu64 len = 0;
    // Read and add code points
    while(max && *units){
        zassert(_nextUTF8(&units, &max), "corrupt UTF-8 in zstring");
        len++;
    }
    return len;
}

// ///////////////////////////////////////////////////////////////////////////////
// Private functions
// ///////////////////////////////////////////////////////////////////////////////

void ZString::_reserve(zu64 size){
    if(size > _realsize || _data == nullptr){ // Only reallocate if new size is larger than buffer
        // TEST: newsize, but always leave extra space for null terminator, but don't count null terminator in realsize
        zu64 newsize = MAX(_realsize * 2, size);
        codeunit *buff = _alloc->alloc(newsize + 1); // New size + null terminator
        _alloc->rawcopy(_data, buff, _size); // Copy data to new buffer
        // Update new buffer size
        _realsize = newsize;
        _alloc->dealloc(_data); // Delete old buffer
        _data = buff;
    }
}

void ZString::_resize(zu64 len){
    _reserve(len);
    // If the new size is smaller, just change size
    _size = len;
    _data[_size] = 0; // Always null terminate
}

bool ZString::_charIsWhitespace(char ch){
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

zu64 ZString::_strReplace(const ZString &before, const ZString &after, zu64 startpos){
    zu64 pos = this->findFirst(before, startpos);
    if(pos == NONE)
        return NONE;
    this->substitute(pos, before.size(), after);
    return pos + after.size();
}

// ///////////////////////////////////////////////////////////////////////////////
// Friend functions
// ///////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream& lhs, ZString rhs){
    lhs << rhs.str();
    return lhs;
}

}
