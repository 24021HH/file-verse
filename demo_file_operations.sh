#!/bin/bash

echo "=================================="
echo "OFS File Operations Demo"
echo "=================================="
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

cd "$(dirname "$0")/source"

# Step 1: Format the disk
echo -e "${BLUE}Step 1: Formatting filesystem...${NC}"
./ofstest << EOF
FORMAT 10485760 4096 ofs.omni
EOF
echo ""

# Step 2: Initialize the filesystem
echo -e "${BLUE}Step 2: Initializing filesystem...${NC}"
./ofstest << EOF
INIT ofs.omni
EOF
echo ""

# Step 3: Create test users
echo -e "${BLUE}Step 3: Setting up test scenario...${NC}"
echo "Users available: admin (default)"
echo ""

# Step 4: Create files as different users
echo -e "${BLUE}Step 4: Creating files...${NC}"
./ofstest << EOF
CREATE /admin_file.txt "This is admin's file" admin
CREATE /user1_file.txt "This is user1's file" admin
EOF
echo ""

# Step 5: Try to read files
echo -e "${BLUE}Step 5: Reading files...${NC}"
./ofstest << EOF
READ /admin_file.txt admin
READ /user1_file.txt admin
EOF
echo ""

# Step 6: List files
echo -e "${BLUE}Step 6: Listing files...${NC}"
./ofstest << EOF
LIST /
EOF
echo ""

# Step 7: Start the server
echo -e "${GREEN}=================================="
echo "Starting OFS Server..."
echo "==================================${NC}"
echo ""
echo "Server will be available at: http://localhost:8080"
echo "Web UI available at: http://localhost:8000/filemanager.html"
echo ""
echo "To access the file manager:"
echo "1. Open another terminal"
echo "2. Run: cd ui && python3 server.py"
echo "3. Open browser to: http://localhost:8000/filemanager.html"
echo "4. Login with: admin / admin123"
echo ""
echo -e "${YELLOW}Press Ctrl+C to stop the server${NC}"
echo ""

./ofsserver ofs.omni
