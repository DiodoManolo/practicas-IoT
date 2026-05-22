/* Edge Impulse Arduino - Giroscopio + LED RGB con colores distinguibles
 * Arduino Nano 33 BLE Sense
 * Modelo entrenado con gyrX, gyrY, gyrZ
 */

#include <tarea6_P6_giroscopio_inferencing.h>   // CAMBIA ESTO si tu librería tiene otro nombre
#include <Arduino_LSM9DS1.h>

/* Pines del LED RGB integrado */
#define RED 22
#define GREEN 23
#define BLUE 24

/*
 * Umbrales por clase.
 * Si una clase no se detecta bien, baja SOLO su umbral.
 * Ejemplo: si Roll_pos casi nunca se enciende, baja TH_ROLL_POS a 0.30.
 */
#define TH_PITCH_NEG 0.60
#define TH_PITCH_POS 0.60
#define TH_ROLL_NEG  0.50
#define TH_ROLL_POS  0.50
#define TH_YAW_NEG   0.87
#define TH_YAW_POS   0.60

/*
 * Orden de clases según tus capturas de Edge Impulse:
 * 0 -> Pitch_neg
 * 1 -> Pitch_pos
 * 2 -> Roll_neg
 * 3 -> Roll_pos
 * 4 -> Yaw_neg
 * 5 -> Yaw_pos
 */
#define IDX_PITCH_NEG 0
#define IDX_PITCH_POS 1
#define IDX_ROLL_NEG  2
#define IDX_ROLL_POS  3
#define IDX_YAW_NEG   4
#define IDX_YAW_POS   5

static bool debug_nn = false;

/*
 * LED RGB activo a nivel bajo:
 * analogWrite(pin, 0)   = encendido máximo
 * analogWrite(pin, 255) = apagado
 *
 * Esta función permite usar colores intermedios.
 */
void setColor(int r, int g, int b) {
    analogWrite(RED,   255 - r);
    analogWrite(GREEN, 255 - g);
    analogWrite(BLUE,  255 - b);
}

void ledOff() {
    setColor(0, 0, 0);
}

void ledRed() {
    setColor(255, 0, 0);        // rojo
}

void ledGreen() {
    setColor(0, 255, 0);        // verde
}

void ledBlue() {
    setColor(0, 0, 255);        // azul
}

void ledYellow() {
    setColor(255, 180, 0);      // amarillo/naranja
}

void ledPink() {
    setColor(255, 20, 120);     // rosa
}

void ledPurple() {
    setColor(90, 0, 255);       // morado
}

/**
 * @brief Arduino setup function
 */
void setup()
{
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);

    ledOff();

    Serial.begin(115200);
    // while (!Serial);   // Comentado para que funcione sin abrir el monitor serie

    Serial.println("Edge Impulse Inferencing Demo - Giroscopio");

    if (!IMU.begin()) {
        ei_printf("Failed to initialize IMU!\r\n");
        while (1);
    }
    else {
        ei_printf("IMU initialized\r\n");
    }

    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (gyrX, gyrY, gyrZ)\n");
        while (1);
    }
}

/**
 * @brief Arduino loop function
 */
void loop()
{
    ei_printf("\nStarting inferencing in 2 seconds...\n");
    delay(2000);

    ei_printf("Sampling...\n");

    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        while (!IMU.gyroscopeAvailable()) {
            delay(1);
        }

        /*
         * Modelo entrenado con gyrX, gyrY, gyrZ.
         * Por eso aquí usamos giroscopio, NO acelerómetro.
         */
        IMU.readGyroscope(buffer[ix], buffer[ix + 1], buffer[ix + 2]);

        int32_t wait_time = next_tick - micros();
        if (wait_time > 0) {
            delayMicroseconds(wait_time);
        }
    }

    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    if (err != 0) {
        ei_printf("Failed to create signal from buffer (%d)\n", err);
        return;
    }

    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);

    if (r != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    /* Mostrar resultados */
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(":\n");

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label,
                  result.classification[ix].value);
    }

    /*
     * Bloque de LEDs estilo Práctica 5, pero con PWM para distinguir rosa/morado.
     *
     * Si una clase no se detecta:
     * - baja su umbral, por ejemplo de 0.40 a 0.30.
     *
     * Si se enciende demasiado fácil:
     * - sube su umbral, por ejemplo de 0.40 a 0.50.
     */

    if (result.classification[IDX_ROLL_POS].value > TH_ROLL_POS) {
        // Roll positivo -> ROJO
        ledRed();

    } else if (result.classification[IDX_ROLL_NEG].value > TH_ROLL_NEG) {
        // Roll negativo -> ROSA
        ledPink();

    } else if (result.classification[IDX_PITCH_POS].value > TH_PITCH_POS) {
        // Pitch positivo -> VERDE
        ledGreen();

    } else if (result.classification[IDX_PITCH_NEG].value > TH_PITCH_NEG) {
        // Pitch negativo -> AMARILLO
        ledYellow();

    } else if (result.classification[IDX_YAW_POS].value > TH_YAW_POS) {
        // Yaw positivo -> AZUL
        ledBlue();

    } else if (result.classification[IDX_YAW_NEG].value > TH_YAW_NEG) {
        // Yaw negativo -> MORADO
        ledPurple();

    } else {
        // Ninguna clase supera el umbral
        ledOff();
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
}

/**
 * @brief Printf function for Arduino serial
 */
void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.print(print_buf);
    }
}

// #if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
// #error "Invalid model for current sensor"
// #endif