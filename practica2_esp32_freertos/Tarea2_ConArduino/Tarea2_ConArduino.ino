// Práctica 2 - Tarea 2
// Arduino Nano 33 BLE Sense
//
// Sensor usado: acelerómetro integrado
//
// Lee aceleración cada 100 ms
// Envía los datos cada 1 segundo por UART
// Enciende el LED integrado durante 200 ms cada vez que envía

#include <Arduino_LSM9DS1.h>

#define LED_PIN LED_BUILTIN

// Tiempos de la práctica
const unsigned long PERIODO_MUESTREO = 100;   // 100 ms
const unsigned long PERIODO_ENVIO = 1000;     // 1 segundo
const unsigned long TIEMPO_LED = 200;         // 200 ms

// Guardamos 10 muestras porque 1 segundo / 100 ms = 10
float ax[10];
float ay[10];
float az[10];

int numMuestras = 0;

// Variables de tiempo
unsigned long tiempoMuestreoAnterior = 0;
unsigned long tiempoEnvioAnterior = 0;
unsigned long tiempoLedEncendido = 0;

bool ledActivo = false;

void setup() {
  Serial.begin(115200);

  // En placas con USB nativo, esto espera a que abras el monitor serie
  while (!Serial);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Inicio practica - Arduino Nano 33 BLE Sense");
  Serial.println("Leyendo acelerometro integrado");

  // Inicializamos la IMU
  if (!IMU.begin()) {
    Serial.println("ERROR: No se detecta la IMU");

    while (true) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }

  Serial.println("IMU detectada correctamente");

  tiempoMuestreoAnterior = millis();
  tiempoEnvioAnterior = millis();
}

void loop() {
  unsigned long tiempoActual = millis();

  // --------------------------------------------------
  // 1. Muestreo de aceleracion cada 100 ms
  // --------------------------------------------------
  if (tiempoActual - tiempoMuestreoAnterior >= PERIODO_MUESTREO) {
    tiempoMuestreoAnterior = tiempoMuestreoAnterior + PERIODO_MUESTREO;

    if (IMU.accelerationAvailable()) {
      float x, y, z;

      IMU.readAcceleration(x, y, z);

      if (numMuestras < 10) {
        ax[numMuestras] = x;
        ay[numMuestras] = y;
        az[numMuestras] = z;

        numMuestras++;
      }
    }
  }

  // --------------------------------------------------
  // 2. Envio por UART cada 1 segundo
  // --------------------------------------------------
  if (tiempoActual - tiempoEnvioAnterior >= PERIODO_ENVIO) {
    tiempoEnvioAnterior = tiempoEnvioAnterior + PERIODO_ENVIO;

    // Encendemos LED al enviar
    digitalWrite(LED_PIN, HIGH);
    ledActivo = true;
    tiempoLedEncendido = tiempoActual;

    Serial.println();
    Serial.println("----- ENVIO CADA 1 SEGUNDO -----");
    Serial.println("Muestra; ax(g); ay(g); az(g)");

    for (int i = 0; i < numMuestras; i++) {
      Serial.print(i);
      Serial.print("; ");
      Serial.print(ax[i], 3);
      Serial.print("; ");
      Serial.print(ay[i], 3);
      Serial.print("; ");
      Serial.println(az[i], 3);
    }

    Serial.print("Numero de muestras enviadas: ");
    Serial.println(numMuestras);
    Serial.println("--------------------------------");

    // Reiniciamos contador para el siguiente segundo
    numMuestras = 0;
  }

  // --------------------------------------------------
  // 3. Apagar LED despues de 200 ms
  // --------------------------------------------------
  if (ledActivo && tiempoActual - tiempoLedEncendido >= TIEMPO_LED) {
    digitalWrite(LED_PIN, LOW);
    ledActivo = false;
  }
}