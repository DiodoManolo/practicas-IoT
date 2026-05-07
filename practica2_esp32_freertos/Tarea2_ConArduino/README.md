# Práctica 2 - Tarea 2 adaptada a Arduino Nano 33 BLE Sense

## Objetivo

Leer la aceleración de un sensor inercial cada 100 ms y enviar los datos por UART cada segundo. Cada vez que se envían datos, se activa el LED integrado durante 200 ms.

## Motivo de la adaptación

La práctica original está pensada para realizarse con una placa ESP32 y un sensor inercial conectado por I2C o SPI. Sin embargo, la placa ESP32 utilizada no dispone de acelerómetro integrado.

Por este motivo, se ha optado por realizar esta tarea con la Arduino Nano 33 BLE Sense, que sí incorpora una IMU integrada. De esta forma se puede comprobar correctamente el muestreo de aceleración y el envío de datos por UART.

Esta versión no utiliza FreeRTOS como en el ESP32. La Arduino Nano 33 BLE Sense trabaja sobre el entorno basado en Mbed OS, por lo que no se usan funciones como `xTaskCreate()` o `vTaskDelay()`. En su lugar, se realiza la temporización con `millis()`.

## Posible solución alternativa

Una solución más cercana a la práctica original sería conectar la Arduino Nano 33 BLE Sense y la ESP32 por I2C:

- La Nano leería su acelerómetro integrado.
- La ESP32 actuaría como maestro I2C.
- La ESP32 pediría los datos a la Nano cada 100 ms.
- La ESP32 ejecutaría las tareas FreeRTOS y enviaría los datos por UART.