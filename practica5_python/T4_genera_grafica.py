import time
import serial

import matplotlib
matplotlib.use("TkAgg")  # fuerza ventana real (muy importante en PyCharm)

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# ===== CONFIG =====
PORT = "COM13"
BAUD = 115200
WINDOW_SECONDS = 5
COL = 3  # 1=ax 2=ay 3=az ... (0 es tiempo)
#ax;ay;az;gx;gy;gz;mx;my;mz
# ==================

def parse_line(line: str):
    line = line.strip()
    if not line or line.startswith("#"):
        return None
    parts = line.split(";")
    if len(parts) < 2:
        return None
    try:
        t_ms = int(parts[0])
        values = [float(x) for x in parts[1:]]  # ax..mz
        return t_ms, values
    except ValueError:
        return None

def mean_std(data):
    n = len(data)
    if n == 0:
        return None, None
    m = sum(data) / n
    var = sum((x - m) ** 2 for x in data) / n
    return m, var ** 0.5

def main():
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
    time.sleep(1)
    ser.reset_input_buffer()

    window_samples = []
    window_start = time.time()

    times, means, stds = [], [], []
    start_time = time.time()

    fig, ax = plt.subplots()
    line_mean, = ax.plot([], [], marker="o", label="media")
    line_plus, = ax.plot([], [], linestyle="--", label="media + std")
    line_minus, = ax.plot([], [], linestyle="--", label="media - std")
    ax.set_title("Media y desviación estándar cada 5 s")
    ax.set_xlabel("Tiempo (s)")
    ax.set_ylabel(f"Columna {COL} (az si COL=3)")
    ax.grid(True)
    ax.legend()

    def update(_frame):
        nonlocal window_start, window_samples

        # leer 1 línea por update (simple y robusto)
        raw = ser.readline()
        if raw:
            line = raw.decode("utf-8", errors="replace")
            parsed = parse_line(line)
            if parsed is not None:
                _, values = parsed
                idx = COL - 1
                if 0 <= idx < len(values):
                    window_samples.append(values[idx])

        now = time.time()
        if (now - window_start) >= WINDOW_SECONDS:
            m, s = mean_std(window_samples)
            print(f"5s -> muestras={len(window_samples)} media={m} std={s}")
            if m is not None:
                t_rel = now - start_time
                times.append(t_rel)
                means.append(m)
                stds.append(s)
            window_samples = []
            window_start = now

        if len(times) > 0:
            line_mean.set_data(times, means)
            line_plus.set_data(times, [m + s for m, s in zip(means, stds)])
            line_minus.set_data(times, [m - s for m, s in zip(means, stds)])

            ax.set_xlim(0, max(times) + 1)

            y_all = []
            for m, s in zip(means, stds):
                y_all += [m - s, m + s]
            ymin, ymax = min(y_all), max(y_all)
            pad = (ymax - ymin) * 0.1 if ymax > ymin else 0.1
            ax.set_ylim(ymin - pad, ymax + pad)

        return line_mean, line_plus, line_minus

    def on_close(_event):
        ser.close()

    fig.canvas.mpl_connect("close_event", on_close)

    ani = FuncAnimation(fig, update, interval=100, cache_frame_data=False)
    plt.show()  # ahora sí se queda “vivo” hasta que cierres la ventana

if __name__ == "__main__":
    main()