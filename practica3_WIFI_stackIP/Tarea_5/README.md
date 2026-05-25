# Tarea 5 - Control start/stop del envío de hora por TCP

## Descripción

En esta tarea se añade una capa de control sobre la comunicación TCP de la tarea anterior. El ESP32 sigue actuando como cliente TCP, pero ya no envía la hora continuamente. En su lugar, espera órdenes enviadas desde el servidor.

Si recibe el comando `start`, comienza a enviar la hora local cada segundo. Si recibe el comando `stop`, deja de enviarla.

---

## Objetivo

Controlar desde el servidor TCP cuándo el ESP32 debe empezar y cuándo debe parar el envío periódico de la hora local.

---

## Funcionamiento

El sistema se organiza así:

- **PC**: servidor TCP mediante SocketTest
- **ESP32**: cliente TCP

El ESP32:

1. se conecta a la red WiFi,
2. sincroniza la hora mediante NTP,
3. se conecta al servidor TCP,
4. espera comandos de texto,
5. activa o desactiva el envío de hora según reciba `start` o `stop`.

La lógica del control se basa en una variable booleana:
- `true` → envía la hora
- `false` → no envía nada

---

## Prueba de funcionamiento

- Al enviar `start` desde SocketTest, el ESP32 comienza a mandar la hora cada segundo.
- Al enviar `stop`, deja de enviarla.

Ejemplo de comandos:

```text
start
stop