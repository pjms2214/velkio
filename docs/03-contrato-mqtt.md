# 03 · Contrato MQTT — la frontera del sistema

Este documento define **todo** lo que cruza entre firmware, app HMI y simulador.
Es el único acuerdo que comparten los tres (ADR-001): cualquier cambio aquí afecta a los
tres consumidores y debe registrarse en §6.

---

## 1. Broker y conexión

| Parámetro | Valor |
|---|---|
| Broker | **Mosquitto** 2.x, corriendo en la PC de la HMI |
| Host por defecto | `localhost` (HMI) / IP fija en LAN (ESP32) |
| Puerto | `1883` (MQTT sin TLS, red local) |
| Protocolo | MQTT 3.1.1 (por defecto en paho-mqtt y PubSubClient) |
| Autenticación | Ninguna en el Prototipo 1 (red local; ver ADR de seguridad en `docs/02-arquitectura.md` §8) |
| IP del broker | **Configurable** desde la app y en `firmware/.../config.h` |

## 2. Tópicos

Prefijo fijo: `parqueadero/` · Mayúsculas/acentos/ñ **prohibidos** en tópicos.

| Tópico | Publicador | Suscriptor | QoS | Retain | Significado |
|---|---|---|---|---|---|
| `parqueadero/zona{NN}/puesto{NN}/estado` | ESP32 (y simulador) | HMI | 1 | **sí** | Estado de ocupación de un puesto |
| `parqueadero/controlador/{zona}/estado` | ESP32 (birth + LWT) | HMI | 1 | **sí** | Enlace del controlador de zona: `online` / `offline` |

- `zona{NN}`: dos dígitos, cero a la izquierda → `zona00`, `zona01`, …
- `puesto{NN}`: dos dígitos → `puesto00` … `puesto07` (Prototipo 1: 8 puestos en `zona00`).
- Suscripción de la HMI: **`parqueadero/#`** con QoS 1.
- El esquema ya contempla N zonas × N puestos: añadir una placa no cambia este contrato.

## 3. Payloads

Codificación: **UTF-8**, JSON, campos en `snake_case` en minúscula.
Los lectores **deben ignorar campos desconocidos** (compatibilidad hacia adelante).

### 3.1 Estado de puesto — `parqueadero/zona00/puesto03/estado`

| Campo | Tipo | Obligatorio | Descripción |
|---|---|---|---|
| `estado` | string | **sí** | `libre` \| `ocupado` \| `indeterminado` |
| `distancia_cm` | number \| null | no | Lectura filtrada del sensor en cm; `null` si no aplica o falló |
| `ts` | number | **sí** | Marca de tiempo epoch en **segundos** (UTC) |

```json
{"estado": "ocupado",  "distancia_cm": 42.5,  "ts": 1791234567}
{"estado": "libre",    "distancia_cm": 183.0, "ts": 1791234569}
{"estado": "indeterminado", "distancia_cm": null, "ts": 1791234571}
```

Semántica de `estado`:

- `libre` — el puesto está disponible.
- `ocupado` — hay vehículo.
- `indeterminado` — sensor fuera de rango, con error o desconectado. **Nunca cuenta
  como libre**: la HMI lo muestra de forma distintiva (ver ADR-002, flujo 7).

### 3.2 Enlace del controlador — `parqueadero/controlador/zona00/estado`

Payload de texto plano (no JSON), solo dos valores:

```text
online    # publicado al conectar (birth message, retain)
offline   # publicado por el broker vía Last Will (LWT) si el ESP32 cae
```

Ambos con `retain=true`, QoS 1.

## 4. Reglas del contrato

1. Todo mensaje de tópico `.../estado` se publica con **`retain=true`** y **QoS 1**.
2. La HMI trata los primeros mensajes recibidos (los retenidos) como **estado inicial**,
   no como eventos — no deben generar historial de transiciones.
3. El ESP32 configura su **LWT** en la conexión (`offline` retenido) y publica `online`
   apenas se conecta.
4. Un mensaje nuevo de un puesto es una **foto del presente**, no una orden: el mismo
   estado repetido no genera evento en la app.
5. Cambios de contrato (nuevo campo, nuevo tópico, nuevo valor de estado) se documentan
   en §6 **antes** de implementarlos.

## 5. Ejemplo de flujo (Prototipo 1)

```text
ESP32  →  parqueadero/controlador/zona00/estado        "online"      (retain)
ESP32  →  parqueadero/zona00/puesto00/estado           {"estado":"libre",...}   (retain)
ESP32  →  parqueadero/zona00/puesto01/estado           {"estado":"ocupado",...} (retain)
   ... (8 puestos)
[el ESP32 se cai] → broker publica por LWT             "offline"     (retain)
HMI: alarma de zona desconectada; los puestos conservan su último estado retenido.
[el ESP32 se reconecta] → publica "online" y republica los 8 estados.
```

## 6. Historial de versiones del contrato

| Versión | Fecha | Cambio |
|---|---|---|
| v1 | 2026-10-13 | Versión inicial: tópicos por zona/puesto, payload de 3 campos, LWT de controlador. |
