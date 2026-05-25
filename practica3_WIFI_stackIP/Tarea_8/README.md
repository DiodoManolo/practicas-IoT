# Tarea 8 - Generación de fichero JSON en formato SenML

## Descripción

En esta tarea el ESP32 genera cada 10 segundos un fichero JSON con una temperatura simulada, sus unidades y una marca temporal. El formato usado está basado en SenML.

Como la placa no dispone de sensor de temperatura para esta práctica, el valor se genera de forma aleatoria dentro de un rango razonable.

---

## Objetivo

Generar periódicamente un contenido JSON en formato SenML y asociarle un nombre de fichero con marca temporal.

---

## Funcionamiento

El programa realiza los siguientes pasos:

1. Se conecta a la red WiFi.
2. Sincroniza la hora mediante NTP.
3. Genera una temperatura aleatoria.
4. Crea un JSON en formato SenML.
5. Genera un nombre de fichero del tipo `grupoXX_ddhhmmss.json`.
6. Muestra el nombre y el contenido por el monitor serie.

---

## Formato del JSON

Ejemplo de salida:

```json
[
  {
    "bn": "grupoXX/",
    "bt": 1748276587,
    "n": "temperatura",
    "u": "Cel",
    "v": 24.3
  }
]