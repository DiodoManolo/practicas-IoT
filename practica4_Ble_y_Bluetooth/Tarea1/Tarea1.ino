#include <ArduinoBLE.h>

// Nombre que aparecerá en nRF Connect
const char* deviceName = "Manolo_BLE";

// UUID personalizada.
// Los primeros bytes son "MANOLO" en ASCII hexadecimal:
// M  = 4D
// A  = 41
// N  = 4E
// O  = 4F
// L  = 4C
// O  = 4F
//
// UUID completa:
// 4D414E4F-4C4F-1000-8000-00805F9B34FB
BLEService manoloService("4D414E4F-4C4F-1000-8000-00805F9B34FB");

// Característica dummy solo para que el servicio exista al conectarse.
// En esta tarea no hace falta usarla.
BLEByteCharacteristic dummyCharacteristic(
  "4D414E4F-4C4F-2000-8000-00805F9B34FB",
  BLERead
);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Iniciando BLE...");

  if (!BLE.begin()) {
    Serial.println("Error al iniciar BLE");
    while (1);
  }

  // Nombre visible del dispositivo
  BLE.setLocalName(deviceName);
  BLE.setDeviceName(deviceName);

  // Añadimos una característica dummy al servicio
  manoloService.addCharacteristic(dummyCharacteristic);
  BLE.addService(manoloService);

  dummyCharacteristic.writeValue(1);

  // Este es el servicio que se anuncia en el advertising
  BLE.setAdvertisedService(manoloService);

  // Empieza el advertising
  BLE.advertise();

  Serial.println("Advertising iniciado");
  Serial.print("Nombre BLE: ");
  Serial.println(deviceName);

  Serial.println("UUID anunciada:");
  Serial.println("4D414E4F-4C4F-1000-8000-00805F9B34FB");
  Serial.println("Los primeros bytes de la UUID codifican MANOLO en ASCII.");
}

void loop() {
  // Mantiene activa la pila BLE
  BLE.poll();

  // Opcional: detectar si alguien se conecta
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Conectado a central: ");
    Serial.println(central.address());

    while (central.connected()) {
      BLE.poll();
    }

    Serial.println("Central desconectada");
  }
}