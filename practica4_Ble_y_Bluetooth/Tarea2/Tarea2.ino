#include <ArduinoBLE.h>

/*
  TAREA 2 - BLE GATT para controlar el LED RGB

  El Arduino Nano 33 BLE actúa como periférico BLE.
  Crea un servicio GATT personalizado y una característica
  en la que el móvil puede escribir un comando.

  Desde nRF Connect se escribe:
    R   -> LED rojo
    G   -> LED verde
    B   -> LED azul
    W   -> LED blanco
    OFF -> LED apagado

  Importante:
  En el Arduino Nano 33 BLE, el LED RGB integrado es activo a nivel bajo.
  Es decir:
    LOW  -> enciende color
    HIGH -> apaga color
*/

// Nombre visible en nRF Connect
const char* deviceName = "MANOLO_LED";

// Servicio BLE personalizado
BLEService ledService("4D414E4F-4C4F-1000-8000-00805F9B34FB");

// Característica BLE para controlar el LED
// BLERead  -> permite leer el último comando enviado
// BLEWrite -> permite escribir desde nRF Connect
BLEStringCharacteristic ledCharacteristic(
  "4D414E4F-4C4F-2000-8000-00805F9B34FB",
  BLERead | BLEWrite,
  10
);

// Variable para guardar el último comando recibido
String lastCommand = "OFF";

void setup() {
  Serial.begin(9600);
  delay(1500);

  Serial.println("Iniciando BLE...");

  // Configuración de los pines del LED RGB integrado
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // Empezamos con el LED apagado
  apagarLED();

  // Inicializa el módulo BLE
  if (!BLE.begin()) {
    Serial.println("Error al iniciar BLE");
    while (1);
  }

  // Configura el nombre visible del dispositivo BLE
  BLE.setLocalName(deviceName);
  BLE.setDeviceName(deviceName);

  // Añade la característica al servicio
  ledService.addCharacteristic(ledCharacteristic);

  // Añade el servicio al GATT del dispositivo
  BLE.addService(ledService);

  // Valor inicial de la característica
  ledCharacteristic.writeValue(lastCommand);

  // Anuncia el servicio para que aparezca en nRF Connect
  BLE.setAdvertisedService(ledService);

  // Comienza el advertising BLE
  BLE.advertise();

  Serial.println("Advertising iniciado");
  Serial.print("Nombre BLE: ");
  Serial.println(deviceName);
  Serial.println("Esperando conexion desde nRF Connect...");
}

void loop() {
  // Espera a que un dispositivo central, por ejemplo el móvil, se conecte
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Conectado a: ");
    Serial.println(central.address());

    // Mientras el móvil esté conectado
    while (central.connected()) {
      BLE.poll();

      // Si el móvil ha escrito algo en la característica
      if (ledCharacteristic.written()) {
        String command = ledCharacteristic.value();

        // Pasamos el comando a mayúsculas para aceptar r, g, b, off, etc.
        command.toUpperCase();

        Serial.print("Comando recibido: ");
        Serial.println(command);

        // Ejecutamos el comando recibido
        if (command == "R") {
          encenderRojo();
          lastCommand = "R";
        }
        else if (command == "G") {
          encenderVerde();
          lastCommand = "G";
        }
        else if (command == "B") {
          encenderAzul();
          lastCommand = "B";
        }
        else if (command == "W") {
          encenderBlanco();
          lastCommand = "W";
        }
        else if (command == "OFF") {
          apagarLED();
          lastCommand = "OFF";
        }
        else {
          Serial.println("Comando no reconocido");
        }

        // Actualiza el valor de la característica para que se pueda leer
        ledCharacteristic.writeValue(lastCommand);
      }
    }

    Serial.println("Dispositivo desconectado");
  }
}

// ----------------------
// Funciones del LED RGB
// ----------------------

void apagarLED() {
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}

void encenderRojo() {
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}

void encenderVerde() {
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);
}

void encenderAzul() {
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);
}

void encenderBlanco() {
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
}