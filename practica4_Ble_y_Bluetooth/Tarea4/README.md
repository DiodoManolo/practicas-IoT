# Tarea 4 - Activar y desactivar acelerómetro por BLE

Código para la **Práctica 4: Comunicaciones BLE y Bluetooth**.  
El objetivo de esta tarea es añadir una característica BLE de **solo escritura** para activar o desactivar el envío de datos del acelerómetro.

## Objetivo

El **Arduino Nano 33 BLE** actúa como periférico BLE y crea un GATT con dos características:

```text
1. Acelerómetro → Read + Notify
2. Control      → Write