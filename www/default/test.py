#!/usr/bin/python3
import os
import sys

# 1. On affiche les headers requis par le protocole CGI
# (Note: Si ton serveur n'analyse pas les headers CGI, ceci s'affichera dans le navigateur, ce n'est pas grave pour le test)
print("Content-Type: text/html\r\n\r\n")

print("<html><body>")
print("<h1>Test CGI Python</h1>")

# 2. Afficher la méthode utilisée
method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
print(f"<h2>Method: {method}</h2>")

# 3. Afficher le Body reçu (Test du POST)
print("<h3>Body Content:</h3>")
print("<pre>")

# On lit STDIN. Si ton serveur bloque, le script plantera ici.
try:
    # On lit le content-length pour savoir combien lire
    content_length = os.environ.get("CONTENT_LENGTH")
    if content_length:
        bytes_to_read = int(content_length)
        body = sys.stdin.read(bytes_to_read)
        print(f"[{body}]")
    else:
        print("(No Body or Content-Length missing)")
except Exception as e:
    print(f"Error reading stdin: {e}")

print("</pre>")
print("</body></html>")