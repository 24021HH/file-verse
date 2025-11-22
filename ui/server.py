#!/usr/bin/env python3
import http.server
import socketserver
import sys

PORT = 8000
if len(sys.argv) > 1:
    PORT = int(sys.argv[1])

class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()
    
    def do_OPTIONS(self):
        self.send_response(200)
        self.end_headers()

Handler = MyHTTPRequestHandler

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print(f"")
    print(f"🌐 OFS Web Server Started")
    print(f"=" * 50)
    print(f"Server running on: http://localhost:{PORT}")
    print(f"")
    print(f"📂 Open this URL in your browser:")
    print(f"   http://localhost:{PORT}/index.html")
    print(f"")
    print(f"Press Ctrl+C to stop the server")
    print(f"=" * 50)
    print(f"")
    httpd.serve_forever()
