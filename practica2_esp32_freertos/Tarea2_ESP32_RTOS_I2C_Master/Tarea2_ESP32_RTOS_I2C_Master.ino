// ESP32_RTOS_I2C_Master
// ESP32 + FreeRTOS
// Pide datos a la Nano por I2C cada 100 ms
// Envia por UART cada 1 segundo
// Enciende LED durante 200 ms en cada envio

#include <Wire.h>
#include <string.h>

#define LED_PIN 23

#define SDA_PIN 21
#define SCL_PIN 22

#define NANO_ADDRESS 0x08
#define MSG_LEN 24

char muestras[10][MSG_LEN];

int indice = 0;
int numMuestras = 0;

// ---------------- TAREA DE MUESTREO ----------------
void tareaMuestreo(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    char dato[MSG_LEN];
    int i = 0;

    int bytesRecibidos = Wire.requestFrom(NANO_ADDRESS, MSG_LEN);

    while (Wire.available() && i < MSG_LEN - 1) {
      char c = Wire.read();

      if (c != '\0') {
        dato[i] = c;
        i++;
      }
    }

    dato[i] = '\0';

    if (bytesRecibidos > 0 && i > 0) {
      strcpy(muestras[indice], dato);

      indice++;

      if (indice >= 10) {
        indice = 0;
      }

      if (numMuestras < 10) {
        numMuestras++;
      }
    }

    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(100));
  }
}

// ---------------- TAREA UART ----------------
void tareaUART(void *parameter) {
  TickType_t tiempoAnterior = xTaskGetTickCount();

  while (true) {
    vTaskDelayUntil(&tiempoAnterior, pdMS_TO_TICKS(1000));

    digitalWrite(LED_PIN, HIGH);

    Serial.println();
    Serial.println("----- ENVIO CADA 1 SEGUNDO -----");
    Serial.println("Muestra; ax(g); ay(g); az(g)");

    if (numMuestras == 0) {
      Serial.println("No se han recibido datos. Revisa I2C.");
    } else {
      for (int i = 0; i < numMuestras; i++) {
        Serial.print(i);
        Serial.print("; ");
        Serial.println(muestras[i]);
      }
    }

    Serial.println("--------------------------------");

    numMuestras = 0;

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

  Serial.println("ESP32 - Maestro I2C con FreeRTOS");
  Serial.println("Pidiendo datos a la Nano 33 BLE Sense");

  Wire.begin(SDA_PIN, SCL_PIN);

  // Bajamos la velocidad I2C para que sea mas estable entre placas
  Wire.setClock(50000);

  xTaskCreate(
    tareaMuestreo,
    "Tarea Muestreo",
    4096,
    NULL,
    2,
    NULL
  );

  xTaskCreate(
    tareaUART,
    "Tarea UART",
    4096,
    NULL,
    1,
    NULL
  );
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}