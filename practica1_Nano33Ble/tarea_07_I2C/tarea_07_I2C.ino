#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM9DS1.h>

// -------------------- CONFIGURACIÓN --------------------

// Dirección I2C de la placa esclava
const uint8_t SLAVE_ADDR = 0x08;

// Muestreo: cada 200 ms durante 1 segundo -> 5 muestras
const unsigned long SAMPLE_PERIOD_MS = 200;
const unsigned long WINDOW_MS = 1000;
const uint8_t NUM_SAMPLES = WINDOW_MS / SAMPLE_PERIOD_MS;  // 5

// Estructura del paquete que se enviará por I2C.
// __attribute__((packed)) evita rellenos extra en memoria.
struct __attribute__((packed)) SamplePacket
{
  uint8_t sampleIndex;   // índice de muestra: 0..4
  uint8_t valid;         // 1 = muestra válida, 0 = error al leer

  // Acelerómetro en mg (g * 1000)
  int16_t ax_mg;
  int16_t ay_mg;
  int16_t az_mg;

  // Giroscopio en décimas de dps (deg/s * 10)
  int16_t gx_dps10;
  int16_t gy_dps10;
  int16_t gz_dps10;

  // Magnetómetro en décimas de uT (uT * 10)
  int16_t mx_uT10;
  int16_t my_uT10;
  int16_t mz_uT10;
};

// Array donde guardamos las 5 muestras antes de enviarlas
SamplePacket samples[NUM_SAMPLES];

// -------------------- FUNCIONES AUXILIARES --------------------

// Espera a que haya una muestra nueva del acelerómetro
bool waitAccel(unsigned long timeoutMs)
{
  unsigned long t0 = millis();
  while (!IMU.accelerationAvailable())
  {
    if (millis() - t0 >= timeoutMs) return false;
  }
  return true;
}

// Espera a que haya una muestra nueva del giroscopio
bool waitGyro(unsigned long timeoutMs)
{
  unsigned long t0 = millis();
  while (!IMU.gyroscopeAvailable())
  {
    if (millis() - t0 >= timeoutMs) return false;
  }
  return true;
}

// Espera a que haya una muestra nueva del magnetómetro
bool waitMag(unsigned long timeoutMs)
{
  unsigned long t0 = millis();
  while (!IMU.magneticFieldAvailable())
  {
    if (millis() - t0 >= timeoutMs) return false;
  }
  return true;
}

// Captura una muestra completa de los 3 sensores y la guarda en pkt
void captureSample(SamplePacket &pkt, uint8_t idx)
{
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;

  pkt.sampleIndex = idx;
  pkt.valid = 0;   // por defecto, inválida hasta que todo salga bien

  // Esperamos datos frescos de cada sensor
  if (!waitAccel(100)) return;
  IMU.readAcceleration(ax, ay, az);

  if (!waitGyro(100)) return;
  IMU.readGyroscope(gx, gy, gz);

  if (!waitMag(100)) return;
  IMU.readMagneticField(mx, my, mz);

  // Escalamos los floats para mandarlos como enteros por I2C
  pkt.ax_mg = (int16_t)(ax * 1000.0f);
  pkt.ay_mg = (int16_t)(ay * 1000.0f);
  pkt.az_mg = (int16_t)(az * 1000.0f);

  pkt.gx_dps10 = (int16_t)(gx * 10.0f);
  pkt.gy_dps10 = (int16_t)(gy * 10.0f);
  pkt.gz_dps10 = (int16_t)(gz * 10.0f);

  pkt.mx_uT10 = (int16_t)(mx * 10.0f);
  pkt.my_uT10 = (int16_t)(my * 10.0f);
  pkt.mz_uT10 = (int16_t)(mz * 10.0f);

  pkt.valid = 1;
}

// Muestra por serie una muestra localmente en la placa 1
void printLocalSample(const SamplePacket &pkt)
{
  Serial.print("Muestra ");
  Serial.print(pkt.sampleIndex + 1);
  Serial.print("/");
  Serial.print(NUM_SAMPLES);

  if (!pkt.valid)
  {
    Serial.println(" -> ERROR de lectura");
    return;
  }

  Serial.print(" | Acc[g]=(");
  Serial.print(pkt.ax_mg / 1000.0f, 3);
  Serial.print(", ");
  Serial.print(pkt.ay_mg / 1000.0f, 3);
  Serial.print(", ");
  Serial.print(pkt.az_mg / 1000.0f, 3);
  Serial.print(")");

  Serial.print(" | Gyro[dps]=(");
  Serial.print(pkt.gx_dps10 / 10.0f, 1);
  Serial.print(", ");
  Serial.print(pkt.gy_dps10 / 10.0f, 1);
  Serial.print(", ");
  Serial.print(pkt.gz_dps10 / 10.0f, 1);
  Serial.print(")");

  Serial.print(" | Mag[uT]=(");
  Serial.print(pkt.mx_uT10 / 10.0f, 1);
  Serial.print(", ");
  Serial.print(pkt.my_uT10 / 10.0f, 1);
  Serial.print(", ");
  Serial.print(pkt.mz_uT10 / 10.0f, 1);
  Serial.println(")");
}

// Envía un paquete por I2C a la placa 2
void sendPacketI2C(const SamplePacket &pkt)
{
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write((const uint8_t*)&pkt, sizeof(SamplePacket)); //
  uint8_t err = Wire.endTransmission();

  if (err == 0)
  {
    Serial.print("Paquete enviado: muestra ");
    Serial.println(pkt.sampleIndex + 1);
  }
  else
  {
    Serial.print("Error I2C al enviar paquete. Codigo=");
    Serial.println(err);
  }
}

// Captura 5 muestras en 1 segundo
void captureBurst()
{
  Serial.println("Capturando durante 1 segundo...");

  unsigned long tStart = millis();

  for (uint8_t i = 0; i < NUM_SAMPLES; i++)
  {
    // Queremos tomar la muestra en: 200, 400, 600, 800 y 1000 ms
    unsigned long target = tStart + (unsigned long)(i + 1) * SAMPLE_PERIOD_MS;

    while ((long)(millis() - target) < 0)
    {
      // Espera activa hasta que llegue el instante de muestreo
    }

    captureSample(samples[i], i);
    printLocalSample(samples[i]);
  }

  Serial.println("Captura terminada");
}

// Envía las 5 muestras a la placa 2
void sendBurst()
{
  Serial.println("Enviando datos por I2C...");

  for (uint8_t i = 0; i < NUM_SAMPLES; i++)
  {
    sendPacketI2C(samples[i]);
    delay(50);  // pequeña separación entre paquetes
  }

  Serial.println("Envio terminado");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}

  Wire.begin();          // placa 1 actúa como maestra I2C
  Wire.setClock(100000); // 100 kHz para que sea fácil de observar en osciloscopio

  if (!IMU.begin())
  {
    Serial.println("ERROR: no se pudo iniciar la IMU");
    while (1) {}
  }

  Serial.println("PLACA 1 lista");
  Serial.println("Escribe START y pulsa Enter para capturar 1 segundo y enviar por I2C");
}

void loop()
{
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "START")
    {
      captureBurst();  // toma las 5 muestras
      sendBurst();     // las manda por I2C
    }
    else
    {
      Serial.println("Comando no valido. Usa START");
    }
  }
}




// PLACA ESCALAVA
// #include <Arduino.h>
// #include <Wire.h>

// // -------------------- CONFIGURACIÓN --------------------

// // Dirección I2C de esta placa esclava
// const uint8_t I2C_ADDR = 0x08;

// // LED externo en D6
// const int LED_PIN = 6;

// // Mismo paquete que en la placa 1
// struct __attribute__((packed)) SamplePacket
// {
//   uint8_t sampleIndex;
//   uint8_t valid;

//   int16_t ax_mg;
//   int16_t ay_mg;
//   int16_t az_mg;

//   int16_t gx_dps10;
//   int16_t gy_dps10;
//   int16_t gz_dps10;

//   int16_t mx_uT10;
//   int16_t my_uT10;
//   int16_t mz_uT10;
// };

// // Cola simple para no perder paquetes si llegan muy seguidos
// const uint8_t QUEUE_SIZE = 8;
// SamplePacket rxQueue[QUEUE_SIZE];

// volatile uint8_t qHead = 0;
// volatile uint8_t qTail = 0;
// volatile uint8_t qCount = 0;

// // Control del LED
// bool ledOn = false;
// unsigned long ledOffMs = 0;

// // -------------------- RECEPCIÓN I2C --------------------

// // Esta función se ejecuta al recibir un paquete I2C
// void receiveEvent(int howMany)
// {
//   SamplePacket temp;
//   uint8_t *p = (uint8_t*)&temp; //coge la dirección de temp y trátala como si fuera un bloque de bytes; guarda esa dirección en p pq temp es una estructura utilizo de memoria para reconstruir el paquete recibido por I2C
//   uint8_t i = 0;

//   // Leemos exactamente los bytes del paquete
//   while (Wire.available() > 0 && i < sizeof(SamplePacket))
//   {
//     p[i++] = Wire.read();
//   }

//   // Si sobra algo, lo vaciamos
//   while (Wire.available() > 0)
//   {
//     Wire.read();
//   }

//   // Solo guardamos el paquete si ha llegado completo y hay hueco
//   if (i == sizeof(SamplePacket) && qCount < QUEUE_SIZE)
//   {
//     rxQueue[qHead] = temp;
//     qHead = (qHead + 1) % QUEUE_SIZE;
//     qCount++;
//   }
// }

// // Muestra por serie el contenido de un paquete
// void printPacket(const SamplePacket &pkt)
// {
//   Serial.print("Muestra ");
//   Serial.print(pkt.sampleIndex + 1);
//   Serial.print(": ");

//   if (!pkt.valid)
//   {
//     Serial.println("ERROR de lectura en la placa 1");
//     return;
//   }

//   Serial.print("Acc[g]=(");
//   Serial.print(pkt.ax_mg / 1000.0f, 3);
//   Serial.print(", ");
//   Serial.print(pkt.ay_mg / 1000.0f, 3);
//   Serial.print(", ");
//   Serial.print(pkt.az_mg / 1000.0f, 3);
//   Serial.print(")");

//   Serial.print(" | Gyro[dps]=(");
//   Serial.print(pkt.gx_dps10 / 10.0f, 1);
//   Serial.print(", ");
//   Serial.print(pkt.gy_dps10 / 10.0f, 1);
//   Serial.print(", ");
//   Serial.print(pkt.gz_dps10 / 10.0f, 1);
//   Serial.print(")");

//   Serial.print(" | Mag[uT]=(");
//   Serial.print(pkt.mx_uT10 / 10.0f, 1);
//   Serial.print(", ");
//   Serial.print(pkt.my_uT10 / 10.0f, 1);
//   Serial.print(", ");
//   Serial.print(pkt.mz_uT10 / 10.0f, 1);
//   Serial.println(")");
// }

// // Procesa los paquetes pendientes fuera del callback I2C
// void serviceRxQueue()
// {
//   while (qCount > 0)
//   {
//     SamplePacket pkt;

//     noInterrupts();
//     pkt = rxQueue[qTail];
//     qTail = (qTail + 1) % QUEUE_SIZE;
//     qCount--;
//     interrupts();

//     // Mostrar por pantalla
//     printPacket(pkt);

//     // Encender LED y mantenerlo 1 segundo
//     digitalWrite(LED_PIN, HIGH);
//     ledOn = true;
//     ledOffMs = millis() + 1000;
//   }
// }

// // Apaga el LED cuando pasa 1 segundo
// void serviceLedTimer()
// {
//   if (ledOn && (long)(millis() - ledOffMs) >= 0)
//   {
//     digitalWrite(LED_PIN, LOW);
//     ledOn = false;
//   }
// }

// void setup()
// {
//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, LOW);

//   Serial.begin(115200);
//   while (!Serial) {}

//   Wire.begin(I2C_ADDR);         // placa 2 actúa como esclava
//   Wire.onReceive(receiveEvent); // función que se ejecuta al recibir I2C

//   Serial.println("PLACA 2 lista");
//   Serial.println("Esperando datos por I2C...");
// }

// void loop()
// {
//   serviceRxQueue();
//   serviceLedTimer();
// }