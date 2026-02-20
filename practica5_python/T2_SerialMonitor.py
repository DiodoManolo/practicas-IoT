import time
import sys

import serial
from serial.tools import list_ports

BAUD = 115200


def choose_port() -> str:
    ports = list(list_ports.comports())
    if not ports:
        raise RuntimeError("No se detectan puertos serie. ¿Está conectada la placa?")

    print("Puertos serie detectados:")
    for i, p in enumerate(ports, 1):
        print(f"  {i}) {p.device} - {p.description}")

    while True:
        choice = input("Elige el número del puerto (ej: 1): ").strip()
        if choice.isdigit() and 1 <= int(choice) <= len(ports):
            return ports[int(choice) - 1].device
        print("Opción no válida.")


def main():
    port = choose_port()

    print(f"\nAbriendo {port} a {BAUD} baudios...")
    print("Cierra el Serial Monitor de Arduino IDE si lo tienes abierto.")

    while True:
        try:
            with serial.Serial(port, BAUD, timeout=1) as ser:
                # A veces al abrir el puerto la placa se resetea. Espera un poco.
                time.sleep(1.0)
                ser.reset_input_buffer()

                while True:
                    raw = ser.readline()  # lee hasta '\n' o hasta timeout
                    if not raw:
                        continue

                    line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
                    print(line, flush=True)

        except serial.SerialException as e:
            print(f"[SerialException] {e}")
            print("Reintentando en 2 segundos... (¿COM cambiado? ¿puerto ocupado?)\n")
            time.sleep(2)

        except KeyboardInterrupt:
            print("\nSaliendo...")
            sys.exit(0)


if __name__ == "__main__":
    main()