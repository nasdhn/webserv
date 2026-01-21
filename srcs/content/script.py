#!/usr/bin/python3
import os
import sys

# 1. Headers obligatoires pour le protocole HTTP
print("Content-Type: text/html\r\n\r\n")

# 2. Le corps de la réponse HTML
print("<html><body>")
print("<h1>Succes : Le CGI fonctionne !</h1>")

# 3. Preuve qu'on recoit bien l'environnement de C++
print("<h2>Variables d'environnement :</h2>")
print("<ul>")
method = os.environ.get('REQUEST_METHOD', 'INCONNUE')
path = os.environ.get('PATH_INFO', 'INCONNU')
print(f"<li>Methode: <b>{method}</b></li>")
print(f"<li>Chemin: <b>{path}</b></li>")
print("</ul>")

print("</body></html>")