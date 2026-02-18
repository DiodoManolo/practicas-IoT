#include <Arduino.h>
#include <mbed.h>

mbed::Ticker ticker;

volatile bool tick_10s = false;   // bandera que pone la ISR
unsigned long lastPrintMs = 0;

void onTimer10s()
{
  // ISR: NO analogRead, NO Serial
  tick_10s = true;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) { }  // ok

  // Si te da problemas, comenta esta línea
  analogReadResolution(12);

  ticker.attach(&onTimer10s, 10.0f);

  Serial.println("OK: Timer cada 10s activo. Espera al primer disparo...");
  Serial.println("Nota: Como no hay nada conectado a A0, el ADC puede variar.");
}

void loop()
{
  if (tick_10s)
  {
    tick_10s = false;

    // Ahora sí: leer ADC fuera de la ISR
    uint16_t adc = analogRead(A0);

    // Medir tiempo entre ticks
    unsigned long now = millis();
    unsigned long dt = (lastPrintMs == 0) ? 0 : (now - lastPrintMs);
    lastPrintMs = now;

    // Señal visual: parpadeo LED
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.print("TICK 10s | dt(ms)=");
    Serial.print(dt);
    Serial.print(" | ADC(A0)=");
    Serial.println(adc);
  }
}
