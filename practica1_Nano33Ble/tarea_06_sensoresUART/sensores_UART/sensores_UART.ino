#include <Arduino.h>
#include <Arduino_LSM9DS1.h>

struct Sample {
  uint32_t t_ms;
  float ax, ay, az; // g
  float gx, gy, gz; // deg/s
  float mx, my, mz; // uT (según librería)
};

static const uint32_t SAMPLE_PERIOD_MS = 100;   // muestreo cada 100 ms
static const uint32_t SEND_PERIOD_MS   = 1000;  // envío cada 1 s
static const int      N_SAMPLES        = 10;    // 1000/100 = 10

Sample buf[N_SAMPLES];
int bufCount = 0;

uint32_t t0 = 0;
uint32_t lastSampleMs = 0;
uint32_t lastSendMs = 0;
uint32_t blockId = 0;

// Últimos valores leídos (por si en un tick no hay "available")
float lastAx=0, lastAy=0, lastAz=0;
float lastGx=0, lastGy=0, lastGz=0;
float lastMx=0, lastMy=0, lastMz=0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {}

  if (!IMU.begin()) {
    Serial.println("ERROR: No se detecta la IMU (LSM9DS1). Revisa placa/libreria.");
    while (true) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(200);
    }
  }

  t0 = millis();
  lastSampleMs = t0;
  lastSendMs = t0;

  Serial.println("OK: IMU iniciada.");
  Serial.println("Formato: bloque cada 1s con 10 muestras (cada 100ms).");
  Serial.println("Campos: t_ms;ax;ay;az;gx;gy;gz;mx;my;mz");
}

void readSensorsUpdateLast() {
  // Acelerómetro
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(lastAx, lastAy, lastAz);
  }
  // Giroscopio
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(lastGx, lastGy, lastGz);
  }
  // Magnetómetro
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(lastMx, lastMy, lastMz);
  }
}

void pushSampleToBuffer(uint32_t nowMs) {
  readSensorsUpdateLast();

  Sample s;
  s.t_ms = nowMs - t0;
  s.ax = lastAx; s.ay = lastAy; s.az = lastAz;
  s.gx = lastGx; s.gy = lastGy; s.gz = lastGz;
  s.mx = lastMx; s.my = lastMy; s.mz = lastMz;

  if (bufCount < N_SAMPLES) {
    buf[bufCount++] = s;
  }
}

void sendBufferBlock() {
  blockId++;

  // Parpadeo al enviar (1 vez por segundo)
  digitalWrite(LED_BUILTIN, HIGH);
  delay(30);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print("#BLOCK ");
  Serial.print(blockId);
  Serial.print(" count=");
  Serial.println(bufCount);

  for (int i = 0; i < bufCount; i++) {
    Serial.print(buf[i].t_ms); Serial.print(';');

    Serial.print(buf[i].ax, 6); Serial.print(';');
    Serial.print(buf[i].ay, 6); Serial.print(';');
    Serial.print(buf[i].az, 6); Serial.print(';');

    Serial.print(buf[i].gx, 6); Serial.print(';');
    Serial.print(buf[i].gy, 6); Serial.print(';');
    Serial.print(buf[i].gz, 6); Serial.print(';');

    Serial.print(buf[i].mx, 6); Serial.print(';');
    Serial.print(buf[i].my, 6); Serial.print(';');
    Serial.print(buf[i].mz, 6);

    Serial.println();
  }

  // Separador visual
  Serial.println();
  bufCount = 0;
}

void loop() {
  uint32_t now = millis();

  // Muestreo cada 100 ms
  if (now - lastSampleMs >= SAMPLE_PERIOD_MS) {
    // para que no derive si hay pequeños retrasos:
    lastSampleMs += SAMPLE_PERIOD_MS;
    pushSampleToBuffer(now);
  }

  // Envío cada 1 s
  if (now - lastSendMs >= SEND_PERIOD_MS) {
    lastSendMs += SEND_PERIOD_MS;
    sendBufferBlock();
  }
}