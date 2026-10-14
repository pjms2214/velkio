# 04 · Firmware (ESP32)

Organización del sketch, módulos y notas eléctricas. **Sin lógica implementada todavía**
(solo estructura); este documento define *qué* va en cada archivo y *por qué*.

---

## 1. Requisitos de entorno

- **Arduino IDE** con placa **ESP32 Dev Module** instalada (Gestor de tarjetas de ESP32).
- Carpeta del sketch = nombre del archivo `.ino` → `firmware/ParkingSensorNode/`.
- Estructura en varios `.h/.cpp` (ADR-008): si algún día se necesita PlatformIO o CI, la
  migración es mover los archivos y crear un `platformio.ini`.

## 2. Archivos del sketch

| Archivo | Responsabilidad | Estado |
|---|---|---|
| `ParkingSensorNode.ino` | Punto de composición: `setup()`/`loop()`; orquesta muestreo → filtrado → publicación | esqueleto (funciones vacías) |
| `config.h` | Parámetros versionados con **placeholders**: WiFi, IP del broker, ID del controlador, tópico base, versión de firmware | plantilla |
| `config.local.h` | Credenciales reales; **ignorado por git** (`.gitignore`) | no existe en el repo |
| `sensors.h` / `sensors.cpp` | Abstracción de sensores (ADR-007) + filtrado | módulo documentado |
| `mqtt_link.h` / `mqtt_link.cpp` | Conexión WiFi, conexión MQTT con LWT/birth, publicación de payloads retenidos, reconexión | módulo documentado |

## 3. Ciclo de muestreo (responsabilidades, no implementación)

Cada iteración del `loop()` realiza, en este orden:

1. **Lectura secuencial** de los 8 sensores (nunca en paralelo: evita *crosstalk* entre
   ultrasonidos vecinos).
2. **Filtro de mediana** por puesto sobre las últimas N lecturas (elimina picos aislados).
3. **Histéresis de estado**: el cambio libre↔ocupado exige M lecturas consecutivas
   consistentes → evita que el mapa "parpadee".
4. Si el valor queda fuera de rango válido → estado `indeterminado` (nunca `libre`).
5. **Publicación** solo cuando el estado cambia (ahorro de tráfico); los tópicos van con
   retain, así que el último estado siempre está en el broker.

## 4. Abstracción de sensores (ADR-007)

El sketch depende de una interfaz común de lectura con salida normalizada
**distancia en cm + validez**, de modo que la decisión de compra (`docs/01-stack.md` §3)
no bloquea el resto del firmware.

Implementaciones previstas (una elegida al tener la maqueta):

| Implementación | Sensor | Nota |
|---|---|---|
| `UltrasonicReader` | HC-SR04 | 2 GPIO por puesto, divisor de voltaje en ECHO, muestreo secuencial |
| `ToFReader` | VL53L0X / VL53L1X | bus I2C compartido, una dirección por sensor |
| `IrReader` | TCRT5000 / E18-D80NK | salida a umbral, ideal para distancias cortas de maqueta |

## 5. Notas eléctricas

- **Los GPIO del ESP32 no toleran 5 V.** ECHO del HC-SR04 → divisor 1 kΩ / 2 kΩ antes
  del pin. (TRIG a 3.3 V sí es aceptable para el sensor.)
- **GND común** entre placa y todos los sensores; sin él, lecturas fantasma.
- Alimentación USB estable; los ultrasonidos generan picos de corriente al sondear —
  condensador de 100 µF cerca del riel de 5 V si las lecturas se vuelven erráticas.
- Cables largos en la maqueta → posible ruido; si ocurre, soldar 100 pF en TRIG/ECHO o
  acortar el cableado.
- Mapeo de pines: **se define en `config.h` cuando se elija el sensor** (pendiente).

## 6. Configuración y secretos

- `config.h` (versionado): solo placeholders tipo `"TU_SSID"`, IP de ejemplo, tópicos.
- `config.local.h` (**gitignored**): credenciales reales de cada integrante.
- Al implementar, `config.h` incluirá `config.local.h` si existe (`__has_include`), de
  modo que el sketch compila sin él usando placeholders.
- **Nunca** commitear SSID/contraseña/broker real en el repo.

## 7. Librerías candidatas (fijar versión al implementar)

| Librería | Uso |
|---|---|
| PubSubClient | Cliente MQTT para ESP32 |
| ArduinoJson | Construcción de los payloads JSON del contrato |
| (HC-SR04) ninguna extra | Lectura con `pulseIn` o librería propia |
| (VL53L0X) Pololu/VL53L0X | Driver ToF |

## 8. Pendientes de esta fase

- [ ] Elegir el modelo de sensor midiendo distancias reales en la maqueta.
- [ ] Definir el mapeo de pines en `config.h`.
- [ ] Implementar `sensors` y `mqtt_link` siguiendo este documento.
- [ ] Probar end-to-end contra la HMI + simulador (contrato: `docs/03-contrato-mqtt.md`).
