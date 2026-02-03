#!/usr/bin/python3
import os
import sys

# On envoie les headers tout de suite pour ne pas faire timeout le serveur
print("Content-Type: text/html\r\n\r\n")

print("<!DOCTYPE html><html><body>")
print("<h1>CGI DEBUG MODE</h1>")

# 1. Vérifions les variables d'environnement cruciales
print("<h2>Variables d'environnement reçues :</h2>")
print("<ul>")
relevant_keys = ["REQUEST_METHOD", "CONTENT_LENGTH", "CONTENT_TYPE", "SCRIPT_NAME"]
for key in relevant_keys:
    val = os.environ.get(key, "NON DÉFINIE ❌")
    icon = "✅" if val != "NON DÉFINIE ❌" else "❌"
    print(f"<li><b>{key}</b> : {val} {icon}</li>")
print("</ul>")

# 2. Tentative de lecture brute (sans parsing)
print("<h2>Lecture de STDIN :</h2>")
try:
    # On récupère la taille attendue
    length = os.environ.get("CONTENT_LENGTH")
    
    if length:
        length = int(length)
        print(f"<p>Le serveur dit qu'il y a {length} octets à lire.</p>")
        
        # On lit stdin
        # ATTENTION : Si le serveur n'envoie rien, cette ligne va bloquer indéfiniment (Deadlock)
        data = sys.stdin.read(length) 
        
        print(f"<p>J'ai réussi à lire : <b>{len(data)} octets</b>.</p>")
        print("<h3>Début du contenu brut :</h3>")
        print(f"<pre>{data[:200]}</pre>") # Affiche juste le début
    else:
        print("<p style='color:red'>CONTENT_LENGTH est vide ! Je ne peux pas lire stdin sans risquer de bloquer.</p>")

except Exception as e:
    print(f"<p style='color:red'>Erreur fatale pendant la lecture : {e}</p>")

print("<button onclick='window.location.href=\"/index.html\"'>Retour</button>")
print("</body></html>")