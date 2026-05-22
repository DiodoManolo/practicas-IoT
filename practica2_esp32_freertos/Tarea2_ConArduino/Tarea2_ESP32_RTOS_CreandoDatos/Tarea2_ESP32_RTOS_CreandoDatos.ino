// Practica 2 - Tarea 2
// ESP32 + FreeRTOS
//
// Version con datos de aceleracion simulados.
//
// Tarea 1: genera una muestra cada 100 ms y la mete en una cola.
// Tarea 2: cada 1 segundo lee la cola, envia por UART y activa LED 200 ms.

#define LED_PIN 23

// Estructura para guardar una muestra de aceleracion
typedef struct {
  float ax;
  float ay;
  float az;
} MuestraAccel;

// Cola de FreeRTOS
QueueHandle_t colaMuestras;

// Variable para simular el movimiento
float t = 0.0;

// ---------------- TAREA DE MUESTREO ----------------
void tareaMuestreo(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    MuestraAccel muestra;

    // Datos simulados de aceleracion
    muestra.ax = 0.10 * sin(t);
    muestra.ay = 0.05 * cos(t);
    muestra.az = 1.00 + 0.02 * sin(t);

    t = t + 0.3;

    // Enviamos la muestra a la cola
    xQueueSend(colaMuestras, &muestra, 0);

    // Muestreo cada 100 ms
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(100));
  }
}

// ---------------- TAREA DE ENVIO UART ----------------
void tareaUART(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    // Envio cada 1 segundo
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(1000));

    // Encendemos el LED al enviar
    digitalWrite(LED_PIN, HIGH);

    Serial.println();
    Serial.println("----- ENVIO CADA 1 SEGUNDO -----");
    Serial.println("Muestra; ax(g); ay(g); az(g)");

    MuestraAccel muestra;
    int contador = 0;

    // Leemos las muestras que haya en la cola
    while (xQueueReceive(colaMuestras, &muestra, 0) == pdTRUE) {
      Serial.print(contador);
      Serial.print("; ");
      Serial.print(muestra.ax, 3);
      Serial.print("; ");
      Serial.print(muestra.ay, 3);
      Serial.print("; ");
      Serial.println(muestra.az, 3);

      contador++;
    }

    Serial.print("Numero de muestras enviadas: ");
    Serial.println(contador);
    Serial.println("--------------------------------");

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

  Serial.println("Inicio Tarea 2 - ESP32 FreeRTOS");
  Serial.println("Datos de aceleracion simulados");
  Serial.println("Comunicacion entre tareas mediante cola");

  // Creamos una cola para guardar hasta 20 muestras
  colaMuestras = xQueueCreate(20, sizeof(MuestraAccel));

  if (colaMuestras == NULL) {
    Serial.println("ERROR: No se pudo crear la cola");
    while (true);
  }

  // Tarea de muestreo cada 100 ms
  xTaskCreate(
    tareaMuestreo,
    "Tarea Muestreo",
    2048,
    NULL,
    2,
    NULL
  );

  // Tarea de envio UART cada 1 segundo
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
  // El trabajo lo hacen las tareas de FreeRTOS
  vTaskDelay(pdMS_TO_TICKS(1000));
}