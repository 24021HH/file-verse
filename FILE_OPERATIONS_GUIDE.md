# OFS File Operations - Complete Guide

## Overview
This implementation provides full file operations with user-specific permissions. Each user can only view and modify their own files, except administrators who have full access.

## Features Implemented

### 1. File Operations
- ✅ **Create**: Create new files with content
- ✅ **Read**: Read file contents (with permission check)
- ✅ **Edit**: Modify file contents (with permission check)
- ✅ **Delete**: Remove files (with permission check)

### 2. Directory Operations
- ✅ **Create**: Create new directories
- ✅ **List**: List directory contents
- ✅ **Delete**: Remove empty directories

### 3. Permission System
- ✅ **Owner-based**: Each file has an owner
- ✅ **Permission Checks**: Users can only modify their own files
- ✅ **Admin Override**: Admin users can access all files

## Architecture

### Backend (C++)

#### Core Components

**1. OFSInstance (ofs_core.h)**
```cpp
struct OFSInstance {
    OMNIHeader header;           // File system metadata
    Bitmap freeMap;              // Block allocation tracking
    DirectoryTree dirTree;       // File/directory hierarchy
    vector<UserInfo> users;      // User database
    HashMap userIndex;           // Username → user index mapping
    bool initialized;            // FS state
    string diskPath;             // Path to .omni file
    FILE* disk_file;             // Persistent file handle
}
```

**2. FileOps Class (file_operations.h)**

All operations are static methods that take the file system instance and user information:

```cpp
static int file_create(OFSInstance& fs, const string& path, 
                       const string& data, UserInfo& owner);

static string file_read(OFSInstance& fs, const string& path, 
                        UserInfo& requester);

static bool file_edit(OFSInstance& fs, const string& path, 
                      const string& new_data, UserInfo& requester);

static bool file_delete(OFSInstance& fs, const string& path, 
                        UserInfo& requester);
```

**3. Request Handler (request_handler.cpp)**

Routes JSON requests to FileOps methods:

```cpp
// Example JSON request for creating a file
{
    "operation": "CREATE",
    "request_id": "123456789",
    "parameters": {
        "path": "/myfile.txt",
        "data": "Hello, world!",
        "owner": "admin"
    }
}
```

### Frontend (HTML/JavaScript)

**File Manager UI (filemanager.html)**
- Modern, responsive interface
- File browser with list view
- Modals for create/edit/read operations
- Real-time file operations via JSON protocol

**Communication Protocol**
```javascript
async function sendCommand(operation, params) {
    const response = await fetch('http://localhost:8080', {
        method: 'POST',
        body: JSON.stringify({
            operation: operation,
            request_id: Date.now().toString(),
            parameters: params
        })
    });
}
```

## Permission System Details

### File Ownership
Every file has an owner field (username). Set during creation.

```cpp
FileEntry entry(filename, EntryType::FILE, dataSize, 
                0644, owner.username, inode);
```

### Permission Checks

**Read Operation**
```cpp
if (entry->owner != requester.username && 
    requester.role != UserRole::ADMIN) {
    cerr << "Permission denied\n";
    return "";
}
```

**Edit Operation**
```cpp
if (entry->owner != requester.username && 
    requester.role != UserRole::ADMIN) {
    cerr << "Permission denied\n";
    return false;
}
```

**Delete Operation**
```cpp
if (entry->owner != requester.username && 
    requester.role != UserRole::ADMIN) {
    cerr << "Permission denied\n";
    return false;
}
```

### Admin Privileges
- Username: `admin`
- Password: `admin123`
- Role: `UserRole::ADMIN`
- Can access all files regardless of owner

## File Storage

### Block Allocation
Files are stored in blocks on disk:

```cpp
uint64_t blocksNeeded = (dataSize + block_size - 1) / block_size;
int firstBlock = freeMap.findFree(blocksNeeded);
```

### Disk Layout
```
[Header: 512 bytes]
[User Table: 50 * 128 bytes]
[File State Storage: 65536 bytes]
[Change Log: 131072 bytes]
[Data Blocks: remainder of disk]
```

### Read/Write Operations

**Write (during create/edit)**
```cpp
uint64_t write_offset = data_blocks_offset + 
                        (firstBlock * block_size);
fseeko(disk_file, write_offset, SEEK_SET);
fwrite(data.c_str(), 1, dataSize, disk_file);
fflush(disk_file);
```

**Read**
```cpp
uint64_t block_offset = data_blocks_offset + 
                        ((start_block + i) * block_size);
fseeko(disk_file, block_offset, SEEK_SET);
fread(buffer, 1, bytes_to_read, disk_file);
```

## Usage Examples

### Command Line

**1. Format and Initialize**
```bash
./ofstest
> FORMAT 10485760 4096 ofs.omni
> INIT ofs.omni
```

**2. Create Files**
```bash
> CREATE /test.txt "Hello World" admin
> CREATE /data.json "{\"name\":\"test\"}" admin
```

**3. Read Files**
```bash
> READ /test.txt admin
```

**4. Delete Files**
```bash
> DELETE /test.txt admin
```

### Web UI

**1. Start the Server**
```bash
cd source
./ofsserver ofs.omni
```

**2. Start Web Server**
```bash
cd ui
python3 server.py
```

**3. Access File Manager**
- Open browser to: `http://localhost:8000/filemanager.html`
- Login with: `admin / admin123`

**4. Operations**
- Click "Create File" to create a new file
- Click "Read" on any file to view contents
- Click "Edit" on your own files to modify them
- Click "Delete" on your own files to remove them

### JSON API

**Create File**
```bash
curl -X POST http://localhost:8080 \
  -H "Content-Type: application/json" \
  -d '{
    "operation": "CREATE",
    "request_id": "1",
    "parameters": {
      "path": "/myfile.txt",
      "data": "File contents here",
      "owner": "admin"
    }
  }'
```

**Read File**
```bash
curl -X POST http://localhost:8080 \
  -H "Content-Type: application/json" \
  -d '{
    "operation": "READ",
    "request_id": "2",
    "parameters": {
      "path": "/myfile.txt",
      "user": "admin"
    }
  }'
```

**Edit File**
```bash
curl -X POST http://localhost:8080 \
  -H "Content-Type: application/json" \
  -d '{
    "operation": "EDIT",
    "request_id": "3",
    "parameters": {
      "path": "/myfile.txt",
      "data": "Updated content",
      "user": "admin"
    }
  }'
```

**Delete File**
```bash
curl -X POST http://localhost:8080 \
  -H "Content-Type: application/json" \
  -d '{
    "operation": "DELETE",
    "request_id": "4",
    "parameters": {
      "path": "/myfile.txt",
      "user": "admin"
    }
  }'
```

## Testing the Permission System

### Scenario 1: Admin Creates and Manages Files
```bash
# Admin creates a file
CREATE /admin_file.txt "Admin's data" admin

# Admin reads it
READ /admin_file.txt admin

# Admin edits it
EDIT /admin_file.txt "Updated data" admin

# Admin deletes it
DELETE /admin_file.txt admin
```

### Scenario 2: User Access Control (When multiple users exist)

When you implement multi-user support:

```bash
# User1 creates a file
CREATE /user1_file.txt "User1's data" user1

# User1 can read it
READ /user1_file.txt user1  # ✅ SUCCESS

# User2 tries to read it
READ /user1_file.txt user2  # ❌ PERMISSION DENIED

# Admin can read it
READ /user1_file.txt admin  # ✅ SUCCESS (admin override)
```

## File Formats

### .omni Binary Format
- Magic: "OMNIFS01"
- Version: 0x00010000
- Block-based storage
- User table embedded
- Persistent file handles

### JSON Protocol
All API communication uses JSON:
```json
{
  "operation": "CREATE|READ|EDIT|DELETE|LIST",
  "request_id": "unique_id",
  "parameters": {
    // operation-specific parameters
  }
}
```

### Response Format
```json
{
  "operation": "CREATE",
  "request_id": "123",
  "status": "success|error",
  "error_code": 0,
  "error_message": "",
  "data": {
    // operation-specific data
  }
}
```

## Running the Demo

```bash
# Make the demo script executable
chmod +x demo_file_operations.sh

# Run the demo
./demo_file_operations.sh
```

This will:
1. Format a new file system
2. Initialize it
3. Create sample files
4. Start the server
5. Show you how to access the web UI

## Troubleshooting

### "Permission denied" when reading/editing
- Check that you're logged in as the file owner or admin
- Verify the file ownership with LIST command

### "File not found"
- Ensure the file was created successfully
- Check the path (should start with /)

### Connection errors in UI
- Verify the server is running on port 8080
- Check that the web server is running on port 8000
- Ensure no firewall is blocking the ports

### Compilation errors
```bash
cd source
make clean  # If Makefile supports it
make
```

## Next Steps

### Potential Enhancements
1. **User Registration**: Add user signup functionality
2. **Shared Files**: Implement file sharing between users
3. **File Metadata**: Show creation time, last modified, etc.
4. **Search**: Add file search functionality
5. **Bulk Operations**: Select and delete multiple files
6. **File Upload**: Upload files from local system
7. **Download**: Download files to local system

## Code Structure

```
file-verse/
├── source/
│   ├── ofs_core.h              # Core data structures
│   ├── file_operations.h       # File operation implementations
│   ├── request_handler.cpp     # JSON API routing
│   ├── fs_init.cpp             # FS format/init/shutdown
│   ├── server.cpp              # TCP server
│   ├── main_server.cpp         # Server entry point
│   └── include/
│       └── odf_types.hpp       # Type definitions
├── ui/
│   ├── filemanager.html        # File manager UI
│   ├── login.html              # Login page
│   └── server.py               # Web server
└── demo_file_operations.sh     # Demo script
```

## Summary

This implementation provides a complete file system with:
- Full CRUD operations on files
- User-based permissions
- Admin override capabilities
- Web-based UI
- JSON API
- Persistent storage in .omni format

Users can create, read, edit, and delete their own files, while administrators have full access to all files in the system.
