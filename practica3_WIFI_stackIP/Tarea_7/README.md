# Tarea 7 - Servidor web con hora y botón de reset en ESP32

## Descripción

En esta tarea se implementa un servidor web en el ESP32. Al acceder desde un navegador a la IP del módulo, se muestra una página web sencilla con la hora actual y un botón para resetear la hora mostrada a `00:00`.

La hora del ESP32 se obtiene previamente mediante NTP, conectando el módulo a una red WiFi.

---

## Objetivo

Montar un servidor web en el ESP32 que:

- muestre la hora actual en una página web,
- permita acceder desde un navegador,
- y tenga un botón para resetear la hora mostrada a las `0:00`.

---

## Funcionamiento

El programa realiza los siguientes pasos:

1. El ESP32 se conecta a la red WiFi.
2. Sincroniza la hora mediante un servidor NTP.
3. Inicia un servidor web HTTP en el puerto `80`.
4. Cuando un cliente entra en la IP del ESP32, el servidor genera dinámicamente una página HTML con la hora actual.
5. La página incluye un botón que llama a una ruta de reset para llevar la hora mostrada a `00:00`.

---

## Estructura de la página web

La página generada por el ESP32 contiene:

- un título,
- la hora actual,
- un botón de reset,
- y un enlace para refrescar la página.

La página se genera directamente desde el código, sin necesidad de archivos HTML externos.

---

## Funcionamiento del reset

El botón no cambia la hora real del sistema sincronizada por NTP, sino que aplica un **offset** sobre la hora mostrada en pantalla.

De este modo, al pulsar el botón, la hora visualizada pasa a ser `00:00:00`, manteniendo una implementación sencilla y fácil de explicar.

---

## Acceso a la página

Una vez cargado el programa, el ESP32 muestra por el monitor serie la IP asignada en la red WiFi.

Ejemplo:

```text
WiFi conectada
IP del ESP32: 10.16.127.180
Servidor web iniciado