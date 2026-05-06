## Tarea 7 - Muestreo de IMU y envío por I2C entre dos placas

### Objetivo
Combinar la lectura de sensores de la IMU con la comunicación I2C entre dos placas Arduino Nano 33 BLE Sense.

- **PLACA 1**:
  - captura datos del acelerómetro, giroscopio y magnetómetro
  - muestrea cada 200 ms durante 1 segundo
  - al terminar, envía los datos por I2C a la otra placa

- **PLACA 2**:
  - recibe los datos por I2C
  - los muestra por pantalla
  - enciende un LED durante 1 segundo al recibirlos

---

## Relación con las tareas anteriores
Esta práctica combina:

- **Tarea 5**: comunicación entre dos placas mediante I2C
- **Tarea 6**: lectura de acelerómetro, giroscopio y magnetómetro

---

## Material necesario
- 2 placas Arduino Nano 33 BLE Sense
- 1 LED
- 1 resistencia de 220 Ω
- cables de conexión
- 2 cables USB

---

## Librerías utilizadas
- `Wire.h`
- `Arduino_LSM9DS1.h`

---

## Conexiones

### Entre las dos placas
- SDA ↔ SDA
- SCL ↔ SCL
- GND ↔ GND

### LED externo en la placa 2
- `D6` → resistencia `220 Ω` → ánodo del LED
- cátodo del LED → `GND`

---

## Funcionamiento

### PLACA 1
La placa 1 actúa como **maestra I2C**.  
Cuando el usuario escribe `START` por el monitor serie:

1. captura 5 muestras en 1 segundo
2. cada muestra incluye datos de acelerómetro, giroscopio y magnetómetro
3. al terminar, envía las 5 muestras por I2C a la placa 2

### PLACA 2
La placa 2 actúa como **esclava I2C** con dirección `0x08`.

Cuando recibe datos:
- los muestra por pantalla
- enciende un LED durante 1 segundo

---

## Muestreo realizado
Se toma una muestra cada 200 ms durante 1 segundo:

- 200 ms
- 400 ms
- 600 ms
- 800 ms
- 1000 ms

Total: **5 muestras**

---

## Formato de los datos
Cada muestra se envía en un paquete I2C independiente.

Los valores se mandan como enteros escalados para reducir tamaño:

- acelerómetro: `g * 1000`
- giroscopio: `deg/s * 10`
- magnetómetro: `uT * 10`

---

## Uso
1. Cargar el programa de **PLACA 2** en una placa.
2. Cargar el programa de **PLACA 1** en la otra.
3. Conectar SDA, SCL y GND entre ambas.
4. Conectar el LED externo a la placa 2.
5. Abrir el monitor serie de la **PLACA 1** a `115200 baudios`.
6. Escribir:

```txt
START