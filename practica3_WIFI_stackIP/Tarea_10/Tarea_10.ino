#include <WiFi.h>
#include "AdafruitIO_WiFi.h"

#define IO_USERNAME  "ManoloGomez"
#define IO_KEY       " IO_KEY "

const char* ssid = "Manolo_movil";
const char* password = "12345678";

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, ssid, password);

// Feed de Adafruit IO
AdafruitIO_Feed *temperatura = io.feed("temperatura");

unsigned long ultimoEnvio = 0;

// Esta funcion se ejecuta cuando llega un valor nuevo al feed
void mensajeRecibido(AdafruitIO_Data *data) {
  Serial.print("Valor recibido desde Adafruit IO: ");
  Serial.println(data->value());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  randomSeed(micros());

  // Asociar la funcion callback al feed
  temperatura->onMessage(mensajeRecibido);

  Serial.print("Conectando a Adafruit IO");
  io.connect();

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Adafruit IO conectado");

  // Pedir el ultimo valor del feed al conectar
  temperatura->get();
}

void loop() {
  // Mantiene viva la conexion MQTT
  io.run();

  // Publicar cada 5 segundos
  if (millis() - ultimoEnvio >= 5000) {
    ultimoEnvio = millis();

    float temp = random(200, 301) / 10.0;   // 20.0 a 30.0
    Serial.print("Publicando temperatura: ");
    Serial.println(temp);

    temperatura->save(temp);
  }
}