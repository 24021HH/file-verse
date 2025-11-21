#pragma once
#include <vector>
#include <cstdint>
using namespace std;

class Bitmap {
private:
vector<uint8_t> bits;
int nbits;


inline void setBitRaw(int idx, bool v) {
int byteIdx = idx / 8;
int bit = idx % 8;
if (v) bits[byteIdx] |= (1 << bit);
else bits[byteIdx] &= ~(1 << bit);
}
inline bool getBitRaw(int idx) const {
int byteIdx = idx / 8;
int bit = idx % 8;
return (bits[byteIdx] >> bit) & 1;
}


public:
Bitmap(int size = 1024) : nbits(size) {
bits.assign((size + 7) / 8, 0);
}


void set(int index, bool value) {
if (index >= 0 && index < nbits)
setBitRaw(index, value);
}


bool get(int index) const {
if (index >= 0 && index < nbits)
return getBitRaw(index);
return false;
}


int findFree(int required = 1) const {
if (required <= 1) {
for (int i = 0; i < nbits; ++i)
if (!getBitRaw(i)) return i;
return -1;
}
int run = 0;
for (int i = 0; i < nbits; ++i) {
if (!getBitRaw(i)) run++; else run = 0;
if (run == required) return i - required + 1;
}
return -1;
}


int totalFree() const {
int count = 0;
for (int i = 0; i < nbits; ++i)
if (!getBitRaw(i)) ++count;
return count;
}


int size() const { return nbits; }
};