#include "tests.h"

#include "zfile.h"
#include "zexception.h"
#include "zrandom.h"
#include "zhash.h"

namespace LibChaosTest {

void file_create_dir(){
    ZPath near = "testdir";
    ZPath farfile = near + "a/b/c/d/e/f.dat";

    LOG("makeDir: " << near);
    TASSERT(ZFile::makeDir(near));

    LOG("createDirsTo: " << farfile);
    TASSERT(ZFile::createDirsTo(farfile));
}

void file_write(){
    ZPath near = "testdir";
    ZPath farfile = near + "a/b/c/d/e/f.dat";
    ZPath nearfile = near + "near.dat";

    ZFile farfl(farfile, ZFile::READWRITE);
    TASSERT(farfl.isOpen());

    LOG(farfl.write("this file is far away"));
    farfl.close();

    ZFile nearfl(nearfile, ZFile::READWRITE);
    TASSERT(nearfl.isOpen());

    LOG(nearfl.write("this file is nearby"));
    nearfl.close();
}

void file_list(){
    ZPath near = "testdir";

    ZArray<ZPath> nearfiles = ZFile::listFiles(near, false);
    for(zu64 i = 0; i < nearfiles.size(); ++i){
        nearfiles[i].relativeTo(ZPath::pwd());
        LOG(nearfiles[i]);
    }

    TASSERT(nearfiles.size() == 1);
    TASSERT(nearfiles.front().data() == ArZ({ "testdir", "near.dat" }))

    ZArray<ZPath> farfiles = ZFile::listFiles(near, true);
    for(zu64 i = 0; i < farfiles.size(); ++i){
        farfiles[i].relativeTo(ZPath::pwd());
        LOG(farfiles[i]);
    }

    TASSERT(farfiles.size() == 2);
    zu64 findf = farfiles.find(ZPath("testdir/a/b/c/d/e/f.dat"));
    TASSERT(findf != ZArray<ZPath>::NONE);
    zu64 finds = farfiles.find(ZPath("testdir/near.dat"));
    TASSERT(finds != ZArray<ZPath>::NONE);
}

void file_read(){
    ZPath near = "testdir";

    ZArray<ZPath> nearfiles = ZFile::listFiles(near, false);
    for(zu64 i = 0; i < nearfiles.size(); ++i){
        nearfiles[i].relativeTo(ZPath::pwd());
        LOG(nearfiles[i]);
    }

    ZArray<ZPath> farfiles = ZFile::listFiles(near, true);
    for(zu64 i = 0; i < farfiles.size(); ++i){
        farfiles[i].relativeTo(ZPath::pwd());
        LOG(farfiles[i]);
    }

    for(zu64 i = 0; i < nearfiles.size(); ++i){
        LOG(nearfiles[i] << " - " << ZFile::readString(nearfiles[i]));
    }

    TASSERT(ZFile::readString(nearfiles.front()) == "this file is nearby");

    for(zu64 i = 0; i < farfiles.size(); ++i){
        LOG(farfiles[i] << " - " << ZFile::readString(farfiles[i]));
    }

    zu64 findf = farfiles.find(ZPath("testdir/a/b/c/d/e/f.dat"));
    zu64 finds = farfiles.find(ZPath("testdir/near.dat"));

    TASSERT(findf != ZArray<int>::NONE);
    TASSERT(finds != ZArray<int>::NONE);
    TASSERT(ZFile::readString(farfiles[findf]) == "this file is far away");
    TASSERT(ZFile::readString(farfiles[finds]) == "this file is nearby");
}

void file_list_dirs(){
    ZPath near = "testdir";

    ZArray<ZPath> dirs = ZFile::listDirs(near, true);
    for(zu64 i = 0; i < dirs.size(); ++i){
        LOG(dirs[i]);
    }

    TASSERT(dirs.size() == 5);
}

void file_sequential_rw(){
    ZFile rfile("testrandom", ZFile::READWRITE);

    ZRandom random;
    ZBinary rand1 = random.generate(0x100);
    TASSERT(rfile.write(rand1) == 0x100);
    ZBinary rand2 = random.generate(0x100);
    TASSERT(rfile.write(rand2) == 0x100);
    ZBinary rand3 = random.generate(0x100);
    TASSERT(rfile.write(rand3) == 0x100);
    ZBinary rand4 = random.generate(0x100);
    TASSERT(rfile.write(rand4) == 0x100);

    rfile.rewind();

    ZBinary randi;
    rfile.read(randi, 0x100);
    TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand1).hash());
    rfile.read(randi, 0x100);
    TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand2).hash());
    rfile.read(randi, 0x100);
    TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand3).hash());
    rfile.read(randi, 0x100);
    TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand4).hash());
}

ZArray<Test> file_tests(){
    return {
        { "file-create-dir",    file_create_dir,    true, {} },
        { "file-write",         file_write,         true, { "file-create-dir" } },
        { "file-list",          file_list,          true, { "file-write", "path-relative" } },
        { "file-read",          file_read,          true, { "file-list" } },
        { "file-list-dirs",     file_list_dirs,     true, { "file-create-dir" } },
        { "file-sequential-rw", file_create_dir,    true, {} },
    };
}

}
