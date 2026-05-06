## Tarea 5 - Comunicación entre dos placas mediante I2C

### Objetivo
Conectar dos placas Arduino Nano 33 BLE Sense mediante el bus I2C y hacer que una de ellas controle el encendido y apagado de un LED externo conectado a la otra.

La idea es que:

- una placa actúe como **maestra I2C**
- la otra actúe como **esclava I2C**
- la maestra envíe comandos
- la esclava encienda o apague un LED externo mediante un pin digital

---

## Material necesario

- 2 placas Arduino Nano 33 BLE Sense
- 1 LED
- 1 resistencia de 220 Ω
- cables de conexión
- 2 cables USB

---

## Conexiones

### Comunicación I2C entre placas
- SDA (maestra) → SDA (esclava)
- SCL (maestra) → SCL (esclava)
- GND (maestra) → GND (esclava)

### LED externo en la placa esclava
- pin digital **D6** → resistencia **220 Ω** → ánodo del LED
- cátodo del LED → **GND**

---

## Funcionamiento del sistema

Se han implementado dos programas diferentes:

### 1. Placa maestra
La placa maestra recibe comandos por el monitor serie:

- `ON`
- `OFF`

En función del texto recibido:

- envía `1` por I2C para encender el LED
- envía `0` por I2C para apagar el LED

### 2. Placa esclava
La placa esclava escucha en la dirección I2C `0x08`.

Cuando recibe un byte:
- si recibe `1`, pone el pin digital del LED a nivel alto
- si recibe `0`, pone el pin digital del LED a nivel bajo

---

## Protocolo utilizado

Se ha definido un protocolo muy simple de 1 byte:

- `1` → encender LED
- `0` → apagar LED

---

## Explicación del código

### Placa maestra
La maestra inicia el bus I2C con:

```cpp
Wire.begin();