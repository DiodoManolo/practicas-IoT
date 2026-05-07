# Tarea 3 - Acelerómetro por BLE con Read y Notify

Código para la **Práctica 4: Comunicaciones BLE y Bluetooth**.  
El objetivo de esta tarea es implementar un dispositivo BLE periférico con un **GATT** que permita consultar los valores del acelerómetro desde la app **nRF Connect**.

## Objetivo

El **Arduino Nano 33 BLE** actúa como periférico BLE y publica una característica con los valores del acelerómetro en formato:

```text
ax;ay;az