#!/bin/bash

# Limpiar archivos de handoff anteriores
rm -f handoff_*

# Lanzar visor del canvas en terminal actual
gnome-terminal -- bash -c "./render_loop; exec bash"

# Esperar un poco para que el visor arranque
sleep 0.5

# Lanzar monitor 1 (filas 0–6)
gnome-terminal -- bash -c "./monitor_process test/monitor1.script X 0 6 1; exec bash"

# Lanzar monitor 2 (filas 7–14)
gnome-terminal -- bash -c "./monitor_process test/monitor2.script X 7 14 2; exec bash"
