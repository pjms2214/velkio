"""Comunicacion MQTT de la app (hilo propio -> senales Qt).

Reglas: R1 (core puro), R3 (no importa ui). Ver docs/02-arquitectura.md.

Responsabilidades:
- Cliente paho-mqtt 2.x (CallbackAPIVersion.VERSION2) con reconexion por backoff.
- Suscripcion a 'parqueadero/#' y traduccion de payloads a modelos de core (ADR-001).
- Puente a Qt: senales encoladas hacia la UI; los callbacks NUNCA tocan widgets (ADR-004).
- Interfaz de fuente de estados compartida con el simulador (ADR-005).

Modulos previstos (orientativo, al implementar):
- fuente.py       interfaz StateSource (la cumplen driver MQTT y simulador)
- mqtt_fuente.py  driver MQTT real
- puente_qt.py    objeto QObject que expone senales a la UI
"""
