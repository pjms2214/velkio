#pragma once

// ---------------------------------------------------------------------------
// sensors.h - Abstraccion de lectura de sensores (ADR-007, docs/04-firmware.md §4).
//
// Este archivo declara RESPONSABILIDADES, no la interfaz final: la definicion
// concreta se redacta al implementar el firmware (fase 2).
//
// Responsabilidades del modulo:
//  - Interfaz comun "SensorReader": leer un puesto -> distancia en cm + validez.
//  - Implementaciones previstas (una elegida con la maqueta):
//      * UltrasonicReader - HC-SR04    (divisor 1k/2k en ECHO, 2 GPIO por puesto)
//      * ToFReader        - VL53L0X/L1X (bus I2C compartido)
//      * IrReader         - TCRT5000 / E18-D80NK (distancias cortas de maqueta)
//  - Muestreo SECUENCIAL de los 8 puestos (evita crosstalk entre ultrasonidos).
//  - Filtro de mediana por puesto (ultimas N lecturas).
//  - Histéresis de estado: libre<->ocupado exige M lecturas consistentes.
//  - Estado "indeterminado" si la lectura queda fuera de rango (nunca "libre").
//
// Salida hacia mqtt_link: estado por puesto conforme al contrato MQTT.
// ---------------------------------------------------------------------------
