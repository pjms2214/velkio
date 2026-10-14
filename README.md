# HMI Parqueadero

**Prototipo 1 — maqueta a escala (8 puestos)**

Aplicación de escritorio (HMI) para gestionar y visualizar en tiempo real la ocupación de
un parqueadero. El estado de cada puesto lo reporta un ESP32 con un sensor por puesto; los
mensajes viajan por WiFi mediante MQTT hasta la app, que muestra el mapa de puestos,
persiste el historial en SQLite y señala alarmas de enlace.

> **Estado del repositorio:** estructura de proyecto y arquitectura documentadas.
> Aún no hay lógica de negocio ni UI implementadas.

## Stack resumido

| Capa | Tecnología |
|---|---|
| Controlador | 1× ESP32 (8 puestos en una sola placa) |
| Sensores | 1 por puesto — *modelo pendiente de definir con la maqueta* (ver `docs/01-stack.md` §3) |
| Firmware | C++ (Arduino IDE) |
| Comunicación | WiFi + MQTT (broker Mosquitto local, mensajes *retained* + LWT) |
| App HMI | Python 3.12+ / PySide6 (Qt 6) — multiplataforma |
| Persistencia | SQLite (WAL, hilo escritor único) |
| Calidad | pytest + pytest-qt, ruff |
| Empaquetado final | PyInstaller (un build por sistema operativo) |

## Estructura del repositorio

```
Proyecto/
├── firmware/
│   └── ParkingSensorNode/     # Sketch de Arduino IDE (ESP32): config, sensores, MQTT
├── hmi/                       # App de escritorio Python (PySide6)
│   ├── main.py                # Punto de entrada (python -m hmi.main)
│   ├── core/                  # Núcleo de dominio: modelos y reglas (sin Qt, sin red)
│   ├── comms/                 # Cliente MQTT en hilo propio + puente de señales Qt
│   ├── persistence/           # SQLite: historial, eventos, alarmas (hilo escritor)
│   ├── ui/                    # Ventanas Qt: mapa de puestos, dashboard, historial
│   └── sim/                   # Publicador simulado (misma fuente de datos que el hardware)
├── tests/
│   ├── unit/                  # Núcleo puro (sin Qt ni red)
│   ├── integration/           # comms + persistence contra Mosquitto/SQLite reales
│   └── ui/                    # pytest-qt (arranque de ventanas, señales)
├── docs/                      # Arquitectura y contratos
├── pyproject.toml             # Dependencias + configuración de ruff y pytest
└── .gitignore
```

## Documentación

| Documento | Contenido |
|---|---|
| [`docs/01-stack.md`](docs/01-stack.md) | Decisiones tecnológicas, alternativas descartadas, criterios para elegir sensor |
| [`docs/02-arquitectura.md`](docs/02-arquitectura.md) | Capas, reglas de dependencia, flujo de datos, decisiones (ADR), estrategia de pruebas |
| [`docs/03-contrato-mqtt.md`](docs/03-contrato-mqtt.md) | Tópicos, payloads JSON, *retain*/LWT — la frontera del sistema |
| [`docs/04-firmware.md`](docs/04-firmware.md) | Organización del sketch ESP32, abstracción de sensores, notas eléctricas |

## Requisitos de desarrollo

- Python 3.12+ (con `venv`)
- Mosquitto (broker MQTT local)
- Arduino IDE con soporte ESP32 *(solo cuando haya hardware)*

```bash
python -m venv .venv
.venv\Scripts\activate        # Windows
pip install -e ".[dev]"       # dependencias de la app + herramientas
```

Ejecución de la app (cuando exista):

```bash
python -m hmi.main
```

## Próximas fases

1. Esqueleto ejecutable de la app (`comms` + `core` mínimos) demostrable con el simulador.
2. Firmware del ESP32 con la abstracción de sensores.
3. Definición del modelo de sensor con la maqueta física (distancias reales).
4. UI HMI: mapa en vivo, alarmas, historial.
5. Empaquetado con PyInstaller.
