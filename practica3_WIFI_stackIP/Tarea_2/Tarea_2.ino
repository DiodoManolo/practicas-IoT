#include <WiFi.h>
#include "time.h"

const char* ssid = "Manolo_movil";
const char* password = "12345678";

// Servidores NTP
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

// Zona horaria España peninsular con cambio automático verano/invierno
const char* tzInfo = "CET-1CEST,M3.5.0/2,M10.5.0/3";

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
  // Inicia NTP con dos servidores
  configTime(0, 0, ntpServer1, ntpServer2);

  // Aplica la zona horaria local
  setenv("TZ", tzInfo, 1);
  tzset();

  Serial.println("Sincronizando hora con NTP...");

  time_t now = 0;
  while (time(&now) < 1700000000) {   // espera hasta recibir una hora válida
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Hora sincronizada");
}

void printLocalTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("No se pudo obtener la hora");
    return;
  }

  Serial.println(&timeinfo, "%d/%m/%Y %H:%M:%S");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  syncTime();
}

void loop() {
  printLocalTime();
  delay(1000);
}