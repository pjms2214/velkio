/**
 * ParkingSensorNode - Nodo de sensores del parqueadero (ESP32).
 *
 * Prototipo 1: maqueta de 8 puestos en zona00, una sola placa.
 *
 * Responsabilidades de este archivo (punto de composicion, ADR-008):
 *  - setup(): inicializar serial, WiFi, MQTT (con LWT) y sensores.
 *  - loop() : ciclo de muestreo -> filtrado -> publicacion de estados retenidos.
 *
 * El detalle de cada modulo esta en docs/04-firmware.md.
 * Contrato de mensajes: docs/03-contrato-mqtt.md.
 *
 * Nota: esqueleto sin logica; la implementacion llega en la fase firmware.
 */

#include "config.h"
#include "sensors.h"
#include "mqtt_link.h"

void setup() {
    // Por implementar: serial, WiFi (config.h), MQTT con LWT/birth, sensores.
}

void loop() {
    // Por implementar: leer secuencial -> filtro mediana -> histéresis -> publicar.
}
