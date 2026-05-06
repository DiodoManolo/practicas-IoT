# Práctica 2 - Tarea 1: FreeRTOS con ESP32

## Objetivo

Crear un firmware basado en FreeRTOS para ESP32 con dos tareas independientes:

- Una tarea que parpadea un LED cada 200 ms.
- Otra tarea que envía el mensaje `"hola mundo"` por UART cada segundo.

## Hardware utilizado

- Placa ESP32-DevKitC V4 con módulo ESP32-WROOM-32D.
- LED externo.
- Resistencia de 220 Ω o 330 Ω.
- Cable USB.

## Conexión del LED

El LED externo se conecta al GPIO23:

```text
GPIO23 ─── resistencia ─── ánodo LED
GND    ───────────────── cátodo LED