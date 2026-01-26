#!/usr/bin/python3
import os
import sys

# --- 1. HEADERS ---
# Le CGI doit envoyer les headers suivi d'une ligne vide
print("Content-Type: text/html")
print("Connection: close")  # Redondant mais explicite
print()                     # La ligne vide OBLIGATOIRE

# --- 2. HTML START ---
print("<html><head><title>CGI Test</title></head><body>")
print("<h1 style='color:blue;'>Test CGI Python Réussi ! 🚀</h1>")

# --- 3. INFOS REQUÊTE ---
method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
print(f"<h2>Méthode reçue : {method}</h2>")
print(f"<p><b>Script Name:</b> {os.environ.get('SCRIPT_FILENAME', '?')}</p>")
print(f"<p><b>Query String:</b> {os.environ.get('QUERY_STRING', '?')}</p>")

# --- 4. LECTURE DU BODY (POST) ---
print("<h3>Contenu du Body (STDIN) :</h3>")
try:
    # On vérifie si on a du contenu à lire
    length_str = os.environ.get("CONTENT_LENGTH", "0")
    if length_str.isdigit():
        length = int(length_str)
        if length > 0:
            # On lit sur l'entrée standard (le Pipe d'entrée du serveur)
            body = sys.stdin.read(length)
            print(f"<pre style='background-color:#f4f4f4; padding:10px; border:1px solid #ccc;'>{body}</pre>")
        else:
            print("<i>(Content-Length est 0)</i>")
    else:
        print("<i>(Content-Length vide ou invalide)</i>")

except Exception as e:
    print(f"<p style='color:red'>Erreur lors de la lecture du body : {e}</p>")

# --- 5. VARIABLES D'ENVIRONNEMENT ---
print("<h3>Variables d'environnement reçues :</h3>")
print("<ul style='font-size:0.9em;'>")
for k, v in os.environ.items():
    print(f"<li><b>{k}</b> : {v}</li>")
print("</ul>")

print("</body></html>")