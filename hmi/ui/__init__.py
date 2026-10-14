"""Ventanas y widgets Qt de la app HMI.

Regla R2: este paquete NO importa paho.mqtt - recibe datos por senales Qt (ADR-004).
Depende solo de PySide6 y de los modelos de core.

Responsabilidades (definicion de UI fuera del alcance de esta fase):
- Mapa de los 8 puestos en tiempo real (libre/ocupado/indeterminado/desconectado).
- Dashboard: contadores, estado de enlace de zonas, alarmas.
- Historial/tendencias alimentados por persistence.

Modulos previstos (orientativo, al implementar):
- ventana_principal.py   contenedor y navegacion
- mapa_plazas.py         vista del mapa de puestos
- dashboard.py           contadores y alarmas
- historial.py           consultas de tendencia
"""
