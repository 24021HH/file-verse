# OFS Web UI - Complete Testing Guide

## System Status

✅ **All Systems Operational**

### Running Services:
- **Part 1 (Core):** File system with 51 pre-loaded users
- **Part 2 (Server):** TCP server on port 8080
- **Part 3 (UI):** Web interface on port 3000

---

## Quick Start - How to Run Everything

### Step 1: Start the OFS Server (Part 2)
```bash
cd "/Users/hamnahassan/Documents/file-verse 3/source"
./ofsserver 8080
```
Expected output: Server listens on port 8080

### Step 2: Start the Web Server (Part 3)
In a new terminal:
```bash
cd "/Users/hamnahassan/Documents/file-verse 3/ui"
python3 server.py 3000
```
Expected output: Server runs on port 3000

### Step 3: Access the UI
Open browser and navigate to:
```
http://localhost:3000
```

---

## UI Features - What You'll See

### 1. Header
- Title: "OFS - Omni File System"
- Clean, professional dark theme

### 2. System Information Section
Displays:
- **Omni File:** fs_server.omni (10 MB, 2560 blocks)
- **Server:** 127.0.0.1:8080 [RUNNING]
- **Users:** 51 Pre-loaded
- **Admin:** admin / admin123

### 3. Quick Command Buttons
Five action buttons:
- **LIST /** - List root directory
- **CREATE** - Create /test file with data "admin"
- **READ** - Read /test file
- **DELETE** - Delete /test file
- **RUN DEMO** - Auto-runs demo sequence (5 commands)

### 4. Activity Log
Real-time display of all commands and responses

---

## Testing Instructions

### Test 1: Basic Server Connectivity
```bash
# Test OFS server is responding
echo "LIST /" | nc -w 1 127.0.0.1 8080

# Expected: LIST succeeded
```

### Test 2: Web Server File Serving
```bash
# Test web server serves HTML
curl -s http://localhost:3000 | head -20

# Expected: HTML page with "OFS - Omni File System" header
```

### Test 3: Direct HTML Access
```bash
# Test direct file access
curl -s http://localhost:3000/simple.html | grep "title"

# Expected: <title>OFS Demo</title>
```

### Test 4: API Proxy Test
```bash
# Test proxy endpoint
curl -X POST http://localhost:3000/api/proxy \
  -H "Content-Type: application/json" \
  -d '{"message":"LIST /","host":"127.0.0.1","port":8080}'

# Expected: JSON response with LIST result
```

---

## Manual Testing in Browser

### Test 1: View System Info
1. Open http://localhost:3000
2. See "System Information" card
3. Verify displays:
   - fs_server.omni (10 MB, 2560 blocks)
   - Server running
   - 51 users
   - admin credentials

### Test 2: Click LIST Button
1. Click "LIST /" button
2. Watch Activity Log
3. Should show: "LIST succeeded"

### Test 3: Click RUN DEMO
1. Click "RUN DEMO" button
2. Watch log auto-execute 5 commands:
   - Step 1: LIST /
   - Step 2: CREATE /demo file
   - Step 3: READ /demo file
   - Step 4: DELETE /demo file
   - Step 5: LIST / again
3. Should see all commands complete successfully

### Test 4: Manual Command
1. Type in input field (if available)
2. Click send/enter
3. See result in log

---

## File Locations

```
/Users/hamnahassan/Documents/file-verse 3/
├── source/
│   ├── ofsserver          (Part 2 - Running on port 8080)
│   └── ofsclient          (C++ client utility)
├── ui/
│   ├── simple.html        (UI - Served by web server)
│   ├── server.py          (Part 3 - Python web server on port 3000)
│   └── tests/             (Test utilities)
└── compiled/
    ├── fs_server.omni     (10MB file system container)
    └── default.uconf      (Configuration)
```

---

## Troubleshooting

### "File not found" on UI
**Solution:** Restart web server
```bash
pkill -9 python3
cd "/Users/hamnahassan/Documents/file-verse 3/ui"
python3 server.py 3000
```

### Server not responding
**Solution:** Restart OFS server
```bash
pkill -9 ofsserver
cd "/Users/hamnahassan/Documents/file-verse 3/source"
./ofsserver 8080
```

### Port already in use
**Solution:** Kill process on port
```bash
# For port 8080
lsof -ti :8080 | xargs kill -9

# For port 3000
lsof -ti :3000 | xargs kill -9
```

---

## Expected Test Results

✅ **All Passing Tests:**
1. OFS Server responds to commands on port 8080
2. Web Server serves HTML on port 3000
3. UI displays system information correctly
4. All buttons execute commands successfully
5. Activity log shows real-time responses
6. Demo runs all 5 steps without errors

---

## Tech Stack

**Part 2 (Server):**
- Language: C++17
- Protocol: TCP/JSON
- File System: 10MB omni file with 2560 blocks
- Users: 51 pre-loaded

**Part 3 (UI):**
- Frontend: HTML5 + CSS3 + JavaScript (Vanilla)
- Backend: Python 3 HTTP Server
- Communication: HTTP POST to proxy, TCP to OFS
- Theme: Dark mode (professional design)

---

## Quick Commands Reference

```bash
# Kill all servers
pkill -9 -f "ofsserver|server.py"

# Start everything fresh
cd /Users/hamnahassan/Documents/file-verse\ 3/source && ./ofsserver 8080 &
sleep 2
cd /Users/hamnahassan/Documents/file-verse\ 3/ui && python3 server.py 3000 &
sleep 2

# Test servers
echo "LIST /" | nc -w 1 127.0.0.1 8080
curl -s http://localhost:3000 | head -5

# Check server status
ps aux | grep -E "ofsserver|server.py" | grep -v grep
lsof -i :8080
lsof -i :3000
```

---

## Success Criteria

✅ You're done when:
1. Both servers running without errors
2. UI accessible at http://localhost:3000
3. System info displays correctly
4. All buttons work and show responses
5. Demo sequence completes successfully
6. Activity log shows all operations

**Congratulations! Your OFS system is complete!** 🎉

