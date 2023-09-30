#include "tests.h"
#include "zbinary.h"

namespace LibChaosTest {

void binary_construct(){
    ZBinary bin1({'A','B','C','D','E','F'});
    LOG(ZString(bin1.printable().asChar()));
    LOG(ZString(bin1.getSub(0, 2).printable().asChar()));
    LOG(ZString(bin1.getSub(2, 2).printable().asChar()));
    LOG(ZString(bin1.getSub(4, 6).printable().asChar()));
    LOG(ZString(bin1.getSub(5, 4).printable().asChar()));
    LOG(ZString(bin1.getSub(6, 2).printable().asChar()));
}

void binary_find(){
    ZBinary bin2({'A','0','B','C','D','2','2','E','F'});
    LOG(ZString(bin2.printable().asChar()));
    zu64 pos = bin2.findFirst({'2','2'});
    LOG(pos);
    LOG(ZString(bin2.getSub(0, pos).printable().asChar()));
}

void binary_read_write(){
    ZBinary bin3;
    bin3.reserve(16);
    zu64 num1 = 0x056379084560;
    zu64 num2 = 0x890345890326;
    bin3.writeleu64(num1);
    bin3.writebeu64(num2);
    bin3.rewind();
    zu64 num1o = bin3.readleu64();
    zu64 num2o = bin3.readbeu64();
    LOG(num1 << " " << num1o);
    LOG(num2 << " " << num2o);
    TASSERT(num1 == num1o);
    TASSERT(num2 == num2o);
}

ZArray<Test> binary_tests(){
    return {
        { "binary-construct",   binary_construct,   true, {} },
        { "binary-find",        binary_find,        true, { "binary-construct" } },
        { "binary-read-write",  binary_read_write,  true, {} },
    };
}

}
