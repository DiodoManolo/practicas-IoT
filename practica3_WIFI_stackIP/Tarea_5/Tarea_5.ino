#include <WiFi.h>
#include "time.h"

const char* ssid = "Manolo_movil";
const char* password = "12345678";

const char* serverIP = "10.16.127.159";   // IP del PC
const int serverPort = 5000;

const char* ntpServer = "pool.ntp.org";
const char* tzInfo = "CET-1CEST,M3.5.0/2,M10.5.0/3";

WiFiClient client;

bool enviarHora = false;
unsigned long ultimoEnvio = 0; //Guarda el instante del último envío.

void conectarWiFi() {
  Serial.print("Conectando a WiFi");

  WiFi.begin(ssid, password); //Hace que el ESP 32 se conecte a la WIFI utilizando ese nombre y contraseña

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
  setenv("TZ", tzInfo, 1); //Le dice al sistema qué zona horaria usar.
  tzset();

  Serial.print("Sincronizando hora");

  time_t now = 0; //variable para guardar el tiempo actual.
  while (time(&now) < 100000) { //Espera un poco para recibir hora valida del NTP
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Hora sincronizada");
}

String leerHora() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "Hora no disponible";
  }

  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo); //Esto convierte la hora a texto.
  return String(buffer);
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

  conectarWiFi();
  ponerHora();
  conectarServidor();
}

void loop() {
  if (!client.connected()) {//Dice si el cliente TCP sigue conectado.
    Serial.println("Conexion perdida");
    client.stop();
    conectarServidor();
  }

  // Leer comandos que lleguen del servidor
  if (client.available()) {
    String comando = client.readStringUntil('\n');
    comando.trim(); //limpia espacios y saltos de línea al principio y al final.

    Serial.print("Comando recibido: ");
    Serial.println(comando);

    if (comando == "start") {
      enviarHora = true;
    }

    if (comando == "stop") {
      enviarHora = false;
    }
  }

  // Enviar hora cada segundo si está activado
  if (enviarHora && millis() - ultimoEnvio >= 1000) { //si enviarHora es true y ha pasado un segundo desde el anterior envio...
    ultimoEnvio = millis();

    String hora = leerHora();
    client.println(hora);

    Serial.print("Hora enviada: ");
    Serial.println(hora);
  }
}