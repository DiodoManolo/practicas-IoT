#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

/*
  TAREA 3 - Acelerómetro por BLE con lectura y notificaciones

  El Arduino Nano 33 BLE actúa como periférico BLE.
  Crea un servicio GATT personalizado y una característica
  que contiene los valores del acelerómetro.

  La característica tiene SOLO:
    BLERead   -> el móvil puede leer el valor actual
    BLENotify -> el móvil puede recibir actualizaciones automáticas

  No tiene BLEWrite, por tanto desde nRF Connect no se puede escribir
  en esta característica.
*/

// Nombre que aparece en nRF Connect
const char* deviceName = "MANOLO_IMU";

// Servicio BLE personalizado
BLEService imuService("4D414E4F-4C4F-3000-8000-00805F9B34FB");

// Característica para enviar aceleración
// Permisos: lectura + notificaciones
// Tamaño máximo: 40 caracteres
BLEStringCharacteristic accelCharacteristic(
  "4D414E4F-4C4F-3001-8000-00805F9B34FB",
  BLERead | BLENotify,
  40
);

// Tiempo entre actualizaciones del acelerómetro
const unsigned long samplePeriodMs = 500;
unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(9600);
  delay(1500);

  Serial.println("Iniciando BLE + IMU...");

  // Inicializa el acelerómetro
  if (!IMU.begin()) {
    Serial.println("Error: no se pudo iniciar la IMU");
    while (1);
  }

  Serial.println("IMU iniciada correctamente");

  // Inicializa BLE
  if (!BLE.begin()) {
    Serial.println("Error: no se pudo iniciar BLE");
    while (1);
  }

  // Nombre visible del dispositivo BLE
  BLE.setLocalName(deviceName);
  BLE.setDeviceName(deviceName);

  // Añadimos la característica al servicio
  imuService.addCharacteristic(accelCharacteristic);

  // Añadimos el servicio al GATT
  BLE.addService(imuService);

  // Valor inicial de la característica
  accelCharacteristic.writeValue("0.000;0.000;0.000");

  // Anunciamos el servicio BLE
  BLE.setAdvertisedService(imuService);

  // Empieza advertising
  BLE.advertise();

  Serial.println("Advertising iniciado");
  Serial.print("Nombre BLE: ");
  Serial.println(deviceName);
  Serial.println("Esperando conexion desde nRF Connect...");
}

void loop() {
  // Mantiene funcionando la pila BLE
  BLE.poll();

  // Actualiza el acelerómetro cada samplePeriodMs
  if (millis() - lastSampleTime >= samplePeriodMs) {
    lastSampleTime = millis();

    float ax, ay, az;

    // Si hay una nueva muestra disponible, la leemos
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(ax, ay, az);

      // Creamos un texto tipo: "0.012;-0.034;0.987"
      char buffer[40];
      snprintf(buffer, sizeof(buffer), "%.3f;%.3f;%.3f", ax, ay, az);

      // Actualizamos la característica BLE.
      // Si el móvil ha activado Notify, le llegará automáticamente.
      accelCharacteristic.writeValue(buffer);

      // También lo mostramos por el monitor serie para depurar
      Serial.print("Acelerometro: ");
      Serial.println(buffer);
    }
  }
}