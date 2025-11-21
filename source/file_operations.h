#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <cstring>
using namespace std;
#include "ofs_core.h"
#include "directory_tree.h"
#include "include/odf_types.hpp"


class FileOps {
public:
  
    static int file_create(OFSInstance& fs, const std::string& path, const std::string& data, UserInfo& owner) {
        DirectoryNode* parent = fs.dirTree.findParentDir(path);
        if (!parent) {
            std::cerr << " Parent directory not found for path: " << path << "\n";
            return -1;
        }
        
        size_t last_slash = path.find_last_of('/');
        std::string filename = path.substr(last_slash + 1);
        
        if (fs.dirTree.findFile(parent, filename)) {
            std::cerr << " File already exists: " << path << "\n";
            return -1;
        }
        
        uint64_t dataSize = data.length();
        uint64_t blocksNeeded = (dataSize + fs.header.block_size - 1) / fs.header.block_size;
        
        int firstBlock = fs.freeMap.findFree(blocksNeeded);
        if (firstBlock < 0) {
            std::cerr << " No free space for file: " << path << "\n";
            return -1;
        }
        
        for (uint64_t i = 0; i < blocksNeeded; i++) {
            fs.freeMap.set(firstBlock + i, true);
        }
        
        static uint32_t inode_counter = 1000;
        uint32_t inode = inode_counter++;
        
        FileEntry entry(filename, EntryType::FILE, dataSize, 0644, owner.username, inode);
        parent->files.push_back(entry);
        
        std::cout << "File created: " << path << " (inode=" << inode << ", blocks=" << blocksNeeded << ")\n";
        return inode;
    }
    

    static std::string file_read(OFSInstance& fs, const std::string& path) {
        DirectoryNode* parent = fs.dirTree.findParentDir(path);
        if (!parent) {
            std::cerr << " Parent directory not found\n";
            return "";
        }
        
        size_t last_slash = path.find_last_of('/');
        std::string filename = path.substr(last_slash + 1);
        
        FileEntry* entry = fs.dirTree.findFile(parent, filename);
        if (!entry) {
            std::cerr << " File not found: " << path << "\n";
            return "";
        }
        
        std::cout << " Reading file: " << path << " (" << entry->size << " bytes)\n";
        return "[file data would be read from blocks]";
    }

    static bool file_delete(OFSInstance& fs, const std::string& path, UserInfo& requester) {
        DirectoryNode* parent = fs.dirTree.findParentDir(path);
        if (!parent) {
            std::cerr << " Parent directory not found\n";
            return false;
        }
        
        size_t last_slash = path.find_last_of('/');
        std::string filename = path.substr(last_slash + 1);
        
        FileEntry* entry = fs.dirTree.findFile(parent, filename);
        if (!entry) {
            std::cerr << "File not found: " << path << "\n";
            return false;
        }
        
        if (std::string(entry->owner) != requester.username && requester.role != UserRole::ADMIN) {
            std::cerr << "Permission denied: not file owner and not admin\n";
            return false;
        }
        
        uint64_t blocksUsed = (entry->size + fs.header.block_size - 1) / fs.header.block_size;
        (void)blocksUsed;  

        if (fs.dirTree.deleteFile(parent, filename)) {
            std::cout << "File deleted: " << path << "\n";
            return true;
        }
        
        return false;
    }
    

    static bool dir_create(OFSInstance& fs, const std::string& path, UserInfo& owner) {
        (void)owner;
        DirectoryNode* parent = fs.dirTree.findParentDir(path);
        if (!parent) {
            std::cerr << "Parent directory not found\n";
            return false;
        }
        
        size_t last_slash = path.find_last_of('/');
        std::string dirname = path.substr(last_slash + 1);
        
        if (fs.dirTree.findDir(path)) {
            std::cerr << "Directory already exists: " << path << "\n";
            return false;
        }
        
        static uint32_t inode_counter = 5000;
        fs.dirTree.addSubDir(parent, dirname, inode_counter++);
        
        std::cout << " Directory created: " << path << "\n";
        return true;
    }
    
    
    static std::string dir_list(OFSInstance& fs, const std::string& path) {
        DirectoryNode* dir;
        
        if (path == "/") {
            dir = fs.dirTree.getRoot();
        } else {
            dir = fs.dirTree.findDir(path);
        }
        
        if (!dir) {
            std::cerr << "Directory not found: " << path << "\n";
            return "";
        }
        
        std::string result = "";
        
        for (const auto& entry : dir->files) {
            if (entry.getType() == EntryType::FILE) {
                result += "[FILE] " + std::string(entry.name) + " (" + std::to_string(entry.size) + " bytes)\n";
            } else if (entry.getType() == EntryType::DIRECTORY) {
                result += "[DIR]  " + std::string(entry.name) + "\n";
            }
        }
        
        std::cout << "Listed directory: " << path << " (" << dir->files.size() << " entries)\n";
        return result;
    }
    
    
    static bool dir_delete(OFSInstance& fs, const std::string& path, UserInfo& requester) {
        (void)requester; 
        DirectoryNode* parent = fs.dirTree.findParentDir(path);
        if (!parent) {
            std::cerr << " Parent directory not found\n";
            return false;
        }
        
        DirectoryNode* dir = fs.dirTree.findDir(path);
        if (!dir) {
            std::cerr << " Directory not found: " << path << "\n";
            return false;
        }
        
        if (!dir->files.empty() || !dir->subDirs.empty()) {
            std::cerr << " Directory not empty\n";
            return false;
        }
        
        size_t last_slash = path.find_last_of('/');
        std::string dirname = path.substr(last_slash + 1);
        
        if (fs.dirTree.deleteDir(parent, dirname)) {
            std::cout << "Directory deleted: " << path << "\n";
            return true;
        }
        
        return false;
    }
};

#endif
