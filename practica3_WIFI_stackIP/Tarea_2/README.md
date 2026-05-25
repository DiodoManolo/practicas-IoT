# Tarea 2 - Sincronización de hora mediante NTP en ESP32

## Descripción

En esta tarea se configura el ESP32 para obtener la hora actual a través de un servidor NTP (*Network Time Protocol*). Para ello, el módulo se conecta primero a una red WiFi y, una vez que tiene acceso a Internet, consulta servidores NTP públicos para sincronizar su reloj interno.

El objetivo de esta tarea es que el ESP32 pueda mostrar por el monitor serie la hora local correcta.

---

## Objetivo

Poner en hora el módulo ESP32 mediante un servidor NTP.

---

## Funcionamiento

El programa realiza los siguientes pasos:

1. Conecta el ESP32 a una red WiFi.
2. Consulta servidores NTP públicos.
3. Configura la zona horaria de España peninsular.
4. Obtiene la hora local ya ajustada.
5. Muestra la hora por el monitor serie.

Para ello se utilizan las librerías:

- `WiFi.h`
- `time.h`

---