// Práctica 2 - Tarea 2
// ESP32 + FreeRTOS + MPU6050 por I2C
//
// Tarea 1: muestrea la aceleración cada 100 ms
// Tarea 2: manda los datos por UART cada 1 segundo
//          y activa un LED durante 200 ms

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define LED_PIN 23
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_MPU6050 mpu;

// Estructura para guardar una muestra de aceleración
typedef struct {
  unsigned long tiempo_ms;
  float ax;
  float ay;
  float az;
} MuestraAccel;

// Cola donde la tarea del sensor deja datos
// y la tarea UART los recoge
QueueHandle_t colaMuestras;

// ---------------- TAREA DE MUESTREO ----------------
void tareaMuestreo(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    sensors_event_t aceleracion, giro, temperatura;

    // Leemos el sensor
    mpu.getEvent(&aceleracion, &giro, &temperatura);

    // Guardamos los datos de aceleración
    MuestraAccel muestra;
    muestra.tiempo_ms = millis();
    muestra.ax = aceleracion.acceleration.x;
    muestra.ay = aceleracion.acceleration.y;
    muestra.az = aceleracion.acceleration.z;

    // Enviamos la muestra a la cola
    xQueueSend(colaMuestras, &muestra, 0);

    // Esperamos hasta el siguiente periodo de 100 ms
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(100));
  }
}

// ---------------- TAREA DE ENVÍO UART ----------------
void tareaEnvioUART(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    // Esta tarea se ejecuta cada 1 segundo
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(1000));

    digitalWrite(LED_PIN, HIGH);   // Activamos LED al enviar

    Serial.println();
    Serial.println("----- ENVIO DE DATOS CADA 1 s -----");
    Serial.println("tiempo_ms; ax; ay; az");

    MuestraAccel muestra;
    int contador = 0;

    // Sacamos de la cola todas las muestras disponibles
    while (xQueueReceive(colaMuestras, &muestra, 0) == pdTRUE) {
      Serial.print(muestra.tiempo_ms);
      Serial.print("; ");
      Serial.print(muestra.ax);
      Serial.print("; ");
      Serial.print(muestra.ay);
      Serial.print("; ");
      Serial.println(muestra.az);

      contador++;
    }

    Serial.print("Muestras enviadas: ");
    Serial.println(contador);

    // Mantenemos el LED encendido 200 ms
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

  Serial.println("Inicio de la Tarea 2 con FreeRTOS");

  // Inicializamos I2C en los pines típicos del ESP32
  Wire.begin(SDA_PIN, SCL_PIN);

  // Inicializamos el MPU6050
  if (!mpu.begin()) {
    Serial.println("Error: no se detecta el MPU6050");
    Serial.println("Revisa VCC, GND, SDA y SCL");

    while (true) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }

  Serial.println("MPU6050 detectado correctamente");

  // Configuración básica del sensor
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Creamos la cola.
  // Como muestreamos cada 100 ms y enviamos cada 1 s,
  // normalmente habrá unas 10 muestras por envío.
  colaMuestras = xQueueCreate(20, sizeof(MuestraAccel));

  if (colaMuestras == NULL) {
    Serial.println("Error creando la cola");
    while (true);
  }

  // Creamos la tarea de muestreo
  xTaskCreate(
    tareaMuestreo,
    "Tarea Muestreo",
    4096,
    NULL,
    2,
    NULL
  );

  // Creamos la tarea de envío por UART
  xTaskCreate(
    tareaEnvioUART,
    "Tarea UART",
    4096,
    NULL,
    1,
    NULL
  );
}

// ---------------- LOOP ----------------
void loop() {
  // El loop queda libre porque el trabajo lo hacen las tareas
  vTaskDelay(pdMS_TO_TICKS(1000));
}