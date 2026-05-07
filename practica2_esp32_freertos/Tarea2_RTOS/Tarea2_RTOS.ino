// Práctica 2 - Tarea 2
// ESP32 + FreeRTOS + MPU6050 por I2C
//
// Tarea 1: lee aceleración cada 100 ms
// Tarea 2: manda los datos por UART cada 1 s
//          y enciende un LED durante 200 ms

#include <Wire.h>

#define LED_PIN 23
#define SDA_PIN 21
#define SCL_PIN 22

#define MPU_ADDR 0x68   // Dirección I2C normal del MPU6050

// Variables globales donde guardamos la última aceleración leída
float ax = 0;
float ay = 0;
float az = 0;

int contadorMuestras = 0;

// ---------------- TAREA DE MUESTREO ----------------
void tareaMuestreo(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    // Pedimos al MPU6050 que empiece a leer desde el registro 0x3B,
    // que es donde empiezan los datos de aceleración
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);

    // Pedimos 6 bytes: 2 para X, 2 para Y y 2 para Z
    Wire.requestFrom(MPU_ADDR, 6, true);

    if (Wire.available() == 6) {
      int16_t rawX = (Wire.read() << 8) | Wire.read();
      int16_t rawY = (Wire.read() << 8) | Wire.read();
      int16_t rawZ = (Wire.read() << 8) | Wire.read();

      // Convertimos a m/s2.
      // En rango ±2g, el MPU6050 usa 16384 cuentas por cada g.
      ax = (rawX / 16384.0) * 9.81;
      ay = (rawY / 16384.0) * 9.81;
      az = (rawZ / 16384.0) * 9.81;

      contadorMuestras++;
    }

    // Espera hasta completar un periodo de 100 ms
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(100));
  }
}

// ---------------- TAREA DE ENVÍO UART ----------------
void tareaUART(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    // Esta tarea se ejecuta cada 1 segundo
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(1000));

    // Encendemos LED al enviar datos
    digitalWrite(LED_PIN, HIGH);

    Serial.print("Muestras tomadas en el ultimo segundo: ");
    Serial.println(contadorMuestras);

    Serial.print("ax = ");
    Serial.print(ax, 3);
    Serial.print(" m/s2, ay = ");
    Serial.print(ay, 3);
    Serial.print(" m/s2, az = ");
    Serial.print(az, 3);
    Serial.println(" m/s2");

    Serial.println("--------------------------------");

    // Reiniciamos el contador para el siguiente segundo
    contadorMuestras = 0;

    // LED encendido durante 200 ms
    vTaskDelay(pdMS_TO_TICKS(200));
    digitalWrite(LED_PIN, LOW);
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Inicio Tarea 2 - ESP32 FreeRTOS + MPU6050");

  // Inicializamos I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Despertamos el MPU6050.
  // El registro 0x6B controla el modo sleep.
  // Escribimos 0 para sacarlo del modo reposo.
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  byte error = Wire.endTransmission(true);

  if (error != 0) {
    Serial.println("ERROR: No se detecta el MPU6050");
    Serial.println("Revisa VCC, GND, SDA y SCL");

    while (true) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }

  Serial.println("MPU6050 detectado correctamente");

  // Creamos tarea de muestreo
  xTaskCreate(
    tareaMuestreo,
    "Tarea Muestreo",
    4096,
    NULL,
    2,
    NULL
  );

  // Creamos tarea UART
  xTaskCreate(
    tareaUART,
    "Tarea UART",
    4096,
    NULL,
    1,
    NULL
  );
}

// ---------------- LOOP ----------------
void loop() {
  // El trabajo lo hacen las tareas
  vTaskDelay(pdMS_TO_TICKS(1000));
}