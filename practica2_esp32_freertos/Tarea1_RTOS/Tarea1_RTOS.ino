// Práctica 2 - Tarea 1
// ESP32 + FreeRTOS
// Tarea 1: parpadea un LED cada 200 ms
// Tarea 2: envía "hola mundo" por UART cada 1 segundo

#define LED_PIN 23   // Pin donde conectamos el LED externo

// ---------------- TAREA DEL LED ----------------
void tareaLED(void *parameter) {
  while (true) {
    digitalWrite(LED_PIN, HIGH);        // Enciende el LED
    vTaskDelay(pdMS_TO_TICKS(200));     // Espera 200 ms

    digitalWrite(LED_PIN, LOW);         // Apaga el LED
    vTaskDelay(pdMS_TO_TICKS(200));     // Espera 200 ms
  }
}

// ---------------- TAREA DE LA UART ----------------
void tareaUART(void *parameter) {
  while (true) {
    Serial.println("hola mundo");       // Envía mensaje por el puerto serie
    vTaskDelay(pdMS_TO_TICKS(1000));    // Espera 1 segundo
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);                 // Inicializamos la comunicación serie
  pinMode(LED_PIN, OUTPUT);             // Configuramos el pin del LED como salida

  delay(1000);                          // Pequeña espera para abrir bien el monitor serie
  Serial.println("Inicio del programa con FreeRTOS");

  // Creamos la tarea del LED
  xTaskCreate(
    tareaLED,       // Función que ejecuta la tarea
    "Tarea LED",    // Nombre de la tarea
    2048,           // Tamaño de pila
    NULL,           // Parámetro de entrada
    1,              // Prioridad
    NULL            // Manejador de la tarea
  );

  // Creamos la tarea de la UART
  xTaskCreate(
    tareaUART,      // Función que ejecuta la tarea
    "Tarea UART",   // Nombre de la tarea
    2048,           // Tamaño de pila
    NULL,           // Parámetro de entrada
    1,              // Prioridad
    NULL            // Manejador de la tarea
  );
}

// ---------------- LOOP ----------------
void loop() {
  // No usamos loop porque el trabajo lo hacen las tareas de FreeRTOS.
  vTaskDelay(pdMS_TO_TICKS(1000));
}