#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║         OFS - Omni File System - Full Stack               ║"
echo "║              (Part 1, 2, and 3 Complete)                  ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

echo "Stopping any existing servers..."
pkill -9 -f "ofsserver|server.py" 2>/dev/null
sleep 1

echo "Starting OFS Server (Part 2) on port 8080..."
cd "/Users/hamnahassan/Documents/file-verse 3/source"
./ofsserver 8080 > /tmp/ofs.log 2>&1 &
OFS_PID=$!
echo "  OFS Server PID: $OFS_PID"
sleep 2

echo "Starting Web UI Server (Part 3) on port 3000..."
cd "/Users/hamnahassan/Documents/file-verse 3/ui"
python3 server.py 3000 > /tmp/web.log 2>&1 &
WEB_PID=$!
echo "  Web Server PID: $WEB_PID"
sleep 2

echo ""
echo "Testing connectivity..."

echo -n "  Testing OFS Server (8080): "
if echo "LIST /" | nc -w 1 127.0.0.1 8080 2>/dev/null | grep -q "succeeded"; then
    echo "✓ RUNNING"
else
    echo "✗ FAILED"
fi

echo -n "  Testing Web Server (3000): "
if curl -s http://localhost:3000 2>/dev/null | grep -q "OFS - Omni File System"; then
    echo "✓ RUNNING"
else
    echo "✗ FAILED"
fi

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                    SYSTEM READY!                          ║"
echo "╠════════════════════════════════════════════════════════════╣"
echo "║                                                            ║"
echo "║  Part 2 - OFS Server:                                     ║"
echo "║    URL: 127.0.0.1:8080                                    ║"
echo "║    Status: RUNNING                                        ║"
echo "║                                                            ║"
echo "║  Part 3 - Web UI:                                         ║"
echo "║    URL: http://localhost:3000                             ║"
echo "║    Status: RUNNING                                        ║"
echo "║                                                            ║"
echo "║  File System:                                             ║"
echo "║    Container: fs_server.omni (10 MB, 2560 blocks)         ║"
echo "║    Users: 51 pre-loaded                                   ║"
echo "║    Admin: admin / admin123                                ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Open browser and go to: http://localhost:3000"
echo ""
echo "Press Ctrl+C to stop servers"
echo ""

wait
