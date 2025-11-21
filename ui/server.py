import http.server
import socketserver
import json
import socket
import os
import sys
from pathlib import Path
from urllib.parse import urlparse, parse_qs

class OFSHandler(http.server.SimpleHTTPRequestHandler):
    
    ofs_host = '127.0.0.1'
    ofs_port = 8080
    ui_dir = Path(__file__).parent
    
    def do_GET(self):
        path = self.path
        if path == '/' or path == '':
            path = '/login.html'
        
        if path.endswith('.html'):
            content_type = 'text/html'
        elif path.endswith('.css'):
            content_type = 'text/css'
        elif path.endswith('.js'):
            content_type = 'application/javascript'
        else:
            self.send_response(404)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b'File not found')
            return
        
        file_path = self.ui_dir / path.lstrip('/')
        
        try:
            if not file_path.exists():
                self.send_response(404)
                self.send_header('Content-type', 'text/plain')
                self.end_headers()
                self.wfile.write(f'File not found: {file_path}'.encode())
                return
            
            with open(file_path, 'rb') as f:
                content = f.read()
            
            self.send_response(200)
            self.send_header('Content-type', content_type)
            self.send_header('Content-Length', str(len(content)))
            self.end_headers()
            self.wfile.write(content)
        except Exception as e:
            self.send_response(500)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(f'Error serving file: {str(e)}'.encode())
    
    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length).decode('utf-8', errors='replace')
        
        try:
            if self.path == '/api/proxy':
                request_data = json.loads(body)
                host = request_data.get('host', self.ofs_host)
                port = request_data.get('port', self.ofs_port)
                message = request_data.get('message', '')
                
                response = self.proxy_to_ofs_with_addr(message, host, port)
            else:
                response = self.proxy_to_ofs_with_addr(body, self.ofs_host, self.ofs_port)
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            
            try:
                if isinstance(response, str) and response.strip().startswith('{'):
                    self.wfile.write(response.encode())
                else:
                    json_response = json.dumps({
                        "status": "success",
                        "response": response
                    })
                    self.wfile.write(json_response.encode())
            except:
                self.wfile.write(response.encode() if isinstance(response, str) else response)
            
        except Exception as e:
            error_response = json.dumps({
                "status": "error",
                "error": str(e)
            })
            self.send_response(500)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(error_response.encode())
    
    def do_OPTIONS(self):
        """Handle CORS preflight requests"""
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'POST, GET, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
    
    def proxy_to_ofs(self, request_data):
        return self.proxy_to_ofs_with_addr(request_data, self.ofs_host, self.ofs_port)
    
    def proxy_to_ofs_with_addr(self, request_data, host, port):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((host, port))
            
            sock.sendall(request_data.encode() if isinstance(request_data, str) else request_data)
            
            response = b''
            while True:
                try:
                    chunk = sock.recv(4096)
                    if not chunk:
                        break
                    response += chunk
                except socket.timeout:
                    break
            
            sock.close()
            
            response_text = response.decode('utf-8', errors='replace')
            
            try:
                json.loads(response_text)
                return response_text
            except:
                return json.dumps({
                    "status": "success",
                    "response": response_text
                })
        
        except socket.timeout:
            return json.dumps({
                "status": "error",
                "error": "Server timeout"
            })
        except ConnectionRefusedError:
            return json.dumps({
                "status": "error",
                "error": "Cannot connect to OFS server on {}:{}".format(host, port)
            })
        except Exception as e:
            return json.dumps({
                "status": "error",
                "error": str(e)
            })
    
    def log_message(self, format, *args):
        print(f"[{self.client_address[0]}] {format % args}")


def main():
    port = 3000
    if len(sys.argv) > 1:
        try:
            port = int(sys.argv[1])
        except:
            pass
    
    if len(sys.argv) > 2:
        OFSHandler.ofs_host = sys.argv[2]
    if len(sys.argv) > 3:
        try:
            OFSHandler.ofs_port = int(sys.argv[3])
        except:
            pass
    
    Handler = OFSHandler
    
    with socketserver.TCPServer(("", port), Handler) as httpd:
        print(f"╔════════════════════════════════════════════════╗")
        print(f"║  OFS Web UI Server Started                    ║")
        print(f"╠════════════════════════════════════════════════╣")
        print(f"║  Web UI: http://localhost:{port:<33} ║")
        print(f"║  OFS Server: {OFSHandler.ofs_host}:{OFSHandler.ofs_port:<38} ║")
        print(f"║  Serving from: {str(OFSHandler.ui_dir):<32} ║")
        print(f"╠════════════════════════════════════════════════╣")
        print(f"║  Press Ctrl+C to stop the server               ║")
        print(f"╚════════════════════════════════════════════════╝")
        print()
        
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")


if __name__ == "__main__":
    main()
