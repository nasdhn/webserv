#!/usr/bin/python3
import sys
import time

# On
print("Content-Type: text/plain\r\n\r\n", end='', flush=True)

received_bytes = 0

while True:
    # On lit par petits blocs de 1KB
    chunk = sys.stdin.read(1024)
    if not chunk:
        break
    received_bytes += len(chunk)
    # ON RALENTIT VOLONTAIREMENT : Cela va forcer le pipe d'entrée à saturer
    # Le serveur devra alors gérer le POLLOUT et ne pas bloquer
    time.sleep(0.001)

print(f"J'ai bien recu {received_bytes} octets. Fin du script.")