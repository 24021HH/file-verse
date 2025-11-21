#ifndef HASHMAP_H
#define HASHMAP_H
#include <vector>
#include <string>
#include <utility>
using namespace std;


struct HashNode {
    string key;
    int value;
    bool occupied;
    HashNode() : key(""), value(-1), occupied(false) {}
};

class HashMap {
private:
    int size;
    vector<HashNode> table;

    int hashFunc(const string &key) const {
        unsigned long hash = 5381;
        for (char c : key)
            hash = ((hash << 5) + hash) + c; 
        return hash % size;
    }

public:
    HashMap(int sz = 100) : size(sz), table(sz) {}

    void insert(const string &key, int val) {
        int idx = hashFunc(key);
        while (table[idx].occupied) {
            if (table[idx].key == key) {
                table[idx].value = val;
                return;
            }
            idx = (idx + 1) % size;
        }
        table[idx].key = key;
        table[idx].value = val;
        table[idx].occupied = true;
    }

    bool find(const string &key, int &val) {
        int idx = hashFunc(key);
        int start = idx;
        while (table[idx].occupied) {
            if (table[idx].key == key) {
                val = table[idx].value;
                return true;
            }
            idx = (idx + 1) % size;
            if (idx == start) break;
        }
        return false;
    }

    bool remove(const string &key) {
        int idx = hashFunc(key);
        int start = idx;
        while (table[idx].occupied) {
            if (table[idx].key == key) {
                table[idx].occupied = false;
                return true;
            }
            idx = (idx + 1) % size;
            if (idx == start) break;
        }
        return false;
    }
};

#endif
