## Tarea 4 - Protocolo UART con `String` para controlar ADC y PWM

### Objetivo
Implementar un protocolo simple por UART utilizando el tipo `String` para gestionar periféricos de la placa Arduino Nano 33 BLE Sense.

El sistema debe reconocer los siguientes comandos:

- `ADC` → envía la lectura actual del ADC
- `ADC(x)` → envía la lectura del ADC cada `x` segundos
- `ADC(0)` → detiene el envío periódico
- `PWM(x)` → ajusta el duty cycle del PWM con valores de `0` a `9`

### Descripción del funcionamiento
La comunicación se realiza a través del puerto serie (`Serial`) a 115200 baudios.  
Los comandos se envían como texto desde el monitor serie y se procesan al recibir fin de línea.

Para gestionar los comandos se utiliza el tipo `String`, que permite:

- almacenar la línea recibida
- recortar espacios con `trim()`
- comprobar patrones con `startsWith()` y `endsWith()`
- extraer argumentos con `substring()`

Se ha reservado memoria al `String` con `reserve(32)` para reducir realojos dinámicos y evitar fragmentación innecesaria de memoria.

### Protocolo implementado

#### 1. Comando `ADC`
Devuelve una lectura instantánea del ADC.

Ejemplo:
```txt
ADC