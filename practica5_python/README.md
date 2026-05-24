# Práctica 5 — Comunicación, gestión y representación de datos de sensores con Python

## Objetivo
El objetivo de esta práctica es familiarizarse con el entorno de Python y realizar un flujo completo de adquisición y tratamiento de datos de sensores:
- Leer datos enviados por **UART/USB (puerto serie)** desde una placa (p. ej. Arduino Nano 33 BLE Sense).
- Visualizar los datos en tiempo real en consola (equivalente a un terminal tipo Termite).
- Guardar los datos en un fichero **.txt tipo CSV** con separador `;` para abrirlo en **Excel**.
- Calcular estadísticos cada cierto tiempo (media y desviación típica) y **representar gráficamente** en tiempo real con `matplotlib`.

---

## Requisitos

### Hardware
- Placa con sensores enviando datos por Serial (USB).
- Cable USB.

### Software
- PyCharm (o cualquier IDE).
- Python 3.x con un intérprete configurado en el proyecto.
- Librerías:
  - `pyserial`
  - `matplotlib`

Instalación:
```bash
pip install pyserial matplotlib