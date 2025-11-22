#ifndef OFS_CORE_H
#define OFS_CORE_H
#include "../source/include/odf_types.hpp"
#include "../source/hashmap.h"
#include "../source/bitmap.h"
#include "../source/directory_tree.h"
#include <string>
#include <vector>
using namespace std;


struct OFSInstance {
    OMNIHeader header;
    Bitmap freeMap;
    DirectoryTree dirTree;
    vector<UserInfo> users;
    HashMap userIndex;
    bool initialized;
    std::string diskPath;
    FILE* disk_file;

    OFSInstance(int blocks = 1024) : freeMap(blocks), userIndex(128), initialized(false), disk_file(nullptr) {}
    
    ~OFSInstance() {
        if (disk_file) {
            fclose(disk_file);
            disk_file = nullptr;
        }
    }
};

int fs_init(OFSInstance &fs, const std::string &diskPath);
int fs_format(OFSInstance &fs, uint64_t totalSize, uint64_t blockSize, const std::string &diskPath);
int fs_shutdown(OFSInstance &fs);

#endif 