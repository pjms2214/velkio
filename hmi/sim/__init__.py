"""Simulador de hardware: publica estados ficticios al broker MQTT.

Cumple el mismo contrato que el ESP32 (docs/03-contrato-mqtt.md) de modo que la app
no distingue entre hardware real y simulado (ADR-005).

Responsabilidades:
- Publicar estados de los 8 puestos de zona00 (retain, QoS 1) con variacion configurable.
- Publicar el enlace del controlador (online/offline) para probar alarmas.
- Permitir escenarios manuales de demo (ocupar/vaciar puestos) sin tocar la app.

Reglas: puede importar core; NUNCA importa ui (R4). Requiere un Mosquitto local.

Modulos previstos (orientativo, al implementar):
- publicador.py  bucle de publicacion y escenarios de demo
"""
