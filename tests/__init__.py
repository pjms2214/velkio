"""Pruebas del HMI de parqueadero.

Estrategia (docs/02-arquitectura.md §7):
- unit/         reglas de core/ puro: sin Qt, sin red, rapidas.
- integration/  comms/ contra Mosquitto local y persistence/ contra SQLite temporal.
- ui/           pytest-qt: arranque de ventanas y respuesta a senales.

Ejecucion: pytest desde la raiz del repositorio.
"""
