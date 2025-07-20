# serverSetUp.py
import socket
import time

HOST = '127.0.0.1'
PORT = 9001

def run_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        try:
            s.bind((HOST, PORT))
            print(f"Server bound to {HOST}:{PORT}")
        except OSError as e:
            print(f"Bind failed: {e}")
            return

        s.listen()
        print(f"Server listening on {HOST}:{PORT} (Ctrl+C to exit)")
        
        try:
            while True:
                conn, addr = s.accept()
                with conn:
                    print(f"New connection from {addr}")
                    while True:
                        data = conn.recv(1024)
                        if not data:
                            print("Connection closed by client")
                            break
                        print(data.decode())
        except KeyboardInterrupt:
            print("\nServer shutdown requested")
        except Exception as e:
            print(f"Unexpected error: {e}")

if __name__ == "__main__":
    print("Starting socket server...")
    run_server()