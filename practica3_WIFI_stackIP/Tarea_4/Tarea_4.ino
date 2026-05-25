#include <WiFi.h>
#include "time.h"

const char* ssid = "Manolo_movil";
const char* password = "12345678";

// IP del PC donde está SocketTest como servidor
const char* serverIP = "10.16.127.159";
const uint16_t serverPort = 5000;

// Servidores NTP
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

// Zona horaria España peninsular
const char* tzInfo = "CET-1CEST,M3.5.0/2,M10.5.0/3";

WiFiClient client;
unsigned long lastSend = 0;

void connectWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.mode(WIFI_STA);
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

void syncTime() {
  configTime(0, 0, ntpServer1, ntpServer2);
  setenv("TZ", tzInfo, 1);
  tzset();

  Serial.print("Sincronizando hora con NTP");
  time_t now = 0;
  while (time(&now) < 1700000000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Hora sincronizada");
}

String getTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Hora no disponible";
  }

  char buffer[16];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  return String(buffer);
}

void connectServer() {
  Serial.print("Conectando al servidor TCP ");
  Serial.print(serverIP);
  Serial.print(":");
  Serial.println(serverPort);

  while (!client.connect(serverIP, serverPort)) {
    Serial.println("No se pudo conectar. Reintentando en 2 s...");
    delay(2000);
  }

  Serial.println("Conectado al servidor TCP");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  syncTime();
  connectServer();
}

void loop() {
  if (!client.connected()) {
    Serial.println("Conexion perdida con el servidor");
    client.stop();
    connectServer();
  }

  if (millis() - lastSend >= 1000) {
    lastSend = millis();

    String hora = getTimeString();
    client.println(hora);   // manda la hora al servidor
    Serial.print("Hora enviada: ");
    Serial.println(hora);
  }
}