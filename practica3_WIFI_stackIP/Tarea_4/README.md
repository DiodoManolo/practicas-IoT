
---

## README Tarea 4

```markdown
# Tarea 4 - Cliente TCP en ESP32 enviando la hora local

## Descripción

En esta tarea se sustituye uno de los extremos del chat por el módulo hardware, utilizando el ESP32 como cliente TCP. El objetivo es que el ESP32 se conecte a un servidor TCP ejecutado en el PC y envíe cada segundo su hora local.

Para ello, primero se conecta a la red WiFi, sincroniza la hora mediante NTP y, posteriormente, establece una conexión TCP con el servidor software.

---

## Objetivo

Configurar el ESP32 como cliente TCP y enviar cada segundo la hora local a un servidor TCP en el PC.

---

## Arquitectura utilizada

- **PC**: servidor TCP mediante SocketTest
- **ESP32**: cliente TCP
- **Red**: WiFi

---

## Funcionamiento

El programa realiza los siguientes pasos:

1. Se conecta a una red WiFi.
2. Sincroniza la hora local mediante NTP.
3. Intenta conectarse al servidor TCP del PC.
4. Una vez establecida la conexión, envía cada segundo la hora local en formato `HH:MM:SS`.

---

## Elementos utilizados

- Placa ESP32
- Arduino IDE
- SocketTest en el PC
- Monitor serie
- Red WiFi

---

## Configuración del servidor

En el PC se ejecuta SocketTest en modo **Server**, escuchando en el puerto:

```text
5000