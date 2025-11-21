#include "../source/ofs_core.h"
#include <iostream>
using namespace std;

int main() {
    cout << "Core Design & Functional Implementation\n\n";

    const string diskPath = "sample.omni";

    uint64_t totalSize = 10 * 1024 * 1024; 
    uint64_t blockSize = 4096;           

    OFSInstance fs;

    cout << " Formatting new OFS container...\n";
    int res = fs_format(fs, totalSize, blockSize, diskPath);
    if (res != OFS_SUCCESS) {
        cerr << "Format failed with code " << res << "\n";
        return 1;
    }

    cout << "Initializing from disk...\n";
    OFSInstance fs2;
    res = fs_init(fs2, diskPath);
    if (res != OFS_SUCCESS) {
        cerr << "Init failed with code " << res << "\n";
        return 1;
    }

    cout << "File system ready in memory.\n";
    cout << "    Header Info:\n";
    cout << "    Magic: " << string(fs2.header.magic, 8) << "\n";
    cout << "    Version: " << fs2.header.format_version << "\n";
    cout << "    Block size: " << fs2.header.block_size << " bytes\n";
    cout << "    Total size: " << fs2.header.total_size / (1024 * 1024) << " MB\n";

    cout << "\nShutting down FS...\n";
    fs_shutdown(fs2);

    cout << "\n Test complete. 'sample.omni' has been created in this directory.\n";
    cout << "You can now inspect the binary file to verify its header.\n";
    return 0;
}
