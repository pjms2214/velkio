# 02 · Arquitectura del proyecto

**Alcance de este documento:** estructura, capas, reglas de dependencia, flujo de datos,
decisiones arquitectónicas (ADR), estrategia de pruebas y requisitos no funcionales.
**No define** diseño de UI ni lógica de negocio.

---

## 1. Visión general

```
┌──────────────────────────────────────────────────────────┐
│ FIRMWARE · firmware/ParkingSensorNode (C++ · Arduino)    │
│  1 sensor/puesto → filtro (mediana + histéresis) → estado │
└──────────────────────────┬───────────────────────────────┘
                           │  WiFi · MQTT (retain + QoS 1 + LWT)
                           │  contrato: docs/03-contrato-mqtt.md
┌──────────────────────────▼───────────────────────────────┐
│ BROKER · Mosquitto (local, IP configurable)              │
└──────────────────────────┬───────────────────────────────┘
                           │  paho-mqtt (hilo propio)
┌──────────────────────────▼───────────────────────────────┐
│ APP HMI · hmi/ (Python 3.12 · PySide6)                   │
│                                                          │
│   comms/ ──modelos──► core/ ◄──modelos── persistence/    │
│                          │                               │
│                     señales Qt (cola)                    │
│                          ▼                               │
│                         ui/                              │
│                                                          │
│   sim/ ──publica al broker con el MISMO contrato──► ◄──  │
└──────────────────────────────────────────────────────────┘
```

## 2. Capas y responsabilidades

| Capa | Paquete | Responsabilidad | Depende de |
|---|---|---|---|
| Firmware | `firmware/ParkingSensorNode/` | Leer sensores, filtrar, publicar estado por MQTT | librerías Arduino (PubSubClient, ArduinoJson) |
| Comunicación | `hmi/comms/` | Conexión MQTT, suscripción, traducción de payload → modelos, puente a señales Qt | `paho-mqtt`, `core` |
| Núcleo | `hmi/core/` | Modelos del dominio (Zona, Plaza, Estado), reglas de ocupación y alarma de enlace | **solo stdlib** |
| Persistencia | `hmi/persistence/` | Historial, eventos y alarmas en SQLite (hilo escritor único) | stdlib `sqlite3`, `core` |
| UI | `hmi/ui/` | Ventanas Qt: mapa de puestos, dashboard, historial | `PySide6`, `core` (vía señales) |
| Simulador | `hmi/sim/` | Publica estados ficticios al broker con el mismo contrato que el ESP32 | `paho-mqtt` |
| Pruebas | `tests/` | Unit, integración y UI según §7 | `pytest`, `pytest-qt` |

## 3. Reglas de dependencia (inviolables)

- **R1** — `core/` **no** importa `PySide6`, `paho.mqtt` ni `sqlite3`. Es dominio puro.
- **R2** — `ui/` **no** importa `paho.mqtt`: la UI recibe datos por señales Qt, jamás toca
  la red.
- **R3** — `comms/` y `persistence/` **no** importan `ui/`.
- **R4** — `sim/` puede importar `core`, pero **nunca** `ui/`.
- **R5** — Todo dato que cruza entre capas es un modelo de `core/` (o el contrato MQTT
  documentado en `docs/03-contrato-mqtt.md` en el borde red).

> Estas reglas se mantienen por convención en la fase 1; pueden automatizarse con
> `import-linter` más adelante sin tocar código.

## 4. Flujo de datos en runtime

1. El ESP32 publica el estado de cada plaza en su tópico (**retain**, QoS 1) y su estado
   de enlace vía **LWT**.
2. Mosquitto entrega los mensajes (incluidos los retenidos al conectar) al suscriptor.
3. `comms/` recibe el mensaje **en su propio hilo** y lo traduce a un modelo de `core/`.
4. El puente emite una **señal Qt encolada**; `core/` actualiza el estado en memoria
   (fuente de verdad de la vista).
5. `ui/` redibuja el mapa de puestos al recibir la señal.
6. `persistence/` registra cada transición de estado en SQLite **desde su hilo**.
7. Si un controlador pasa a `offline` (LWT), `core/` levanta una alarma de enlace que la
   UI muestra de forma distintiva (un puesto desconectado **no** cuenta como libre).

## 5. Decisiones arquitectónicas (ADR)

### ADR-001 — El contrato MQTT es la frontera del sistema
- **Decisión:** firmware y app solo se conocen por `docs/03-contrato-mqtt.md`.
- **Consecuencias:** desarrollo en paralelo de ambos equipos; el simulador es trivial
  (publica el mismo contrato); cambiar el sensor o el firmware no toca la app.

### ADR-002 — Mensajes *retain* + LWT
- **Decisión:** estados de plaza con `retain=true`; cada controlador declara su enlace
  con *Last Will* (`online`/`offline` retenido).
- **Consecuencias:** la HMI arranca con el estado real aunque los ESP32 estén dormidos;
  se detecta desconexion de zona; el suscriptor debe tratar los primeros mensajes como
  estado inicial, no como eventos.

### ADR-003 — Núcleo sin dependencias de UI, red ni persistencia
- **Decisión:** `core/` puro (R1).
- **Consecuencias:** reglas de ocupación y alarma se prueban con `pytest` puro y rápido;
  el núcleo se reutiliza si la UI cambia (Widgets → Quick).

### ADR-004 — MQTT en hilo propio → señales Qt encoladas
- **Decisión:** `paho-mqtt` corre en su hilo (`loop` con callbacks); el puente a Qt usa
  señales con conexión encolada; la UI nunca bloquea.
- **Consecuencias:** cero acceso a widgets desde callbacks de red; reconexión con backoff
  exponencial (`reconnect_delay_set`) sin congelar la ventana.

### ADR-005 — Fuente de estado intercambiable (`StateSource`)
- **Decisión:** el driver MQTT real y el simulador implementan la misma interfaz de
  fuente de estados.
- **Consecuencias:** toda la app se desarrolla y demuestra **sin hardware**; dos
  terminales bastan para la demo (broker + simulador).

### ADR-006 — SQLite con hilo escritor único (WAL)
- **Decisión:** la base se abre con WAL; todas las escrituras pasan por un hilo propio
  de persistencia.
- **Consecuencias:** sin bloqueos de la UI ni carreras entre hilos; lecturas concurrentes
  baratas.

### ADR-007 — Abstracción de sensores en el firmware (`SensorReader`)
- **Decisión:** el sketch depende de una interfaz común de lectura (cm + validez), con
  implementaciones previstas para HC-SR04, ToF (VL53L0X/L1X) e IR.
- **Consecuencias:** la decisión de compra de sensores (pendiente) no bloquea el resto
  del firmware (ver `docs/01-stack.md` §3).

### ADR-008 — Arduino IDE con sketch modular
- **Decisión:** sketch organizado en varios `.h/.cpp` (`config`, `sensors`, `mqtt_link`)
  en vez de un único `.ino`.
- **Consecuencias:** migración trivial a PlatformIO si se necesita CI o depurar con más
  herramientas; lógica de aplicación separada de la configuración.

## 6. Estructura del repositorio

```
Proyecto/
├── firmware/
│   └── ParkingSensorNode/          # Sketch Arduino IDE (carpeta = nombre del .ino)
│       ├── ParkingSensorNode.ino   # Composición: setup()/loop() (solo estructura)
│       ├── config.h                # Parámetros versionados con placeholders
│       ├── config.local.h          # [NO VERSIONADO] credenciales reales (gitignored)
│       ├── sensors.h / .cpp        # Abstracción de sensores + filtrado (ADR-007)
│       └── mqtt_link.h / .cpp      # WiFi, conexión MQTT, LWT/publicaciones (ADR-002)
│
├── hmi/
│   ├── main.py                     # Punto de entrada: python -m hmi.main
│   ├── core/                       # Núcleo de dominio puro (R1, ADR-003)
│   ├── comms/                      # Cliente MQTT + puente de señales Qt (ADR-004/005)
│   ├── persistence/                # Repositorio SQLite, hilo escritor (ADR-006)
│   ├── ui/                         # Ventanas Qt (sin importar paho, R2)
│   └── sim/                        # Publicador simulado (ADR-005)
│
├── tests/
│   ├── unit/                       # core/ puro — sin Qt ni red
│   ├── integration/                # comms/ + persistence/ contra Mosquitto y SQLite
│   └── ui/                         # pytest-qt: arranque de ventanas y señales
│
├── docs/
│   ├── 01-stack.md                 # Decisiones tecnológicas y alternativas
│   ├── 02-arquitectura.md          # Este documento
│   ├── 03-contrato-mqtt.md         # Frontera del sistema
│   └── 04-firmware.md              # Sketch ESP32: módulos y notas eléctricas
│
├── pyproject.toml                  # Dependencias + ruff + pytest
├── .gitignore                      # Incluye config.local.h y *.db
└── README.md
```

## 7. Estrategia de pruebas

| Nivel | Carpeta | Qué cubre | Herramienta |
|---|---|---|---|
| Unitarias | `tests/unit/` | Reglas de `core/`: transiciones de estado, alarmas, conteos | `pytest` (sin Qt, sin red) |
| Integración | `tests/integration/` | `comms/` contra un Mosquitto local; `persistence/` contra SQLite temporal | `pytest` |
| UI | `tests/ui/` | Arranque de ventanas, reacción a señales, sin parpadeos de mapa | `pytest-qt` |
| Firmware | — | Prototipo 1: verificación manual con monitor serie + pruebas con el simulador | — |

Criterio: toda nueva regla de negocio nace con su prueba en `tests/unit/` (el núcleo está
diseñado para que eso sea trivial).

## 8. Requisitos no funcionales

- **Multiplataforma:** solo APIs multiplataforma (`pathlib`, sin puertos COM — por eso
  MQTT y no serial); PyInstaller genera un build por SO.
- **UI nunca bloquea:** todo trabajo de red y de disco ocurre fuera del hilo de UI
  (ADR-004 y ADR-006).
- **Resiliencia:** reconexión automática con backoff exponencial en la app; los ESP32
  reconectan y republican su estado retenido.
- **Seguridad:** sin secretos en el repo (`config.local.h` y ajustes de la app se
  ignoran); broker sin autenticación **solo** en la red local — si el HMI sale de esa
  red, añadir usuario/TLS al broker.
- **Escalabilidad:** el esquema MQTT ya contempla N zonas × N puestos; añadir una placa
  no requiere cambios en la app (ADR-001).
- **Rendimiento:** con 8 puestos el consumo es despreciable; la regla de diseño es que
  el hilo de UI nunca espera I/O.
