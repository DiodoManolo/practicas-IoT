// P1 - Tarea 1: Leer ADC cada 1 segundo y mostrar por consola
// Arduino Nano 33 BLE Sense (3.3V máx en entradas analógicas)

const int ADC_PIN = A0;

// Si quieres 12 bits (0..4095) en esta placa, descomenta:
const int ADC_BITS = 12;  // 10 o 12 típicamente

unsigned long t0 = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { }  // espera a abrir el monitor serie (USB)

  analogReadResolution(ADC_BITS);   // 12 bits => 0..4095
}

void loop() {
  if (millis() - t0 >= 1000) {   // cada 1000 ms
    t0 = millis();

    int adc = analogRead(ADC_PIN);

    // Convertir a voltios (suponiendo Vref = 3.3V)
    float v = (adc * 3.3f) / ((1 << ADC_BITS) - 1);

    // Formateo con sprintf (como en el material de apoyo)
    char msg[64];
    snprintf(msg, sizeof(msg), "ADC=%d | V=%.3f V", adc, v);
    Serial.println(msg);
  }
}