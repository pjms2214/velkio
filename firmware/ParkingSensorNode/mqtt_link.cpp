// ---------------------------------------------------------------------------
// mqtt_link.cpp - Implementacion del enlace WiFi/MQTT (docs/04-firmware.md).
//
// Actualmente vacio: contiene solo la estructura del modulo.
// Al implementar aqui entran:
//   1. Conexion WiFi con reintento.
//   2. Conexion MQTT con LWT y mensaje de nacimiento (retain).
//   3. Publicacion de payloads JSON retenidos por puesto.
//   4. Reconexion automatica al broker.
// ---------------------------------------------------------------------------

#include "mqtt_link.h"
#include "config.h"
