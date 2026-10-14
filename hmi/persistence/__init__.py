"""Persistencia en SQLite: historial, eventos y alarmas.

Regla R3 (no importa ui). Ver ADR-006 en docs/02-arquitectura.md.

Responsabilidades:
- Base SQLite en modo WAL, escrituras SOLO desde el hilo propio de persistencia.
- Registrar transiciones de estado de cada puesto (historial) y alarmas de enlace.
- Consultas de apoyo para las vistas de tendencia/historial (leidas, no escritas, por ui).

Modulos previstos (orientativo, al implementar):
- esquema.sql / migraciones  estructura de tablas
- repositorio.py             altas y consultas; un unico hilo de escritura
"""
