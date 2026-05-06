#include <Arduino.h>
#include <mbed.h>
#include "pinDefinitions.h"

// -------------------- CONFIGURACIÓN GENERAL --------------------

// Pin analógico donde entra el potenciómetro
const pin_size_t ADC_PIN = A0;

// Pin de salida PWM
const pin_size_t PWM_PIN = 6;

// Resolución del ADC: 12 bits -> valores entre 0 y 4095
const int ADC_BITS = 12;
const uint16_t ADC_MAX = (1 << ADC_BITS) - 1;   // 4095 (000000000001 desplazado 12 veces a la izquierda es 1000000000000 = 4096)

// Tensión de referencia de la placa
const float VREF = 3.3f; 
// -------------------- OBJETOS DE MBED --------------------

// mbed::PwmOut es la clase de Mbed para manejar una salida PWM;
// el * indica que pwm es un puntero al objeto PWM real
mbed::PwmOut* pwm = nullptr;

// Ticker: temporizador periódico de Mbed.
// Cada vez que vence, llama a una función tipo ISR.
mbed::Ticker adcTicker;

// -------------------- VARIABLES GLOBALES --------------------

// Buffer String donde iremos guardando lo recibido por UART
String rxLine;

// Contador de ticks pendientes puestos por la ISR.
// volatile porque cambia dentro de una interrupción.
volatile uint32_t adcTicksPending = 0;

// Periodo actual del envío automático del ADC (en segundos)
uint32_t adcPeriod_s = 0;

// Duty actual del PWM (0.0 a 0.9 en este ejemplo)
float pwmDuty = 0.0f;

// -------------------- FUNCIONES AUXILIARES --------------------

// ISR del timer: NO hacer aquí analogRead ni Serial.
// Solo avisamos de que hay un tick pendiente.
void onAdcTimer()
{
  adcTicksPending++;
}

// Comprueba si un String contiene solo dígitos
// Sirve para validar argumentos como "3", "10", etc.
bool isUnsignedInteger(const String& s) //ahorrar RAM, lee el texto directamente desde su ubicación original (&)
{
  if (s.length() == 0) return false;

  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (!isDigit(s[i])) return false;
  }

  return true;
}

// Lee el ADC y lo envía por UART
void sendCurrentAdc()
{
  uint16_t adc = analogRead(ADC_PIN);              // Lectura cruda del ADC
  float vin = (adc * VREF) / ADC_MAX;              // Conversión a voltios

  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print(" | Vin=");
  Serial.print(vin, 3);
  Serial.println(" V");
}

// Detiene el envío periódico
void stopAdcStream()
{
  adcTicker.detach();   // desconecta el timer periódico

  noInterrupts();
  adcTicksPending = 0;
  interrupts();

  adcPeriod_s = 0;

  Serial.println("ADC stream OFF");
}

// Activa el envío periódico cada 'seconds' segundos
void startAdcStream(uint32_t seconds)
{
  adcTicker.detach();   // por si ya había uno activo, .detach deteien el temporizador

  noInterrupts();
  adcTicksPending = 0;
  interrupts();

  adcPeriod_s = seconds;

  // attach asocia la ISR al timer cada 'seconds' segundos
  adcTicker.attach(&onAdcTimer, (float)seconds);

  Serial.print("ADC stream ON cada ");
  Serial.print(seconds);
  Serial.println(" s");
}

// Ajusta el PWM a partir de un dígito 0..9
void setPwmFromDigit(uint8_t x)
{
  // En esta práctica interpretamos:
  // 0 -> 0%, 1 -> 10%, ... , 9 -> 90%
  pwmDuty = x / 10.0f;

  pwm->write(pwmDuty);

  Serial.print("PWM duty=");
  Serial.print(pwmDuty * 100.0f, 0);
  Serial.println(" %");
}

// Procesa un comando completo recibido por UART
void processCommand(String cmd)
{
  cmd.trim();   // quita espacios, \r, \n, etc.

  if (cmd.length() == 0) return;

  // -------- Comando ADC --------
  // Si recibimos "ADC", enviamos una sola lectura actual
  if (cmd.equalsIgnoreCase("ADC")) //Comprueba si cmd es igual a "ADC" sin importar mayúsculas o minúsculas.
  {
    sendCurrentAdc();
    return;
  }

  // -------- Comando ADC(x) --------
  // Ejemplos:
  // ADC(1) -> enviar cada 1 s
  // ADC(5) -> enviar cada 5 s
  // ADC(0) -> parar envío periódico
  if (cmd.startsWith("ADC(") && cmd.endsWith(")"))
  {
    String arg = cmd.substring(4, cmd.length() - 1); //Lee lo que va dentro del parentesis por posición
    arg.trim();

    if (!isUnsignedInteger(arg))
    {
      Serial.println("ERROR: ADC(x) requiere un entero >= 0");
      return;
    }

    uint32_t seconds = (uint32_t)arg.toInt();

    if (seconds == 0)
      stopAdcStream();
    else
      startAdcStream(seconds);

    return;
  }

  // -------- Comando PWM(x) --------
  // PWM(0) ... PWM(9)
  if (cmd.startsWith("PWM(") && cmd.endsWith(")"))
  {
    String arg = cmd.substring(4, cmd.length() - 1);
    arg.trim();

    if (!isUnsignedInteger(arg))
    {
      Serial.println("ERROR: PWM(x) requiere un entero entre 0 y 9");
      return;
    }

    int x = arg.toInt();

    if (x < 0 || x > 9)
    {
      Serial.println("ERROR: PWM(x) debe estar entre 0 y 9");
      return;
    }

    setPwmFromDigit((uint8_t)x);
    return;
  }

  // Si no entra en ningún caso, el comando no es válido
  Serial.println("ERROR: usa ADC, ADC(x) o PWM(x)");
}

// Lee la UART sin bloquear.
// Vamos guardando caracteres en un String hasta recibir fin de línea.
void readSerialCommands()
{
  while (Serial.available() > 0)
  {
    char c = (char)Serial.read();

    // Si llega fin de línea, procesamos el comando acumulado
    if (c == '\n' || c == '\r')
    {
      if (rxLine.length() > 0)
      {
        processCommand(rxLine);
        rxLine = ""; // Vacia el String
      }
    }
    else
    {
      rxLine += c;   // añadimos el carácter al String
    }
  }
}

// Atiende los ticks pendientes puestos por la ISR
void serviceAdcTicker()
{
  uint32_t pending = 0;

  // Copiamos y borramos el contador de forma segura
  noInterrupts();
  pending = adcTicksPending;
  adcTicksPending = 0;
  interrupts();

  // Por cada tick pendiente, enviamos una lectura
  while (pending > 0)
  {
    sendCurrentAdc();
    pending--;
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}

  // Reservamos memoria para el String para reducir realojos
  // y evitar fragmentación innecesaria
  rxLine.reserve(32);

  // ADC a 12 bits
  analogReadResolution(ADC_BITS);

  // Creamos el objeto PWM sobre el pin elegido
  pwm = new mbed::PwmOut(digitalPinToPinName(PWM_PIN));

  // Frecuencia PWM = 5 kHz -> periodo = 200 us
  pwm->period_us(200);

  // Duty inicial: 0%
  pwm->write(0.0f);

  Serial.println("Tarea 4 lista");
  Serial.println("Comandos:");
  Serial.println("  ADC");
  Serial.println("  ADC(x)   -> x en segundos, ADC(0) detiene el envio");
  Serial.println("  PWM(x)   -> x entre 0 y 9");
  Serial.println("Importante: en el monitor serie usa fin de linea");
}

void loop()
{
  // 1) Atender lo que llega por UART
  readSerialCommands();

  // 2) Atender los ticks del timer y enviar ADC si toca
  serviceAdcTicker();
}