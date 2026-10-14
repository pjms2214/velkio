#pragma once

// ---------------------------------------------------------------------------
// config.h - Configuracion versionada del nodo (docs/04-firmware.md §6).
//
// Este archivo versionado solo contiene PLACEHOLDERS.
// Las credenciales reales van en "config.local.h" (ignorado por git).
// Al implementar, config.h incluira config.local.h si existe (__has_include).
// NUNCA commitear SSID, contrasena ni IP real del broker.
// ---------------------------------------------------------------------------

// --- Identidad del controlador ---------------------------------------------
#define FW_VERSION      "0.1.0"
#define CONTROLLER_ID   "controlador-zona00"

// --- WiFi (placeholders) ----------------------------------------------------
#define WIFI_SSID       "TU_SSID"
#define WIFI_PASS       "TU_PASSWORD"

// --- MQTT (ver docs/03-contrato-mqtt.md) -----------------------------------
#define MQTT_BROKER_IP   "192.168.1.100"   // IP fija en LAN de la PC con Mosquitto
#define MQTT_BROKER_PORT 1883
#define MQTT_TOPIC_BASE  "parqueadero/zona00"                            // .../puestoNN/estado
#define MQTT_TOPIC_LINK  "parqueadero/controlador/zona00/estado"         // online/offline (LWT)

// --- Puestos y pines --------------------------------------------------------
// Prototipo 1: 8 puestos (puesto00 .. puesto07).
// El mapa de pines se define aqui cuando se elija el sensor
// (docs/01-stack.md §3): divisor de voltaje en ECHO si es HC-SR04.
// #define SENSOR_PINS { ... }

// --- Muestreo ---------------------------------------------------------------
// Valores de filtrado/histéresis (N lecturas por mediana, M para cambiar estado)
// a fijar en la fase de firmware (docs/04-firmware.md §3).
