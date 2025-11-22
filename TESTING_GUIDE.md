# 🧪 OFS File Operations - Complete Testing Guide

## 📋 Prerequisites

Before testing, ensure you have:
- ✅ C++ project compiled successfully
- ✅ Python 3 installed (for web server)
- ✅ A modern web browser (Chrome, Firefox, Safari)
- ✅ Two terminal windows available

---

## 🚀 Step-by-Step Testing Instructions

### Phase 1: Prepare the File System (5 minutes)

#### Terminal 1 - Setup

```bash
# Navigate to the source directory
cd /Users/hamnahassan/Documents/file-verse/source

# Verify binaries exist
ls -la ofsserver ofstest

# If they don't exist, compile:
make

# Create and format a new file system
./ofstest
```

#### In the ofstest prompt, run these commands:

```
FORMAT 10485760 4096 ofs.omni
INIT ofs.omni
CREATE /readme.txt "Welcome to OFS! This is a test file." admin
CREATE /data.json "{\"name\":\"test\",\"value\":123}" admin
CREATE /notes.txt "Important notes here" admin
LIST /
SHUTDOWN
```

**Expected Output:**
```
✅ File system formatted successfully
✅ FS initialized successfully
✅ File created: /readme.txt (inode=1000, blocks=1) by admin
✅ File created: /data.json (inode=1001, blocks=1) by admin
✅ File created: /notes.txt (inode=1002, blocks=1) by admin
✅ Listed directory: / (3 entries)
```

---

### Phase 2: Start the Backend Server (Terminal 1)

```bash
# Still in /Users/hamnahassan/Documents/file-verse/source
./ofsserver ofs.omni
```

**Expected Output:**
```
FS initialized successfully. Version: 65536
   Total blocks: 2560 x 4096 bytes
   Users loaded: 1
Server listening on port 8080
Ready to accept connections...
```

**✅ Keep this terminal running!**

---

### Phase 3: Start the Web UI Server (Terminal 2)

Open a **NEW terminal window** and run:

```bash
# Navigate to the UI directory
cd /Users/hamnahassan/Documents/file-verse/ui

# Start the Python web server
python3 server.py
```

**Expected Output:**
```
Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
```

**Alternative if port 8000 is busy:**
```bash
python3 -m http.server 8001
# Then use http://localhost:8001 instead
```

**✅ Keep this terminal running too!**

---

## 🌐 Phase 4: Testing the Web UI

### Step 1: Access the Login Page

1. **Open your web browser**
2. **Navigate to**: `http://localhost:8000/login.html`
3. **You should see**: A beautiful purple gradient login page

### Step 2: Login

**Enter credentials:**
- Username: `admin`
- Password: `admin123`

**Click "Sign In"**

**Expected Result:**
- ✅ Green notification: "Login successful! Redirecting..."
- ✅ Automatically redirected to file manager

---

### Step 3: View Files

You should now see the **File Manager** interface with:
- Header showing "OFS File Manager" and username badge "admin"
- Left sidebar with action buttons
- Main area showing your files

**You should see 3 files:**
- 📄 `readme.txt` (48 bytes)
- 📄 `data.json` (27 bytes)
- 📄 `notes.txt` (21 bytes)

Each file should have **Read**, **Edit**, and **Delete** buttons.

---

### Step 4: Test READ Operation

1. **Click the "Read" button** on `readme.txt`
2. **A modal window should appear** with:
   - Title: "File Content"
   - Path: `/readme.txt`
   - Content: "Welcome to OFS! This is a test file."
3. **Click the X** to close the modal

**Expected in Terminal 1 (server):**
```
Read file: /readme.txt (48 bytes) by admin
```

**✅ Test passed if you see the file content!**

---

### Step 5: Test EDIT Operation

1. **Click the "Edit" button** on `readme.txt`
2. **A modal window should appear** with:
   - Title: "Edit: /readme.txt"
   - Text area with current content
3. **Modify the text**, for example:
   ```
   Welcome to OFS! This is a test file.
   This line was added via the web UI!
   Testing edit functionality.
   ```
4. **Click "Save Changes"**
5. **You should see**: Green notification "File saved successfully!"

**Expected in Terminal 1:**
```
Read file: /readme.txt (48 bytes) by admin
File edited: /readme.txt (new size: 103 bytes) by admin
```

6. **Verify the edit**: Click "Read" again and confirm your changes are saved

**✅ Test passed if changes are persisted!**

---

### Step 6: Test CREATE Operation

1. **Click "📄 Create File"** in the sidebar
2. **A modal appears** with form fields
3. **Fill in the form:**
   - File Name: `mytest.txt`
   - File Content:
   ```
   This is my test file!
   Created from the web UI.
   Testing create functionality.
   ```
4. **Click "Create File"**
5. **You should see**:
   - Green notification "File created successfully!"
   - New file appears in the list: `mytest.txt`

**Expected in Terminal 1:**
```
File created: /mytest.txt (inode=1003, blocks=1) by admin
```

6. **Verify creation**: Click "Read" on the new file to confirm content

**✅ Test passed if the new file appears and contains your text!**

---

### Step 7: Test DELETE Operation

1. **Click "Delete"** on the file you just created (`mytest.txt`)
2. **A confirmation dialog appears**: "Are you sure you want to delete /mytest.txt?"
3. **Click "OK"** to confirm
4. **You should see**:
   - Green notification "File deleted successfully!"
   - The file disappears from the list

**Expected in Terminal 1:**
```
File deleted: /mytest.txt
```

**✅ Test passed if the file is removed from the list!**

---

### Step 8: Test CREATE with Different File Types

Create various files to test:

**1. JSON File:**
- Name: `config.json`
- Content:
```json
{
  "app": "OFS",
  "version": "1.0",
  "enabled": true
}
```

**2. Code File:**
- Name: `hello.cpp`
- Content:
```cpp
#include <iostream>
int main() {
    std::cout << "Hello from OFS!" << std::endl;
    return 0;
}
```

**3. Markdown File:**
- Name: `README.md`
- Content:
```markdown
# My Project
This is a **test** markdown file.
- Item 1
- Item 2
```

**✅ Test passed if all file types can be created and read back!**

---

### Step 9: Test Refresh Functionality

1. **Click the "🔄 Refresh Files"** button in sidebar
   - OR click "Refresh" in the toolbar
2. **File list should reload**
3. **All files should still be visible**

**✅ Test passed if the list refreshes without errors!**

---

### Step 10: Test Large File Content

1. **Create a file** named `large.txt`
2. **Paste a large amount of text** (copy this multiple times):
```
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
```
3. **Create the file**
4. **Read it back** to verify

**✅ Test passed if large content is saved and displayed correctly!**

---

## 🔒 Phase 5: Testing Permission System

### Current State
Currently, only the `admin` user exists. The permission system is fully implemented but can only be fully tested when multiple users exist.

### What IS Working Now:

1. **Owner Tracking**: Each file shows who created it
2. **Admin Access**: Admin can access all files
3. **Permission Checks**: Code verifies ownership before allowing operations

### Test Permission Code:

You can verify permission checks work by examining Terminal 1 output:
```
File created: /test.txt (inode=1004, blocks=1) by admin
Read file: /test.txt (10 bytes) by admin
File edited: /test.txt (new size: 20 bytes) by admin
```

Notice each operation shows "by admin" - this is the ownership tracking!

### When You Add More Users:

The system will automatically enforce:
- ❌ User A cannot read User B's files
- ❌ User A cannot edit User B's files
- ❌ User A cannot delete User B's files
- ✅ Admin can access all files

---

## 🎯 Testing Checklist

Use this checklist to verify everything works:

### Basic Operations
- [ ] Login with admin/admin123
- [ ] View file list (should show 3 default files)
- [ ] Read a file (content displays in modal)
- [ ] Edit a file (changes are saved)
- [ ] Create a new file (appears in list)
- [ ] Delete a file (removed from list)
- [ ] Refresh file list (list updates)

### Advanced Operations
- [ ] Create JSON file (valid JSON content)
- [ ] Create code file (C++ or other)
- [ ] Create markdown file (formatted text)
- [ ] Edit multiple files in sequence
- [ ] Create file with empty content
- [ ] Create file with large content (>1KB)
- [ ] Edit file to be larger than original
- [ ] Edit file to be smaller than original

### UI/UX
- [ ] All modals open and close properly
- [ ] Notifications appear for all operations
- [ ] Success notifications are green
- [ ] Error notifications (if any) are red
- [ ] Close modal with X button works
- [ ] Close modal by clicking outside works
- [ ] File list updates after each operation
- [ ] No console errors in browser (F12)

### Server Communication
- [ ] Terminal 1 shows all operations
- [ ] No error messages in server terminal
- [ ] Server responds to all requests
- [ ] JSON responses are valid

---

## 🐛 Troubleshooting Common Issues

### Issue 1: "Cannot connect to server"

**Symptoms:** UI shows connection errors, files don't load

**Solution:**
```bash
# Check if ofsserver is running
ps aux | grep ofsserver

# Check if port 8080 is in use
lsof -i :8080

# If needed, kill the process and restart
kill -9 <PID>
cd /Users/hamnahassan/Documents/file-verse/source
./ofsserver ofs.omni
```

---

### Issue 2: "UI doesn't load" or "404 Not Found"

**Symptoms:** Browser shows "can't connect" or 404 error

**Solution:**
```bash
# Check if web server is running
ps aux | grep python

# Check if port 8000 is in use
lsof -i :8000

# Restart web server
cd /Users/hamnahassan/Documents/file-verse/ui
python3 server.py
```

---

### Issue 3: "Login doesn't work"

**Symptoms:** Login fails, shows "Invalid credentials"

**Solution:**
- Make sure you initialized the filesystem first (`INIT ofs.omni`)
- Default credentials are: `admin` / `admin123`
- Check Terminal 1 for error messages

---

### Issue 4: "Files don't appear after creation"

**Symptoms:** Created file doesn't show in list

**Solution:**
- Click "Refresh" button
- Check Terminal 1 for success message
- Verify no error notifications appeared
- Check browser console (F12) for errors

---

### Issue 5: "Permission denied" errors

**Symptoms:** Operations fail with permission errors

**Solution:**
- Currently, only admin user exists
- All files should be owned by admin
- Check Terminal 1 logs for actual error

---

## 📊 Expected Server Logs

During a typical testing session, Terminal 1 should show:

```
FS initialized successfully. Version: 65536
   Total blocks: 2560 x 4096 bytes
   Users loaded: 1
Server listening on port 8080
Ready to accept connections...

[Client connected]
Listed directory: / (3 entries)

[Client connected]
Read file: /readme.txt (48 bytes) by admin

[Client connected]
Read file: /readme.txt (48 bytes) by admin
File edited: /readme.txt (new size: 103 bytes) by admin

[Client connected]
File created: /mytest.txt (inode=1003, blocks=1) by admin

[Client connected]
File deleted: /mytest.txt
```

---

## 🎓 Testing Tips

1. **Keep Both Terminals Visible**: Monitor server logs while using UI
2. **Use Browser Developer Tools**: Press F12 to see any JavaScript errors
3. **Test Systematically**: Go through the checklist in order
4. **Try Edge Cases**: Empty files, very long filenames, special characters
5. **Refresh Often**: Use the refresh button to verify changes persist
6. **Read Server Logs**: They show exactly what's happening on the backend

---

## 🎉 Success Criteria

Your implementation is working correctly if:

✅ **All basic operations work** (create, read, edit, delete)
✅ **Files persist** (survive refresh, readable after edit)
✅ **UI is responsive** (modals open/close, notifications appear)
✅ **Server logs show operations** (each action logged in Terminal 1)
✅ **No errors in browser console** (F12 developer tools)
✅ **Permission tracking works** (files show owner "admin")

---

## 📹 Testing Video Script

If you want to record a demo, follow this script:

1. **Show both terminals** side by side
2. **Start with login** - enter credentials
3. **Show file list** - point out existing files
4. **Read a file** - open modal, show content
5. **Edit that file** - modify text, save, read again to verify
6. **Create new file** - show the form, add content, submit
7. **Verify creation** - refresh list, read the new file
8. **Delete the file** - confirm deletion, show it disappears
9. **Show server logs** - point to Terminal 1, show operations logged

---

## 🚦 Quick Test (2 Minutes)

Short version for quick verification:

```bash
# Terminal 1
cd /Users/hamnahassan/Documents/file-verse/source
./ofstest
> FORMAT 10485760 4096 ofs.omni
> INIT ofs.omni
> CREATE /test.txt "Hello" admin
> SHUTDOWN
./ofsserver ofs.omni

# Terminal 2
cd /Users/hamnahassan/Documents/file-verse/ui
python3 server.py

# Browser
# 1. Go to http://localhost:8000/filemanager.html
# 2. Login: admin/admin123
# 3. Click Read on test.txt
# 4. Click Edit, change text, save
# 5. Click Read again to verify
```

✅ **If this works, your implementation is solid!**

---

## 📝 What to Document/Screenshot

For your submission or demo, capture:

1. ✅ Server startup (Terminal 1)
2. ✅ File list view (showing multiple files)
3. ✅ Read modal (showing file content)
4. ✅ Edit modal (with text being edited)
5. ✅ Create modal (creating a new file)
6. ✅ Success notifications
7. ✅ Server logs showing operations
8. ✅ Files persisting after refresh

---

## 🎯 Next Steps After Testing

Once basic testing passes:

1. **Performance Testing**: Create many files, test with large content
2. **Stress Testing**: Multiple rapid operations
3. **Error Testing**: Try to break it (invalid paths, empty names, etc.)
4. **User Testing**: Add more users, test permission system fully
5. **Feature Enhancement**: Add file search, sorting, filters

---

**Happy Testing! 🚀**

If you encounter any issues not covered here, check the server logs in Terminal 1 - they usually reveal what's wrong!
