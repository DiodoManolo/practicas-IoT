#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

/*
  TAREA 4 - Control del acelerómetro por BLE

  Este código combina la tarea 3 y la tarea 4.

  El Arduino Nano 33 BLE actúa como periférico BLE y crea un GATT con:

  1) Característica de acelerómetro:
     - BLERead
     - BLENotify
     Permite leer ax;ay;az y recibir actualizaciones automáticas.

  2) Característica de control:
     - BLEWrite
     Permite activar o desactivar la actualización del acelerómetro.

  Comandos desde nRF Connect:
     1  -> activar acelerómetro
     0  -> desactivar acelerómetro
*/

// Nombre que aparece en nRF Connect
const char* deviceName = "MANOLO_IMU_CTRL";

// Servicio BLE personalizado
BLEService imuService("4D414E4F-4C4F-4000-8000-00805F9B34FB");

// Característica del acelerómetro
// Solo lectura y notificaciones
BLEStringCharacteristic accelCharacteristic(
  "4D414E4F-4C4F-4001-8000-00805F9B34FB",
  BLERead | BLENotify,
  40
);

// Característica de control del acelerómetro
// Solo escritura
BLEStringCharacteristic accelControlCharacteristic(
  "4D414E4F-4C4F-4002-8000-00805F9B34FB",
  BLEWrite,
  5
);

// Variable que indica si el acelerómetro está activo o no
bool acelerometroActivo = true;

// Periodo de actualización de la medida
const unsigned long samplePeriodMs = 500;
unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(9600);
  delay(1500);

  Serial.println("Iniciando BLE + IMU...");

  // Inicializamos la IMU
  if (!IMU.begin()) {
    Serial.println("Error: no se pudo iniciar la IMU");
    while (1);
  }

  Serial.println("IMU iniciada correctamente");

  // Inicializamos BLE
  if (!BLE.begin()) {
    Serial.println("Error: no se pudo iniciar BLE");
    while (1);
  }

  // Nombre visible en nRF Connect
  BLE.setLocalName(deviceName);
  BLE.setDeviceName(deviceName);

  // Añadimos las características al servicio
  imuService.addCharacteristic(accelCharacteristic);
  imuService.addCharacteristic(accelControlCharacteristic);

  // Añadimos el servicio al GATT del Arduino
  BLE.addService(imuService);

  // Valor inicial de la característica del acelerómetro
  accelCharacteristic.writeValue("0.000;0.000;0.000");

  // Anunciamos el servicio BLE
  BLE.setAdvertisedService(imuService);

  // Comenzamos el advertising
  BLE.advertise();

  Serial.println("Advertising iniciado");
  Serial.print("Nombre BLE: ");
  Serial.println(deviceName);
  Serial.println("Caracteristica acelerometro: Read + Notify");
  Serial.println("Caracteristica control: Write only");
  Serial.println("Escribe 1 para activar y 0 para desactivar");
}

void loop() {
  // Mantiene activa la comunicación BLE
  BLE.poll();

  // Comprobamos si el móvil ha escrito algo en la característica de control
  if (accelControlCharacteristic.written()) {
    String comando = accelControlCharacteristic.value();

    comando.trim();
    comando.toUpperCase();

    Serial.print("Comando recibido en control: ");
    Serial.println(comando);

    if (comando == "1" || comando == "ON") {
      acelerometroActivo = true;
      Serial.println("Acelerometro ACTIVADO");

      // Actualizamos la característica para avisar del estado
      accelCharacteristic.writeValue("ACCEL_ON");
    }
    else if (comando == "0" || comando == "OFF") {
      acelerometroActivo = false;
      Serial.println("Acelerometro DESACTIVADO");

      // Al desactivar, dejamos de actualizar las medidas
      accelCharacteristic.writeValue("ACCEL_OFF");
    }
    else {
      Serial.println("Comando no reconocido. Usa 1/0 u ON/OFF.");
    }
  }

  // Si el acelerómetro está desactivado, no actualizamos valores
  if (!acelerometroActivo) {
    return;
  }

  // Actualizamos la medida cada samplePeriodMs
  if (millis() - lastSampleTime >= samplePeriodMs) {
    lastSampleTime = millis();

    float ax, ay, az;

    // Si hay una nueva muestra de aceleración disponible
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(ax, ay, az);

      // Creamos un mensaje tipo: "0.012;-0.034;0.987"
      char buffer[40];
      snprintf(buffer, sizeof(buffer), "%.3f;%.3f;%.3f", ax, ay, az);

      // Actualizamos la característica del acelerómetro.
      // Si Notify está activado en nRF Connect, el móvil recibe el dato automáticamente.
      accelCharacteristic.writeValue(buffer);

      Serial.print("Acelerometro: ");
      Serial.println(buffer);
    }
  }
}