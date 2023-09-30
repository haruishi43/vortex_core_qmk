/*******************************************************************************
**                                  LibChaos                                  **
**                                  ztime.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "ztime.h"
#include <stdio.h>

#if LIBCHAOS_COMPILER == LIBCHAOS_COMPILER_MSVC
    #include <windows.h>
#endif

namespace LibChaos {

#if LIBCHAOS_COMPILER == LIBCHAOS_COMPILER_MSVC

ZTime::ZTime(){
    SYSTEMTIME systime;
    GetLocalTime(&systime);
    FILETIME filetime;
    SystemTimeToFileTime(&systime, &filetime);

    // Smash FILETIME into _time
    // It works, I promise*
    timet time;
    memcpy(&time, &filetime, 8);
    _time = time;
}

ZTime::ZTime(const ZTime &other) : _time(other._time){

}

ZTime::~ZTime(){

}

int ZTime::getSecs() const {
    //tm *time;
    return 0;
}

ZString ZTime::timeStr() const {
    SYSTEMTIME systime;
    FILETIME time;
    memcpy(&time, &_time, 8);
    FileTimeToSystemTime(&time, &systime);
    return ZString::ItoS(systime.wHour, 10, 2) + ":" + ZString::ItoS(systime.wMinute, 10, 2) + ":" + ZString::ItoS(systime.wSecond, 10, 2);
}

ZString ZTime::dateStr() const {
    SYSTEMTIME systime;
    FILETIME time;
    memcpy(&time, &_time, 8);
    FileTimeToSystemTime(&time, &systime);
    return ZString::ItoS(systime.wMonth, 10, 2) + "/" + ZString::ItoS(systime.wDay, 10, 2) + "/" + ZString::ItoS(systime.wYear, 10, 2);
}

#else

ZTime::ZTime(){
    _time = time(NULL);
}

ZTime::ZTime(const ZTime &other) : _time(other._time){

}

ZTime::~ZTime(){

}

int ZTime::getSecs() const {
    tm *time;
    return 0;
}

/*! Format Options:
 *  - h(h) - Most significant digit(s) of hour, leading zeroes removed
 *  - m(m) - Most significant digit(s) of minute, leading zeroes removed
 *  - s(s) - Most significant digit(s) of second, leading zeroes removed
 *  - H(H) - Most significant digit(s) of hour, leading zeroes kept
 *  - M(M) - Most significant digit(s) of minute, leading zeroes kept
 *  - S(S) - Most significant digit(s) of second, leading zeroes kept
 *  Examples:
 *  - "hh:mm:ss" -> "2:50:23"
 *  - "HH:MM:SS" -> "05:32:03"
 *  \param format Format String
 *  \return Formatted Time String
 */
ZString ZTime::formatStr(ZString format) const {
    ZString str;
    // TODO: ZTime formatStr()
    return str;
}

ZString ZTime::timeStr() const {
    struct tm *time;
    time = localtime(&_time);
    char buffer[20];
    sprintf(buffer, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
    ZString out(buffer);
    return out;
}

ZString ZTime::dateStr() const {
    struct tm *time;
    time = localtime(&_time);
    char buffer[20];
    //sprintf(buffer, "%02d/%02d/%02d", time->tm_mon + 1, time->tm_mday, time->tm_year - 100);
    sprintf(buffer, "%d-%02d-%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
    ZString out(buffer);
    return out;
}

#endif

}
