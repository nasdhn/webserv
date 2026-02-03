#!/usr/bin/python3
import cgi
import cgitb
import os
import sys

cgitb.enable()

# FICHIER DE DEBUG (On écrit dans /tmp pour être sûr que ça marche)
LOG_FILE = "/tmp/debug_cgi.txt"

def log(message):
    with open(LOG_FILE, "a") as f:
        f.write(message + "\n")

# On vide le log au début
with open(LOG_FILE, "w") as f:
    f.write("--- DEBUT DU SCRIPT CGI ---\n")

print("Content-Type: text/html\r\n\r\n")
print("<html><body><h1>Debug</h1>")

try:
    # 1. Où suis-je ?
    cwd = os.getcwd()
    log(f"1. Dossier courant (CWD) : {cwd}")
    
    # 2. Le dossier upload existe-t-il VU D'ICI ?
    # Note: J'utilise un chemin ABSOLU basé sur le CWD pour être sûr
    upload_target = os.path.join(cwd, "www/default/upload")
    
    if os.path.exists(upload_target):
        log(f"2. Dossier upload TROUVÉ : {upload_target}")
    else:
        log(f"2. Dossier upload INTROUVABLE à : {upload_target}")
        # Tentative de secours : chercher juste "upload" ou "../upload"
        log("   -> Tentative de lister le dossier courant : " + str(os.listdir(cwd)))

    # 3. Réception des données
    form = cgi.FieldStorage()
    keys = list(form.keys())
    log(f"3. Clés reçues dans le formulaire : {keys}")

    if "file" in form:
        fileitem = form["file"]
        if fileitem.filename:
            fn = os.path.basename(fileitem.filename)
            # On force le chemin complet
            filepath = os.path.join(upload_target, fn)
            
            log(f"4. Tentative d'écriture dans : {filepath}")
            
            with open(filepath, 'wb') as f:
                f.write(fileitem.file.read())
            
            # CHMOD
            try:
                os.chmod(filepath, 0o755)
                log("5. Chmod OK")
            except Exception as e:
                log(f"5. Erreur Chmod : {e}")
                
            print("Upload OK")
        else:
            log("Erreur: Pas de filename")
    else:
        log("Erreur: Pas de clé 'file'")

except Exception as e:
    log(f"CRASH FATAL : {e}")
    print(e)

print("</body></html>")