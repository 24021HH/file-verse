import socket
import json
import sys

def send_json(req):
    with socket.create_connection(('127.0.0.1', 8080), timeout=4) as s:
        s.sendall(json.dumps(req).encode())
        data = b''
        while True:
            part = s.recv(4096)
            if not part: break
            data += part
        return json.loads(data.decode())

def main():
    try:
        delete_req = {"operation":"DELETE", "parameters":{"path":"/testfile","user":"admin"}, "request_id":"cleanup"}
        delete_req['session_id'] = ''
        send_json(delete_req)
    except Exception:
        pass

    tests = [
        {"operation":"CREATE", "parameters":{"path":"/testfile","data":"abc","owner":"admin"}, "request_id":"t1"},
        {"operation":"LIST", "parameters":{"path":"/"}, "request_id":"t2"},
        {"operation":"READ", "parameters":{"path":"/testfile"}, "request_id":"t3"},
        {"operation":"DELETE", "parameters":{"path":"/testfile","user":"admin"}, "request_id":"t4"}
    ]
    for req in tests:
        req['session_id'] = ''
        resp = send_json(req)
        print('REQ', req['operation'], '->', resp)
        if resp.get('status') != 'success':
            print('Test failed for op', req['operation'])
            sys.exit(1)
    print('All JSON tests passed')

if __name__ == '__main__':
    main()
