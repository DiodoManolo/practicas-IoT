#include <WiFi.h>

const char* ssid = "Manolo_movil";
const char* password = "12345678";

const char* serverIP = "10.16.127.159";   // IP del PC
const int serverPort = 5000;

WiFiClient client;
unsigned long ultimoEnvio = 0;

void conectarWiFi() {
  Serial.print("Conectando a WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectada");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
}

void conectarServidor() {
  Serial.print("Conectando al servidor ");

  while (!client.connect(serverIP, serverPort)) {
    Serial.println("fallo, reintentando...");
    delay(2000);
  }

  Serial.println("conectado");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  randomSeed(micros());   // semilla para números aleatorios

  conectarWiFi();
  conectarServidor();
}

void loop() {
  if (!client.connected()) {
    Serial.println("Conexion perdida");
    client.stop();
    conectarServidor();
  }

  // Enviar datos cada 100 ms
  if (millis() - ultimoEnvio >= 100) {
    ultimoEnvio = millis();

    // Datos simulados de acelerómetro
    float ax = random(-200, 201) / 100.0;   // entre -2.00 y 2.00
    float ay = random(-200, 201) / 100.0;
    float az = random(800, 1201) / 100.0;   // entre 8.00 y 12.00 para parecer gravedad

    // Enviar en formato: ax;ay;az
    String datos = String(ax, 2) + ";" + String(ay, 2) + ";" + String(az, 2);
    client.println(datos);

    Serial.print("Datos enviados: ");
    Serial.println(datos);
  }
}