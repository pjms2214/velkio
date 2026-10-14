"""Nucleo de dominio: modelos y reglas del parqueadero.

Regla R1 (ADR-003): este paquete NO importa PySide6, paho.mqtt ni sqlite3.
Es dominio puro, probado con pytest sin Qt ni red.

Responsabilidades:
- Modelos de los puestos: Zona, Plaza, EstadoOcupacion (libre/ocupado/indeterminado).
- Reglas de ocupacion: conteos, transiciones, historia de eventos.
- Alarma de enlace: estado online/offline de cada controlador de zona.

Modulos previstos (orientativo, al implementar):
- modelos.py   modelos de dominio inmutables/enum de estados
- reglas.py    transiciones, conteos y condiciones de alarma
"""
