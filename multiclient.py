import socket
import threading


NUM_CLIENTS = 10

start_event = threading.Event()


def client(client_id):
    # 全スレッドがここで待機
    start_event.wait()

    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_STREAM
    )

    sock.connect(("127.0.0.1", 8080))

    request = (
        "GET /hello HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    )

    sock.sendall(request.encode())

    response = sock.recv(4096)

    print(
        f"Client {client_id}: "
        f"{response.decode()}"
    )

    sock.close()


threads = []

for i in range(NUM_CLIENTS):
    thread = threading.Thread(
        target=client,
        args=(i,)
    )

    thread.start()
    threads.append(thread)


print("All clients are ready. Start!")

# 全スレッドに開始指示
start_event.set()


for thread in threads:
    thread.join()
