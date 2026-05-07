#include "BluetoothSerial.h"

/*
  TAREA 5 - Chat Bluetooth Classic con ESP32

  El ESP32 crea un dispositivo Bluetooth Classic.
  Desde una app de terminal Bluetooth en el móvil se puede conectar
  y enviar mensajes.

  Comunicación:
    Móvil  -> ESP32
    ESP32  -> Móvil

  Además, el Monitor Serie del Arduino IDE también puede enviar
  mensajes al móvil por Bluetooth.
*/

// Objeto que gestiona la comunicación Bluetooth Classic
BluetoothSerial SerialBT;

// Nombre que aparecerá en el móvil
const char* deviceName = "MANOLO_ESP32_BT";

void setup() {
  // Puerto serie USB con el PC
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Iniciando Bluetooth Classic...");

  // Inicia Bluetooth Classic con el nombre indicado
  if (!SerialBT.begin(deviceName)) {
    Serial.println("Error al iniciar Bluetooth");
    while (1);
  }

  Serial.print("Bluetooth iniciado. Nombre del dispositivo: ");
  Serial.println(deviceName);
  Serial.println("Conectate desde una app tipo Serial Bluetooth Terminal.");
  Serial.println("Puedes escribir desde el movil o desde el Monitor Serie.");
}

void loop() {
  // Si llega un mensaje desde el móvil por Bluetooth
  if (SerialBT.available()) {
    String mensaje = SerialBT.readStringUntil('\n');
    mensaje.trim();

    Serial.print("Movil dice: ");
    Serial.println(mensaje);

    // Respuesta automática al móvil
    SerialBT.print("ESP32 recibido: ");
    SerialBT.println(mensaje);
  }

  // Si escribes algo en el Monitor Serie del Arduino IDE
  if (Serial.available()) {
    String mensajePC = Serial.readStringUntil('\n');
    mensajePC.trim();

    Serial.print("PC dice: ");
    Serial.println(mensajePC);

    // Se manda al móvil por Bluetooth
    SerialBT.print("ESP32/PC dice: ");
    SerialBT.println(mensajePC);
  }
}