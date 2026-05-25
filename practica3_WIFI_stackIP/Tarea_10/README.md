# Tarea 10.d - Suscripción al feed de Adafruit IO desde el ESP32

## Descripción

En este apartado el ESP32 no solo envía datos al feed de Adafruit IO, sino que además se suscribe a él para recibir actualizaciones cuando el valor cambia.

De este modo, el ESP32 puede enterarse de los nuevos datos escritos desde el navegador o desde una herramienta como apirequest.

---

## Objetivo

Suscribirse a un feed de Adafruit IO mediante MQTT y comprobar que el ESP32 recibe actualizaciones cuando se escribe un nuevo valor desde el navegador.

---

## Funcionamiento

El programa realiza los siguientes pasos:

1. El ESP32 se conecta a la red WiFi.
2. Se conecta a Adafruit IO mediante MQTT.
3. Se suscribe al feed `temperatura`.
4. Asocia una función callback que se ejecuta cuando llega un nuevo valor.
5. Cuando se escribe un valor nuevo en el feed desde el navegador, el ESP32 lo recibe y lo muestra por el monitor serie.

Además, el ESP32 puede seguir publicando sus propios valores al mismo feed.

---

## Prueba de funcionamiento

Para comprobar que funciona:

1. Se carga el programa en el ESP32.
2. Se abre el monitor serie.
3. Se escribe un valor nuevo en el feed `temperatura` desde el navegador o desde apirequest.
4. El ESP32 muestra por serie el valor recibido.

Ejemplo:

```text
Valor recibido desde Adafruit IO: 99