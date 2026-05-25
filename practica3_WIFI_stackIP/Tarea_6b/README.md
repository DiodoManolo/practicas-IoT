# Tarea 6.b - Recepción de datos por TCP en Python

## Descripción

En este apartado se modifica el programa de Python para que deje de recibir los datos por el puerto serie y pase a recibirlos a través de una conexión TCP.

El PC actúa como **servidor TCP** y el ESP32 como **cliente TCP**. Una vez establecida la conexión, el programa en Python recibe las líneas de datos enviadas por el ESP32 y las muestra por pantalla.

---

## Objetivo

Recibir en Python los datos enviados por el ESP32 a través de WiFi usando un servidor TCP.

---

## Funcionamiento

El programa en Python realiza los siguientes pasos:

1. Crea un socket servidor TCP.
2. Escucha en el puerto `5000`.
3. Espera la conexión del ESP32.
4. Recibe las líneas de datos enviadas por el ESP32.
5. Muestra por pantalla cada línea recibida.

Los datos recibidos tienen este formato:

```text
ax;ay;az