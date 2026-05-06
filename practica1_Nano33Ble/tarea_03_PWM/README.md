## Tarea 3 - PWM a 5 kHz proporcional a la lectura del ADC

### Objetivo
Generar una salida PWM de 5 kHz cuyo ciclo de trabajo sea proporcional a la tensión analógica aplicada en la entrada ADC. La señal analógica se obtiene con un potenciómetro alimentado a 3.3 V y se comprueba tanto la entrada como la salida PWM con el osciloscopio.

### Descripción
Se conecta un potenciómetro a la placa Arduino Nano 33 BLE Sense utilizando:

- un extremo a **3V3**
- el otro extremo a **GND**
- el terminal central al pin **A0**

El programa lee el valor del ADC en `A0` con resolución de 12 bits, por lo que la lectura varía entre `0` y `4095`.

A partir de esa lectura se calcula un valor proporcional entre `0.0` y `1.0`, que se utiliza como ciclo de trabajo del PWM. La frecuencia del PWM se fija en **5 kHz**, lo que corresponde a un período de **200 µs**.

### Funcionamiento
- Si la tensión de entrada es cercana a `0 V`, el duty cycle es cercano al `0%`.
- Si la tensión de entrada es cercana a `1.65 V`, el duty cycle es aproximadamente del `50%`.
- Si la tensión de entrada es cercana a `3.3 V`, el duty cycle es cercano al `100%`.

La frecuencia del PWM permanece constante en 5 kHz y únicamente cambia el ancho del pulso.

### Fórmulas usadas

Conversión ADC a tensión:

`Vin = (ADC / 4095) · 3.3`

Conversión ADC a duty cycle:

`duty = ADC / 4095`

Duty cycle en porcentaje:

`Duty(%) = (ADC / 4095) · 100`

### Comprobación con osciloscopio
Se comprobó:

1. **Entrada analógica** en el cursor del potenciómetro:
   - tensión continua variable entre `0 V` y `3.3 V`

2. **Salida PWM** en el pin de salida:
   - frecuencia fija de `5 kHz`
   - amplitud aproximada entre `0 V` y `3.3 V`
   - duty cycle variable en función de la tensión de entrada

A 5 kHz, el período esperado es:

`T = 1 / 5000 = 200 µs`

Por tanto:
- al `25%` de duty, el tiempo en alto es `50 µs`
- al `50%` de duty, el tiempo en alto es `100 µs`
- al `75%` de duty, el tiempo en alto es `150 µs`

### Conclusión
La práctica permite comprobar cómo convertir una medida analógica obtenida por el ADC en una señal PWM de frecuencia fija y ciclo de trabajo variable. De esta forma, la salida PWM representa digitalmente el nivel de la señal analógica de entrada.