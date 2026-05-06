#include <Arduino.h>
#include <Wire.h>

// Dirección I2C de la placa esclava
const uint8_t SLAVE_ADDR = 0x08;

// Envía un comando de 1 byte a la esclava:
// 1 = encender LED
// 0 = apagar LED
void sendLedCommand(uint8_t value)
{
  Wire.beginTransmission(SLAVE_ADDR); // empieza la transmisión hacia la esclava
  Wire.write(value);                  // envía 1 byte
  uint8_t error = Wire.endTransmission(); // termina la transmisión

  if (error == 0)
  {
    Serial.print("Comando enviado correctamente: ");
    Serial.println(value);
  }
  else
  {
    Serial.print("Error I2C al enviar. Codigo: ");
    Serial.println(error);
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}

  // Inicia I2C como maestro
  Wire.begin();

  Serial.println("Placa MAESTRA I2C lista");
  Serial.println("Escribe ON o OFF en el monitor serie");
}

void loop()
{
  // Si llega algo por el monitor serie, lo leemos como texto
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n'); // lee hasta pulsar Enter
    cmd.trim();                                // quita espacios y salto de línea
    cmd.toUpperCase();                         // convierte a mayúsculas

    if (cmd == "ON")
    {
      sendLedCommand(1);
      Serial.println("Orden: encender LED");
    }
    else if (cmd == "OFF")
    {
      sendLedCommand(0);
      Serial.println("Orden: apagar LED");
    }
    else
    {
      Serial.println("Comando no valido. Usa ON o OFF");
    }
  }
}



#include <Arduino.h>
#include <Wire.h>

// Dirección I2C de esta placa esclava
const uint8_t I2C_ADDR = 0x08;

// Pin digital que controla el LED externo
const int LED_PIN = 6;

// Variables compartidas entre la recepción I2C y el loop()
// volatile porque pueden cambiar fuera del flujo normal del programa
volatile bool newCommand = false;
volatile uint8_t receivedValue = 0;

// Esta función se ejecuta cuando la esclava recibe datos por I2C
void receiveEvent(int howMany)
{
  // Leemos todos los bytes recibidos.
  // En esta práctica solo esperamos 1, pero así queda más robusto.
  while (Wire.available() > 0)
  {
    receivedValue = Wire.read();
    newCommand = true;
  }
}

// void setup()
// {
//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, LOW); // LED inicialmente apagado

//   Serial.begin(115200);
//   while (!Serial) {}

//   // Inicia I2C como esclava con dirección 0x08
//   Wire.begin(I2C_ADDR);

//   // Asocia la función que se ejecutará cuando lleguen datos
//   Wire.onReceive(receiveEvent);

//   Serial.println("Placa ESCLAVA I2C lista");
//   Serial.println("Esperando comandos desde la placa maestra...");
// }

// void loop()
// {
//   // Si ha llegado un comando nuevo, lo procesamos fuera del callback I2C
//   if (newCommand)
//   {
//     noInterrupts();
//     uint8_t cmd = receivedValue;
//     newCommand = false;
//     interrupts();

//     if (cmd == 1)
//     {
//       digitalWrite(LED_PIN, HIGH);
//       Serial.println("LED ENCENDIDO");
//     }
//     else if (cmd == 0)
//     {
//       digitalWrite(LED_PIN, LOW);
//       Serial.println("LED APAGADO");
//     }
//     else
//     {
//       Serial.print("Comando desconocido recibido: ");
//       Serial.println(cmd);
//     }
//   }
// }