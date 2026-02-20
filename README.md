# 🌐 Webserv

Ce projet a été réalisé dans le cadre du cursus **42** par **Cyril Goldenschue** (cgoldens), **Yann Oberson** (yaoberso) et **Nasim Dahman** (nadahman).

---

## 📝 Description

**Webserv** est un serveur HTTP/1.1 fonctionnel écrit en **C++98**. 
Le but principal de ce projet est de comprendre les mécanismes internes du protocole HTTP, la programmation réseau via les sockets et le multiplexage d'E/S.

Contrairement à un serveur bloquant traditionnel, Webserv utilise `poll()` pour gérer simultanément plusieurs connexions clients sans bloquer le flux d'exécution. Il est conçu pour être robuste, conforme aux standards HTTP strictement définis, et capable de gérer du contenu statique ainsi que du contenu dynamique via **CGI**.

---

## 🚀 Instructions

### 🛠 Compilation
Pour compiler le projet, exécutez simplement `make` à la racine du dépôt :

```bash
make

💻 Exécution
Lancez le serveur en fournissant un fichier de configuration.

```bash
./webserv [configuration_file.conf]

Exemple :

```bash
./webserv l.conf
Note : Une fois le serveur lancé (port par défaut : 8080), accédez-y via : http://localhost:8080

✨ Fonctionnalités
Le serveur implémente les fonctionnalités suivantes :

Méthodes HTTP : Support complet des requêtes GET, POST, et DELETE.

Multiplexage E/S : Opérations non bloquantes utilisant poll().

Support CGI : Éxécution de scripts .py et .php.

Upload de fichiers : Gestion du multipart/form-data via POST.

Configuration : Parsing de fichiers .conf (ports, hostnames, routes, etc.).

Gestion des Erreurs : Pages d'erreurs personnalisées (404, 403, 500, etc.).

Ressources & IA
🔗 Liens utiles
Socket Programming in C++

Guide Curl

🤖 Utilisation de l'IA
Conformément aux exigences du sujet, nous déclarons l'usage d'outils d'IA (ChatGPT / Gemini) pour :

Explication de concepts (E/S non bloquantes, poll()).

Debugging (Logique de parsing, C++98).

Génération de tests (Scripts Python, commandes curl).