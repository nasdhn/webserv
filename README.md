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

Exécution
Lancez le serveur en fournissant un fichier de configuration. Si aucun fichier n'est fourni, le serveur peut utiliser une configuration par défaut ou s'arrêter selon l'implémentation.
./webserv [configuration_file.conf]

Exemple :
./webserv l.conf

Une fois le serveur lancé (le port par défaut est généralement 8080), vous pouvez y accéder via votre navigateur : http://localhost:8080

Fonctionnalités
Le serveur implémente les fonctionnalités obligatoires suivantes :

Méthodes HTTP : Support complet des requêtes GET, POST, et DELETE.

Multiplexage E/S : Opérations d'E/S non bloquantes utilisant poll().

Support CGI : Exécution de scripts .py et .php via l'interface CGI (Common Gateway Interface).

Upload de fichiers : Gestion robuste du multipart/form-data pour les téléchargements de fichiers via POST.

Configuration : Parsing de fichiers .conf personnalisés (ports, hostnames, pages d'erreur par défaut, limites de taille du corps client, routes, etc.).

Contenu Statique : Service de fichiers HTML, CSS, JS et images.

Listing de Répertoire : Fonctionnalité autoindex lorsque le listage de répertoires est activé.

Gestion des Erreurs : Pages d'erreurs personnalisées (404, 403, 500, 413, etc.).


Ressources & Utilisation de l'IA
🔗 Ressources utiles
Socket Programming in C++

Guide Curl - Administration Serveur

What is Epoll?

Understanding the Poll Function

🤖 Utilisation de l'IA
Conformément aux exigences du sujet, nous déclarons explicitement que des outils d'Intelligence Artificielle (ChatGPT / Gemini) ont été utilisés pendant le développement de ce projet comme outils de support pour les tâches suivantes :

Explication de concepts : Clarification de sujets complexes tels que les E/S non bloquantes, le cycle de vie de poll(), et la structure des requêtes/réponses HTTP.

Debugging : Aide à l'identification de causes d'erreurs complexes (ex: logique de parsing, gestion de la mémoire en C++98).

Génération de tests : Création de scripts Python pour les tests CGI et de commandes curl complexes pour vérifier les cas limites (comme les uploads multipart).

