#pragma once

// ---------------------------------------------------------------------------
// mqtt_link.h - Enlace WiFi + MQTT del nodo (ADR-002/ADR-008, docs/04-firmware.md).
//
// Este archivo declara RESPONSABILIDADES, no la interfaz final: la definicion
// concreta se redacta al implementar el firmware (fase 2).
//
// Responsabilidades del modulo:
//  - Conexion WiFi con reconexion automatica (credenciales en config.h).
//  - Conexion al broker Mosquitto con:
//      * LWT      -> publica "offline" (retain) si el nodo cae.
//      * birth    -> publica "online"  (retain) al conectarse.
//  - Publicacion de estados por puesto (JSON, retain, QoS 1) segun
//    docs/03-contrato-mqtt.md:
//      topico  : parqueadero/zona00/puestoNN/estado
//      payload : {"estado": "libre|ocupado|indeterminado",
//                 "distancia_cm": <number|null>, "ts": <epoch segundos>}
//  - Publicar SOLO en cambios de estado (los retenidos mantienen el ultimo valor).
//
// Librerias candidatas: PubSubClient + ArduinoJson (docs/04-firmware.md §7).
// ---------------------------------------------------------------------------

#include <Arduino.h>
