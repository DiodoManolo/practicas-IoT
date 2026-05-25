# Tarea 6.a - Envío de datos simulados de acelerómetro por TCP con ESP32

## Descripción

En esta tarea se adapta el código del ESP32 para que se conecte a una red WiFi, establezca una conexión con un servidor TCP y envíe datos de aceleración.

Como la placa utilizada no dispone de acelerómetro integrado, en lugar de leer un sensor real se generan **datos aleatorios simulados** para las tres componentes de aceleración:

- `ax`
- `ay`
- `az`

De este modo, el ESP32 se comporta como si estuviera enviando datos reales de un acelerómetro.

---

## Objetivo

Conectar el ESP32 a la WiFi, abrir una conexión TCP con un servidor y enviar periódicamente datos simulados de aceleración.

---

## Funcionamiento

El programa realiza los siguientes pasos:

1. Se conecta a la red WiFi.
2. Se conecta a un servidor TCP en el PC.
3. Genera valores aleatorios para `ax`, `ay` y `az`.
4. Envía esos valores por TCP en formato texto.

Los datos se envían en líneas con este formato:

```text
ax;ay;az