#include <WiFi.h>
#include <WebServer.h>
#include "time.h"

// http://10.16.127.123
const char* ssid = "Manolo_movil";
const char* password = "12345678";

const char* ntpServer = "pool.ntp.org";
const char* tzInfo = "CET-1CEST,M3.5.0/2,M10.5.0/3";

WebServer server(80);

// Offset en segundos para modificar solo la hora mostrada
long offsetSegundos = 0;

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

void ponerHora() {
  configTime(0, 0, ntpServer);
  setenv("TZ", tzInfo, 1);
  tzset();

  Serial.print("Sincronizando hora");

  time_t now = 0;
  while (time(&now) < 100000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Hora sincronizada");
}

String leerHoraMostrada() {
  time_t now;
  time(&now);

  now = now + offsetSegundos;

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

  return String(buffer);
}

void handleRoot() {
  String hora = leerHoraMostrada();

  String pagina = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  pagina += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  pagina += "<title>Hora ESP32</title></head><body>";
  pagina += "<h1>Servidor Web ESP32</h1>";
  pagina += "<h2>Hora actual: " + hora + "</h2>";
  pagina += "<form action='/reset' method='get'>";
  pagina += "<button type='submit'>Resetear hora a 00:00</button>";
  pagina += "</form>";
  pagina += "<br><a href='/'>Actualizar pagina</a>";
  pagina += "</body></html>";

  server.send(200, "text/html", pagina);
}

void handleReset() {
  time_t now;
  time(&now);

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  int segundosActuales = timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;

  offsetSegundos = -segundosActuales;

  Serial.println("Hora reseteada a 00:00");
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  conectarWiFi();
  ponerHora();

  server.on("/", handleRoot);
  server.on("/reset", handleReset);

  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();
}