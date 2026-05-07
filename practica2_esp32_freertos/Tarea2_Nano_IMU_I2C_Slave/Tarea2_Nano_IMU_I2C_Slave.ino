// Nano_IMU_I2C_Slave
// Arduino Nano 33 BLE Sense
// Lee la IMU integrada y envia ax;ay;az por I2C a la ESP32

#include <Wire.h>
#include <Arduino_LSM9DS1.h>

#define I2C_ADDRESS 0x08
#define MSG_LEN 24

char mensaje[MSG_LEN] = "0.000;0.000;0.000";

unsigned long tiempoAnterior = 0;
const unsigned long PERIODO_LECTURA = 100;

volatile int peticiones = 0;

void enviarDatos() {
  Wire.write((uint8_t *)mensaje, MSG_LEN);
  peticiones++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Nano 33 BLE Sense - Esclavo I2C");

  if (!IMU.begin()) {
    Serial.println("ERROR: No se detecta la IMU");
    while (true);
  }

  Serial.println("IMU detectada correctamente");

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(enviarDatos);

  tiempoAnterior = millis();
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= PERIODO_LECTURA) {
    tiempoAnterior = tiempoAnterior + PERIODO_LECTURA;

    if (IMU.accelerationAvailable()) {
      float x, y, z;
      IMU.readAcceleration(x, y, z);

      char nuevoMensaje[MSG_LEN];
      snprintf(nuevoMensaje, MSG_LEN, "%.3f;%.3f;%.3f", x, y, z);

      noInterrupts();
      strncpy(mensaje, nuevoMensaje, MSG_LEN);
      mensaje[MSG_LEN - 1] = '\0';
      interrupts();

      Serial.print(mensaje);
      Serial.print(" | peticiones ESP32: ");
      Serial.println(peticiones);
    }
  }
}