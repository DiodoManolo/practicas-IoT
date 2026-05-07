# Tarea 2 - Control de LED RGB por BLE

Código para la **Práctica 4: Comunicaciones BLE y Bluetooth**.  
El objetivo de esta tarea es implementar un dispositivo BLE periférico con un **GATT** que contenga una característica desde la cual se pueda cambiar el color del LED integrado de la placa.

## Objetivo

El **Arduino Nano 33 BLE** actúa como periférico BLE y crea un servicio personalizado con una característica de lectura y escritura.

Desde la app **nRF Connect**, el usuario puede escribir comandos en la característica BLE para cambiar el color del LED RGB.

## Dispositivo BLE

Nombre anunciado:

```text
MANOLO_LED