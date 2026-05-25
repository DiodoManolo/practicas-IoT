import socket

HOST = "0.0.0.0"
PORT = 5000 #Puerto del servido TCP

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #crea un socket de red que use direcciones IPv4 y funcione con TCP
server.bind((HOST, PORT)) #este servidor va a escuchar en 0.0.0.0:5000
server.listen(1) #Hace que el socket se quede esperando conexiones entrantes, 1 de una conexión.

print(f"Servidor TCP escuchando en el puerto {PORT}...")
print("Esperando conexion del ESP32...")

conn, addr = server.accept()
print(f"Conexion recibida desde {addr}")

buffer = "" #para ir acumulando texto recibido

while True:
    data = conn.recv(1024) #Lee hasta 1024 bytes desde la conexión TCP

    if not data:
        print("Conexion cerrada")
        break

    buffer += data.decode() #convierte bytes en texto y lo guarda en el buffer

    while "\n" in buffer:
        linea, buffer = buffer.split("\n", 1)
        linea = linea.strip()

        if linea:
            print("Datos recibidos:", linea)

conn.close()
server.close()