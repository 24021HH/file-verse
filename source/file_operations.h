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
        
        // Write data to disk if available
        if (fs.disk_file && dataSize > 0) {
            uint64_t data_offset = fs.header.data_blocks_offset;
            uint64_t write_offset = data_offset + (firstBlock * fs.header.block_size);
            
            fseeko(fs.disk_file, write_offset, SEEK_SET);
            fwrite(data.c_str(), 1, dataSize, fs.disk_file);
            fflush(fs.disk_file);
        }
        
        FileEntry entry(filename, EntryType::FILE, dataSize, 0644, owner.username, inode);
        parent->files.push_back(entry);
        
        std::cout << "File created: " << path << " (inode=" << inode << ", blocks=" << blocksNeeded << ") by " << owner.username << "\n";
        return inode;
    }
    

    static std::string file_read(OFSInstance& fs, const std::string& path, UserInfo& requester) {
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
        
        // Permission check: only owner or admin can read
        if (std::string(entry->owner) != requester.username && requester.role != UserRole::ADMIN) {
            std::cerr << "Permission denied: " << requester.username << " cannot read file owned by " << entry->owner << "\n";
            return "";
        }
        
        // For now, return the stored content from memory
        // In a real implementation, this would read from disk blocks
        std::string content = "";
        
        // Try to read from disk if available
        if (fs.disk_file && entry->size > 0 && entry->inode >= 1000) {
            uint64_t data_offset = fs.header.data_blocks_offset;
            uint64_t start_block = (entry->inode - 1000);  // Simple mapping
            
            uint64_t block_offset = data_offset + (start_block * fs.header.block_size);
            
            char* buffer = new char[entry->size + 1];
            fseeko(fs.disk_file, block_offset, SEEK_SET);
            size_t bytes_read = fread(buffer, 1, entry->size, fs.disk_file);
            buffer[bytes_read] = '\0';
            content = std::string(buffer, bytes_read);
            delete[] buffer;
        }
        
        std::cout << "Read file: " << path << " (" << entry->size << " bytes) by " << requester.username << "\n";
        std::cout << "Content: [" << content << "]\n";
        return content;
    }
    
    static bool file_edit(OFSInstance& fs, const std::string& path, const std::string& new_data, UserInfo& requester) {
        DirectoryNode* parent = fs.dirTree.findParentDir(path);
        if (!parent) {
            std::cerr << " Parent directory not found\n";
            return false;
        }
        
        size_t last_slash = path.find_last_of('/');
        std::string filename = path.substr(last_slash + 1);
        
        FileEntry* entry = fs.dirTree.findFile(parent, filename);
        if (!entry) {
            std::cerr << " File not found: " << path << "\n";
            return false;
        }
        
        // Permission check: only owner or admin can edit
        if (std::string(entry->owner) != requester.username && requester.role != UserRole::ADMIN) {
            std::cerr << "Permission denied: " << requester.username << " cannot edit file owned by " << entry->owner << "\n";
            return false;
        }
        
        // Free old blocks
        uint64_t old_blocks = (entry->size + fs.header.block_size - 1) / fs.header.block_size;
        uint64_t start_block = entry->inode % 1000;
        
        for (uint64_t i = 0; i < old_blocks; i++) {
            fs.freeMap.set(start_block + i, false);
        }
        
        // Allocate new blocks for new data
        uint64_t new_size = new_data.length();
        uint64_t new_blocks = (new_size + fs.header.block_size - 1) / fs.header.block_size;
        
        int firstBlock = fs.freeMap.findFree(new_blocks);
        if (firstBlock < 0) {
            std::cerr << " No free space for file edit\n";
            // Try to restore old blocks
            for (uint64_t i = 0; i < old_blocks; i++) {
                fs.freeMap.set(start_block + i, true);
            }
            return false;
        }
        
        for (uint64_t i = 0; i < new_blocks; i++) {
            fs.freeMap.set(firstBlock + i, true);
        }
        
        // Write new data to disk
        if (fs.disk_file) {
            uint64_t data_offset = fs.header.data_blocks_offset;
            uint64_t write_offset = data_offset + (firstBlock * fs.header.block_size);
            
            fseeko(fs.disk_file, write_offset, SEEK_SET);
            fwrite(new_data.c_str(), 1, new_size, fs.disk_file);
            fflush(fs.disk_file);
        }
        
        // Update file entry
        entry->size = new_size;
        entry->inode = 1000 + firstBlock; // Update inode to point to new blocks
        
        std::cout << "File edited: " << path << " (new size: " << new_size << " bytes) by " << requester.username << "\n";
        return true;
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
