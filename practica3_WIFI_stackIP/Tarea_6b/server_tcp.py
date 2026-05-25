import socket

HOST = "0.0.0.0"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(1)

print(f"Servidor TCP escuchando en el puerto {PORT}...")
print("Esperando conexion del ESP32...")

conn, addr = server.accept()
print(f"Conexion recibida desde {addr}")

buffer = ""

while True:
    data = conn.recv(1024)

    if not data:
        print("Conexion cerrada")
        break

    buffer += data.decode()

    while "\n" in buffer:
        linea, buffer = buffer.split("\n", 1)
        linea = linea.strip()

        if linea:
            print("Datos recibidos:", linea)

conn.close()
server.close()