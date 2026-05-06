# Práctica 2 - Tarea 2: FreeRTOS con ESP32 y sensor inercial

## Objetivo

Diseñar un firmware basado en FreeRTOS para ESP32 que lea la aceleración de un sensor inercial cada 100 ms y envíe los datos por UART cada segundo. Cada vez que se envían datos, se activa un LED durante 200 ms.

## Hardware utilizado

- ESP32-DevKitC V4 con módulo ESP32-WROOM-32D.
- Sensor inercial MPU6050 conectado por I2C.
- LED externo.
- Resistencia de 220 Ω o 330 Ω.

## Conexiones

### MPU6050

```text
MPU6050 VCC -> ESP32 3V3
MPU6050 GND -> ESP32 GND
MPU6050 SDA -> ESP32 GPIO21
MPU6050 SCL -> ESP32 GPIO22