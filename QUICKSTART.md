# OFS File Operations - Quick Start

## 🚀 Quick Start (5 Minutes)

### Step 1: Build the Project
```bash
cd source
make
```

### Step 2: Format and Initialize the File System
```bash
cd source
./ofstest

# In the ofstest prompt:
FORMAT 10485760 4096 ofs.omni
INIT ofs.omni
CREATE /welcome.txt "Welcome to OFS!" admin
LIST /
SHUTDOWN
```

### Step 3: Start the Server
```bash
# Terminal 1 - Start OFS Server
cd source
./ofsserver ofs.omni
```

### Step 4: Start the Web UI
```bash
# Terminal 2 - Start Web Server
cd ui
python3 server.py
```

### Step 5: Access the File Manager
1. Open browser to: **http://localhost:8000/filemanager.html**
2. Login with:
   - **Username**: `admin`
   - **Password**: `admin123`

### Step 6: Try File Operations

**Create a File:**
1. Click "📄 Create File"
2. Enter filename: `test.txt`
3. Enter content: `Hello from OFS!`
4. Click "Create File"

**Read a File:**
1. Click the "Read" button on any file
2. View the contents in a popup

**Edit a File:**
1. Click the "Edit" button on your file
2. Modify the content
3. Click "Save Changes"

**Delete a File:**
1. Click the "Delete" button on your file
2. Confirm deletion

## 🎯 Key Features Implemented

✅ **Full File Operations**
- Create files with content
- Read file contents
- Edit/update file contents
- Delete files

✅ **User Permissions**
- Each file has an owner
- Users can only modify their own files
- Admin can access all files

✅ **Web Interface**
- Modern, responsive UI
- Real-time file operations
- Separate windows for viewing/editing

✅ **Persistent Storage**
- Files stored in binary .omni format
- Block-based allocation
- Survives server restart

## 📋 Testing Permissions

Currently, only the `admin` user exists. The permission system is fully implemented:

```cpp
// In file_operations.h
if (entry->owner != requester.username && 
    requester.role != UserRole::ADMIN) {
    cerr << "Permission denied\n";
    return false;
}
```

When you add more users, each user will only be able to access their own files!

## 🔧 API Examples

### Create File
```bash
curl -X POST http://localhost:8080 -H "Content-Type: application/json" -d '{
  "operation": "CREATE",
  "request_id": "1",
  "parameters": {
    "path": "/myfile.txt",
    "data": "Hello, World!",
    "owner": "admin"
  }
}'
```

### Read File
```bash
curl -X POST http://localhost:8080 -H "Content-Type: application/json" -d '{
  "operation": "READ",
  "request_id": "2",
  "parameters": {
    "path": "/myfile.txt",
    "user": "admin"
  }
}'
```

### Edit File
```bash
curl -X POST http://localhost:8080 -H "Content-Type: application/json" -d '{
  "operation": "EDIT",
  "request_id": "3",
  "parameters": {
    "path": "/myfile.txt",
    "data": "Updated content!",
    "user": "admin"
  }
}'
```

### Delete File
```bash
curl -X POST http://localhost:8080 -H "Content-Type: application/json" -d '{
  "operation": "DELETE",
  "request_id": "4",
  "parameters": {
    "path": "/myfile.txt",
    "user": "admin"
  }
}'
```

## 📁 Project Structure

```
file-verse/
├── source/
│   ├── ofs_core.h              # Core structures
│   ├── file_operations.h       # File ops with permissions
│   ├── request_handler.cpp     # API routing
│   ├── fs_init.cpp             # Format/init
│   ├── server.cpp              # TCP server
│   ├── ofsserver              # Server binary
│   └── ofstest                # Test client
├── ui/
│   ├── filemanager.html        # File manager UI ⭐
│   ├── login.html              # Login page
│   └── server.py               # Web server
└── FILE_OPERATIONS_GUIDE.md    # Detailed guide
```

## 🎓 What's Implemented

### Backend (C++)
- ✅ File create with block allocation
- ✅ File read with actual disk I/O
- ✅ File edit with reallocation
- ✅ File delete with permission check
- ✅ Owner-based permission system
- ✅ Admin override for all operations
- ✅ JSON protocol support

### Frontend (JavaScript)
- ✅ Modern file manager UI
- ✅ Create/read/edit/delete operations
- ✅ Modal windows for file content
- ✅ Real-time updates
- ✅ User authentication
- ✅ Error handling and notifications

## 🐛 Troubleshooting

**Server won't start:**
```bash
# Make sure no other process is using port 8080
lsof -i :8080
kill -9 <PID>
```

**UI shows "Connection error":**
- Check that ofsserver is running on port 8080
- Check that server.py is running on port 8000

**Permission denied errors:**
- Verify you're logged in as the correct user
- Check file ownership with LIST command

**File not found:**
- Ensure file was created with CREATE
- Check the full path starts with /

## 🎬 Demo Script

Run the automated demo:
```bash
./demo_file_operations.sh
```

This will:
1. Format a new filesystem
2. Initialize it
3. Create sample files
4. Start the server
5. Show instructions for the web UI

## 📖 Full Documentation

For detailed architecture, permission system details, and advanced usage, see:
- **FILE_OPERATIONS_GUIDE.md** - Complete technical documentation

## ✨ Key Highlights

1. **Complete CRUD Operations**: All file operations fully implemented
2. **Real Disk I/O**: Files actually written to and read from disk
3. **Permission System**: Users can only modify their own files
4. **Modern UI**: Beautiful web interface with separate edit windows
5. **Production Ready**: Error handling, validation, and persistence

Enjoy your fully functional file system! 🎉
