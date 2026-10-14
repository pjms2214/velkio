"""Punto de entrada de la app HMI (Prototipo 1 - maqueta, 8 puestos).

Responsabilidades (al implementar):
- Arrancar la aplicacion Qt (QApplication) y la ventana principal.
- Crear la fuente de estados (MQTT real o simulador, ver ADR-005) y el puente de senales.
- Crear el repositorio de persistencia (hilo escritor, ADR-006) y conectarlo a core.
- Salida ordenada: cerrar hilos de red y base de datos al terminar.

Ejecucion (desde la raiz del repositorio):
    python -m hmi.main

Documentacion:
- docs/02-arquitectura.md (flujo de datos, ADR)
- docs/03-contrato-mqtt.md (frontera del sistema)

Nota: este archivo es solo estructura; todavia no contiene logica.
"""
