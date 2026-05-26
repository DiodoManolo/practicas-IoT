import time
from pathlib import Path

import serial

# === CONFIGURA ESTO ===
PORT = "COM13"        # cambia si tu puerto es otro
BAUD = 115200
OUT_FILE = Path("imu_log.csv")  # se crea en la carpeta del proyecto
WRITE_HEADER = True             # pone una cabecera opcional

def main():
    print(f"Abriendo {PORT} a {BAUD}...")
    print(f"Guardando en: {OUT_FILE.resolve()}")
    print("Ctrl+C para parar.\n")

    with serial.Serial(PORT, BAUD, timeout=1) as ser:
        # Al abrir el puerto, a veces la placa resetea. Esperamos un poco.
        time.sleep(1.0)
        ser.reset_input_buffer()

        with OUT_FILE.open("w", encoding="utf-8", newline="") as f:
            if WRITE_HEADER:
                # Cabecera tipo CSV separado por ; (Excel lo entiende bien)
                f.write("t_ms;ax;ay;az;gx;gy;gz;mx;my;mz\n")

            while True:
                raw = ser.readline()
                if not raw:
                    continue

                line = raw.decode("utf-8", errors="replace").strip()

                # Saltamos líneas que no son muestras (por ejemplo: "#BLOCK ...")
                if not line:
                    continue
                if line.startswith("#"):
                    continue


                # Aseguramos salto de línea final:
                f.write(line + "\n")

                # También lo mostramos en pantalla (opcional)
                print(line)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nParado por el usuario. Archivo guardado.")