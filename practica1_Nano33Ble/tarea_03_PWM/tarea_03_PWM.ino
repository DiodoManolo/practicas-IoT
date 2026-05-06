#include <Arduino.h>
#include <mbed.h>
#include "pinDefinitions.h"

const pin_size_t ADC_PIN = A0;
const pin_size_t PWM_PIN = 6;
const int ADC_BITS = 12;

// mbed::PwmOut es la clase de Mbed para manejar una salida PWM;
// el * indica que pwm es un puntero, o sea, una variable que apuntará al objeto PWM real
mbed::PwmOut* pwm = nullptr;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  analogReadResolution(ADC_BITS);

  // Crear el PWM en setup(), no como objeto global
  pwm = new mbed::PwmOut(digitalPinToPinName(PWM_PIN));

  // 5 kHz -> T = 1/5000 s = 200 us
  pwm->period_us(200);

  // Duty inicial 0%
  pwm->write(0.0f);

  Serial.println("Tarea 3: PWM a 5 kHz proporcional al ADC");
}

void loop() {
  uint16_t adc = analogRead(ADC_PIN); // Lee el valor del ADC (entre 0 y 4095)

  float duty = (float)adc / 4095.0f; // Convierte la lectura del ADC en un duty cycle entre 0.0 y 1.0
  float vin  = (adc * 3.3f) / 4095.0f; // Convierte la lectura ADC a voltios

  pwm->write(duty);

  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print(" | Vin=");
  Serial.print(vin, 3);
  Serial.print(" V | Duty=");
  Serial.print(duty * 100.0f, 1);
  Serial.println(" %");

  delay(100);
}