import socket

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', 2312))

    print("Listening....")
    s.listen(2)
    conn, addr = s.accept()
    print(f"Connected to {addr}")

    data = conn.recv(1024)
    print(f"RECV: {data}")

    conn.close()
    s.close()

if __name__ == "__main__":
    main()