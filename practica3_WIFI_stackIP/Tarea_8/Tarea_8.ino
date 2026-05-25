#include <WiFi.h>
#include "time.h"
#include <ArduinoJson.h>

const char* ssid = "Manolo_movil";
const char* password = "12345678";

const char* ntpServer = "pool.ntp.org";
const char* tzInfo = "CET-1CEST,M3.5.0/2,M10.5.0/3";

unsigned long ultimoJson = 0;

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

String generarNombreFichero() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "grupoXX_error.json";
  }

  char nombre[25];
  strftime(nombre, sizeof(nombre), "grupoXX_%d%H%M%S.json", &timeinfo);
  return String(nombre);
}

String generarSenML() {
  time_t now;
  time(&now);

  float temperatura = random(200, 301) / 10.0;   // entre 20.0 y 30.0 ºC

  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  JsonObject obj = array.add<JsonObject>();
  obj["bn"] = "grupoXX/";
  obj["bt"] = (long)now;
  obj["n"] = "temperatura";
  obj["u"] = "Cel";
  obj["v"] = temperatura;

  String json;
  serializeJsonPretty(doc, json);
  return json;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  randomSeed(micros());

  conectarWiFi();
  ponerHora();
}

void loop() {
  if (millis() - ultimoJson >= 10000) {
    ultimoJson = millis();

    String nombreFichero = generarNombreFichero();
    String json = generarSenML();

    Serial.println("====================================");
    Serial.print("Nombre del fichero: ");
    Serial.println(nombreFichero);
    Serial.println("Contenido JSON:");
    Serial.println(json);
    Serial.println("====================================");
  }
}