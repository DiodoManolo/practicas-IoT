import socket
import threading
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

HOST = "0.0.0.0"
PORT = 5000

# Listas para guardar datos
muestras = []
ax_vals = []
ay_vals = []
az_vals = []

contador = 0

def recibir_datos():
    global contador

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #crea un socket de red que use direcciones IPv4 y funcione con TCP
    server.bind((HOST, PORT))#este servidor va a escuchar en 0.0.0.0:5000
    server.listen(1)#Hace que el socket se quede esperando conexiones entrantes, 1 de una conexión.

    print(f"Servidor TCP escuchando en el puerto {PORT}...")
    print("Esperando conexion del ESP32...")

    conn, addr = server.accept()
    print(f"Conexion recibida desde {addr}")

    buffer = ""#para ir acumulando texto recibido

    while True:
        data = conn.recv(1024)#Lee hasta 1024 bytes desde la conexión TCP

        if not data:
            print("Conexion cerrada")
            break

        buffer += data.decode()#convierte bytes en texto y lo guarda en el buffer

        while "\n" in buffer:
            linea, buffer = buffer.split("\n", 1)
            linea = linea.strip()

            if linea:
                try:
                    ax, ay, az = linea.split(";")
                    ax = float(ax)
                    ay = float(ay)
                    az = float(az)

                    muestras.append(contador)
                    ax_vals.append(ax)
                    ay_vals.append(ay)
                    az_vals.append(az)

                    contador += 1

                    # Mantener solo las últimas 100 muestras
                    if len(muestras) > 100:
                        muestras.pop(0)
                        ax_vals.pop(0)
                        ay_vals.pop(0)
                        az_vals.pop(0)

                    print(f"Datos recibidos: ax={ax}, ay={ay}, az={az}")

                except:
                    print("Linea no valida:", linea)

    conn.close()
    server.close()

def actualizar_grafica(frame):
    plt.cla()
    plt.plot(muestras, ax_vals, label="ax")
    plt.plot(muestras, ay_vals, label="ay")
    plt.plot(muestras, az_vals, label="az")
    plt.xlabel("Muestra")
    plt.ylabel("Aceleracion")
    plt.title("Datos de aceleracion recibidos por TCP")
    plt.legend()
    plt.grid(True)

# Hilo para recibir datos sin bloquear la gráfica
hilo_tcp = threading.Thread(target=recibir_datos, daemon=True)
hilo_tcp.start()

ani = FuncAnimation(plt.gcf(), actualizar_grafica, interval=200)
plt.show()