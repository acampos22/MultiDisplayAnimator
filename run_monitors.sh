#!/bin/bash

# Inicializa canvas
./render_loop &

# Espera 0.3s para asegurar que render_loop empiece
sleep 0.3

# Lanza dos monitores con scripts y letras distintas
gnome-terminal -- bash -c "./monitor_process test/monitor1.script A; exec bash"
gnome-terminal -- bash -c "./monitor_process test/monitor2.script B; exec bash"
