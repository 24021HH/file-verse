#ifndef DIRECTORY_TREE_H
#define DIRECTORY_TREE_H
using namespace std;
#include <string>
#include <vector>
#include <sstream>
#include "../source/include/odf_types.hpp"

struct DirectoryNode {
    string name;
    vector<FileEntry> files;
    vector<DirectoryNode*> subDirs;
    DirectoryNode* parent;

    DirectoryNode(string n = "/", DirectoryNode* p = nullptr)
        : name(n), parent(p) {}
};

class DirectoryTree {
private:
    DirectoryNode* root;

    vector<string> splitPath(const string& path) {
        vector<string> parts;
        if (path.empty() || path == "/") return parts;
        
        string trimmed = path;
        if (trimmed[0] == '/') trimmed = trimmed.substr(1);
        if (!trimmed.empty() && trimmed.back() == '/') trimmed.pop_back();
        
        stringstream ss(trimmed);
        string item;
        while (getline(ss, item, '/')) {
            if (!item.empty()) parts.push_back(item);
        }
        return parts;
    }

public:
    DirectoryTree() {
        root = new DirectoryNode("/");
    }

    DirectoryNode* getRoot() { return root; }

    void addFile(DirectoryNode* dir, const std::string& filename, uint64_t size, uint32_t inode) {
        FileEntry f(
            filename,
            EntryType::FILE, 
            size,
            0644,            
            "root",         
            inode        
        );
        dir->files.push_back(f);
    }

    DirectoryNode* addSubDir(DirectoryNode* parent, const std::string& dirname, uint32_t inode = 0) {
        DirectoryNode* node = new DirectoryNode(dirname, parent);
        FileEntry dirEntry(
            dirname,
            EntryType::DIRECTORY,
            0,              
            0755,           
            "root",
            inode
        );
        parent->files.push_back(dirEntry);
        parent->subDirs.push_back(node);
        return node;
    }

    DirectoryNode* findParentDir(const string& path) {
        vector<string> parts = splitPath(path);
        if (parts.empty()) return root;
        
        DirectoryNode* current = root;
        
        for (size_t i = 0; i < parts.size() - 1; i++) {
            bool found = false;
            for (auto subdir : current->subDirs) {
                if (subdir->name == parts[i]) {
                    current = subdir;
                    found = true;
                    break;
                }
            }
            if (!found) return nullptr; 
        }
        return current;
    }

    DirectoryNode* findDir(const string& path) {
        vector<string> parts = splitPath(path);
        if (parts.empty() || path == "/") return root;
        
        DirectoryNode* current = root;
        for (const auto& part : parts) {
            bool found = false;
            for (auto subdir : current->subDirs) {
                if (subdir->name == part) {
                    current = subdir;
                    found = true;
                    break;
                }
            }
            if (!found) return nullptr;
        }
        return current;
    }

    FileEntry* findFile(DirectoryNode* dir, const string& filename) {
        if (!dir) return nullptr;
        for (auto& entry : dir->files) {
            if (entry.getType() == EntryType::FILE && string(entry.name) == filename) {
                return &entry;
            }
        }
        return nullptr;
    }

    bool deleteFile(DirectoryNode* dir, const string& filename) {
        if (!dir) return false;
        for (size_t i = 0; i < dir->files.size(); i++) {
            if (dir->files[i].getType() == EntryType::FILE && string(dir->files[i].name) == filename) {
                dir->files.erase(dir->files.begin() + i);
                return true;
            }
        }
        return false;
    }

    bool deleteDir(DirectoryNode* parent, const string& dirname) {
        if (!parent) return false;
        
        for (size_t i = 0; i < parent->subDirs.size(); i++) {
            if (parent->subDirs[i]->name == dirname) {
                if (!parent->subDirs[i]->files.empty() || !parent->subDirs[i]->subDirs.empty()) {
                    return false; 
                }
                delete parent->subDirs[i];
                parent->subDirs.erase(parent->subDirs.begin() + i);
                
                for (size_t j = 0; j < parent->files.size(); j++) {
                    if (parent->files[j].getType() == EntryType::DIRECTORY && string(parent->files[j].name) == dirname) {
                        parent->files.erase(parent->files.begin() + j);
                        break;
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool pathExists(const string& path) {
        vector<string> parts = splitPath(path);
        if (parts.empty() || path == "/") return true;
        
        DirectoryNode* current = root;
        for (size_t i = 0; i < parts.size(); i++) {
            bool found = false;
            
            if (i == parts.size() - 1) {
                FileEntry* file = findFile(current, parts[i]);
                if (file) return true;
            }
            
            for (auto subdir : current->subDirs) {
                if (subdir->name == parts[i]) {
                    current = subdir;
                    found = true;
                    break;
                }
            }
            if (!found && i < parts.size() - 1) return false;  
        }
        return false;
    }
};

#endif
