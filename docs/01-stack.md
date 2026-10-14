# 01 · Stack tecnológico

**Estado:** cerrado para el Prototipo 1 (maqueta, 8 puestos), con una única excepción
pendiente: el modelo de sensor (§3).

---

## 1. Contexto del proyecto

- Prototipo funcional a pequeña escala (maqueta) de un HMI de parqueadero.
- 8 puestos, cada uno con un sensor propio, gestionados por **un único ESP32**.
- App de escritorio **multiplataforma** que visualiza la ocupación en tiempo real.
- Idioma del equipo: español → lenguajes de script en Python, firmware en C++.

## 2. Hardware

| Componente | Decisión | Alternativas descartadas | Motivo |
|---|---|---|---|
| Controlador | **1× ESP32 DevKit** | Arduino Uno/Nano (sin WiFi), Raspberry Pi (costo, overkill para 8 puestos), PLC (costo) | WiFi nativo para MQTT; GPIO suficiente para 8 sensores en una sola placa; costo bajo |
| Alimentación | USB 5 V | Fuente externa dedicada | Suficiente para placa + 8 sensores |
| Comunicación | **WiFi + MQTT** | Serial/USB (cable físico hacia la PC, acopla app y hardware), TCP/IP propio (protocolo a medida) | Broker **Mosquitto** local; hardware desacoplado de la app; permite simulador remoto y varias placas después |

### Puntos críticos del hardware (aplican a cualquier sensor elegido)

- Los GPIO del ESP32 **no toleran 5 V**: si el sensor entrega ECHO a 5 V se necesita divisor
  de voltaje (1 kΩ / 2 kΩ). Omisión que rompe prototipos.
- Lectura **secuencial** de ultrasonidos para evitar *crosstalk* entre puestos vecinos.
- GND común entre placa y sensores; alimentación estable (los ultrasonidos consumen picos).

## 3. Sensor por puesto — decisión DIFERIDA

No se define hasta tener la maqueta física y medir las distancias reales. El firmware
expone la abstracción `SensorReader` (ver `docs/04-firmware.md`) de modo que comprar un
sensor u otro **no bloquea ningún otro trabajo**.

| Candidato | Precio aprox. | Rango | Encaja cuando… | Precauciones |
|---|---|---|---|---|
| **HC-SR04** (ultrasonido) | $1–3 | 2–400 cm | distancias ≥ ~10 cm y presupuesto ajustado | divisor en ECHO; 2 GPIO por puesto; muestreo secuencial |
| **VL53L0X / VL53L1X** (ToF láser) | $3–5 | 3–200 cm (L1X ~4 m) | se prioriza cableado limpio (bus I2C compartido) y precisión | cuesta más; límite de rango (usar L1X si el techo es alto) |
| **TCRT5000** (IR reflectivo) | ~$1 | 1–25 cm | **maqueta en miniatura** con distancias cortas (<10 cm) | sensible a color/brillo; calibrar umbral |
| **E18-D80NK** (barrera IR) | $2–4 | 3–80 cm | hay pared lateral por pasillo (corte de haz) | alineación estricta emisor/receptor |
| Cámara + visión (OpenCV) | 0 (webcam) | todo el lote | descartada como fuente primaria | no determinista: iluminación y oclusión generan falsos libres/ocupado |

**Regla de decisión:** medir distancia real sensor→objetivo en la maqueta. Si es <10 cm,
el HC-SR04 queda en su zona ciega → optar por IR reflectivo o ToF.

## 4. Software de escritorio

| Capa | Decisión | Alternativas descartadas | Motivo |
|---|---|---|---|
| Framework HMI | **Python 3.12+ / PySide6 (Qt 6)** | C++/Qt (curva y velocidad de desarrollo), C#/WPF (solo Windows), Electron/Tauri (stack JS, peso), JavaFX (ecosistema en declive) | Qt **es** el estándar HMI/SCADA; multiplataforma; desarrollo rápido; enlaza con el ecosistema Python |
| Cliente MQTT | **paho-mqtt 2.x** | amqtt, gmqtt | maduro (Eclipse), API `CallbackAPIVersion.VERSION2`, reconexión con backoff exponencial integrada |
| Persistencia | **SQLite** (`sqlite3` de la stdlib) con WAL | PostgreSQL/MySQL (servicio externo innecesario), JSON plano (sin consultas) | embebido, un solo archivo, sin servidor |
| UI (Widgets o Quick) | *Por definir en la fase de UI* | — | fuera del alcance de esta fase |

## 5. Herramientas

| Uso | Herramienta |
|---|---|
| Dependencias y entorno | `pyproject.toml` + `venv` |
| Lint / formato | `ruff` |
| Pruebas | `pytest` + `pytest-qt` |
| Empaquetado final | `PyInstaller` (un build por sistema operativo) |
| Firmware | Arduino IDE (estructura portable a PlatformIO, ver ADR-008) |
| Broker MQTT | Mosquitto 2.x (local, en la PC de la HMI) |

## 6. Versiones mínimas (verificadas contra docs oficiales)

| Tecnología | Mínimo | Nota de verificación |
|---|---|---|
| Python | 3.12 | PySide6 exige ≥ 3.10 |
| PySide6 | `>=6.8,<7` | última estable consultada: **6.10.1** (doc.qt.io/qtforpython-6) |
| paho-mqtt | `>=2.1,<3` | callbacks versión 2 (`CallbackAPIVersion.VERSION2`) (github.com/eclipse-paho/paho.mqtt.python) |
| Mosquitto | 2.x | broker local, sin autenticación en LAN |
| MQTT | 3.1.1 | por defecto en paho y en PubSubClient |
