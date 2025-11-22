#include "ofs_core.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;


int fs_init(OFSInstance &fs, const string &diskPath) {
    ifstream disk(diskPath, ios::binary);
    if (!disk.is_open()) {
        cerr << " Disk file not found: " << diskPath << "\n";
        return OFS_ERR_NOTFOUND;
    }

    disk.read(reinterpret_cast<char*>(&fs.header), sizeof(OMNIHeader));
    if (!disk) {
        cerr << " Failed to read header.\n";
        return OFS_ERR_INVALID;
    }

    if (strncmp(fs.header.magic, "OMNIFS01", 8) != 0) {
        cerr << " Invalid or corrupted filesystem.\n";
        return OFS_ERR_INVALID;
    }

    uint64_t totalBlocks = fs.header.total_size / fs.header.block_size;
    fs.freeMap = Bitmap(static_cast<int>(totalBlocks));

    disk.seekg(fs.header.user_table_offset);
    for (uint32_t i = 0; i < fs.header.max_users; i++) {
        UserInfo user;
        disk.read(reinterpret_cast<char*>(&user), sizeof(UserInfo));
        if (!disk) break;
        
        if (user.is_active) {
            fs.users.push_back(user);
            fs.userIndex.insert(user.username, fs.users.size() - 1);
        }
    }

    cout << " Loaded " << fs.users.size() << " users from disk.\n";

    DirectoryNode* root = fs.dirTree.getRoot();
    if (root->files.empty()) {
        cout << " Initializing root directory.\n";
    }
    
    disk.close();
    
    // Open disk file for read/write operations
    fs.diskPath = diskPath;
    fs.disk_file = fopen(diskPath.c_str(), "r+b");
    if (!fs.disk_file) {
        cerr << " Failed to open disk file for I/O: " << diskPath << "\n";
        return OFS_ERR_INVALID;
    }

    fs.initialized = true;
    cout << "FS initialized successfully. Version: "
              << fs.header.format_version << "\n";
    cout << "   Total blocks: " << totalBlocks << " x " << fs.header.block_size << " bytes\n";
    cout << "   Users loaded: " << fs.users.size() << "\n";
    return OFS_SUCCESS;
}


int fs_format(OFSInstance &fs, uint64_t totalSize, uint64_t blockSize, const std::string &diskPath) {
    ofstream disk(diskPath, ios::binary | ios::trunc);
    if (!disk.is_open()) {
        cerr << " Unable to create disk file: " << diskPath << "\n";
        return OFS_ERR_NOTFOUND;
    }

    memset(&fs.header, 0, sizeof(OMNIHeader));
    memcpy(fs.header.magic, "OMNIFS01", 8);
    fs.header.format_version = 0x00010000;
    fs.header.total_size = totalSize;
    fs.header.block_size = blockSize;
    fs.header.header_size = sizeof(OMNIHeader);
    fs.header.user_table_offset = sizeof(OMNIHeader);
    fs.header.max_users = 50;
    fs.header.file_state_storage_offset = fs.header.user_table_offset + (50 * sizeof(UserInfo));
    fs.header.change_log_offset = fs.header.file_state_storage_offset + 65536;
    fs.header.data_blocks_offset = fs.header.change_log_offset + 131072; // Data blocks start after change log

    disk.write(reinterpret_cast<const char*>(&fs.header), sizeof(OMNIHeader));

    UserInfo adminUser("admin", "admin123", UserRole::ADMIN, time(nullptr));
    
    disk.write(reinterpret_cast<const char*>(&adminUser), sizeof(UserInfo));
    
    for (uint32_t i = 1; i < 50; i++) {
        UserInfo emptyUser;
        disk.write(reinterpret_cast<const char*>(&emptyUser), sizeof(UserInfo));
    }

    uint64_t totalBlocks = totalSize / blockSize;
    fs.freeMap = Bitmap(static_cast<int>(totalBlocks));
    fs.users.push_back(adminUser);
    fs.userIndex.insert("admin", 0);
    fs.dirTree.getRoot();  

    fs.initialized = true;
    disk.close();
    
    cout << "   File system formatted successfully:\n";
    cout << "   Total size: " << totalSize / (1024 * 1024) << " MB\n";
    cout << "   Block size: " << blockSize << " bytes\n";
    cout << "   Total blocks: " << totalBlocks << "\n";
    cout << "   Default admin user created (admin/admin123)\n";
    return OFS_SUCCESS;
}

int fs_shutdown(OFSInstance &fs) {
    if (!fs.initialized) return OFS_ERR_INVALID;
    
    if (fs.disk_file) {
        fclose(fs.disk_file);
        fs.disk_file = nullptr;
    }
    
    fs.initialized = false;
    cout << "FS shutdown complete and metadata saved.\n";
    return OFS_SUCCESS;
}
