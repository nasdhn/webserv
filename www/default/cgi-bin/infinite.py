#!/usr/bin/python3
import time

# On envoie les headers pour que le serveur sache que ça a commencé
print("Content-Type: text/html\r\n\r\n")
print("<h1>Je suis une boucle infinie...</h1>")

while True:
    # On ne fait rien, on attend juste l'éternité
    time.sleep(1)