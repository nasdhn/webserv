Très bonne question 👍
En gros : **tout ce qui est “ambigu, incohérent, invalide ou interdit” doit être détecté par ton parser**.
Je vais te répondre **comme pour un vrai mini-nginx / webserv (42)**.

Je vais classer les erreurs **par catégorie**, avec **des exemples précis tirés de TON `.conf`**.

---

# 1️⃣ Erreurs de syntaxe (OBLIGATOIRES)

Ces erreurs doivent faire **échouer le parsing** immédiatement.

### 🔴 Accolades

* `{` sans `}`
* `}` sans `{`
* bloc `server` ou `location` (site) non fermé

👉 Exemple à détecter :

```conf
server {
    hostname prout;
```

(fin de fichier sans `}`)

---

### 🔴 Point-virgule `;`

Toute directive doit finir par `;` **sauf les blocs**

❌ Erreurs présentes dans ton fichier :

```conf
defaultFile /path/of/exile          # pas de ;
CGI /path/of/moncul                 # pas de ;
root pipou                          # pas de ;
redirection www.matesticulegauche.com
```

👉 **Erreur fatale**

---

### 🔴 Directive inconnue

Toute clé non reconnue doit être rejetée

Exemple :

```conf
demo { ... }     # OK (bloc site)
caca { ... }     # OK
prout { ... }    # OK
toto truc;       # ❌ inconnu
```

---

# 2️⃣ Erreurs de structure (TRÈS IMPORTANT)

### 🔴 Directive au mauvais niveau

Certaines directives **n’ont le droit d’exister que dans certains blocs**

| Directive | server | site |
| --------- | ------ | ---- |
| hostname  | ✅      | ❌    |
| listen    | ✅      | ❌    |
| errorPage | ✅      | ❌    |
| maxSize   | ✅      | ❌    |
| methods   | ❌      | ✅    |
| root      | ❌      | ✅    |
| CGI       | ❌      | ✅    |

❌ Exemple :

```conf
methods GET POST;
```

au niveau `server` → **erreur**

---

### 🔴 Bloc site hors server

```conf
demo {
   ...
}
```

sans être dans un `server` → **erreur**

---

# 3️⃣ Erreurs de redéfinition / cohérence

### 🟠 Redéfinition multiple

Certaines directives **ne doivent apparaître qu’une seule fois**

#### server

* `maxSize` → une seule fois
* `hostname` → **une seule fois par server** (chez toi ❌)

```conf
hostname prout;
hostname pipou;
hostname alfred;
```

👉 **Erreur ou warning** (au choix du projet)

---

### 🟠 listen dupliqué

```conf
listen localhost:4242;
listen localhost:4242;
```

👉 soit erreur, soit ignoré (à toi de choisir)

---

### 🟠 site avec même nom

```conf
pipou { ... }
pipou { ... }
```

👉 **interdit**

---

# 4️⃣ Erreurs de valeur (TRÈS IMPORTANT)

### 🔴 Boolean invalide

```conf
listDirectory true;
listDirectory false;
listDirectory TRUE;   # ❌ si tu refuses les caps
listDirectory yes;    # ❌
```

👉 À parser explicitement

---

### 🔴 methods invalide

```conf
methods GET POST DELETE;
```

Erreurs possibles :

* méthode inconnue
* doublon
* aucune méthode

❌ Exemple :

```conf
methods GET PIPO DELETE;
```

---

### 🔴 errorPage invalide

```conf
errorPage 400 401 403 404 /path/40x.html;
```

À vérifier :

* codes HTTP ∈ [400–599]
* au moins **1 code**
* un **path valide**

---

### 🔴 listen invalide

```conf
listen localhost:4242;
listen wawou:5000;
```

À vérifier :

* host non vide
* port ∈ [1–65535]
* format `host:port`

---

### 🔴 root / defaultFile

* chemin vide
* chemin relatif interdit (selon ton projet)
* espace dans path non échappé

---

# 5️⃣ Erreurs logiques (BONUS mais pro)

### 🟡 root manquant dans site

Un site **doit avoir un root**

```conf
test {
    methods GET;
}
```

👉 **Erreur**

---

### 🟡 defaultFile sans root

```conf
defaultFile /index.html;
```

sans `root` → warning ou erreur

---

### 🟡 CGI incohérent

```conf
CGI /path/of/cgi/CGI.py;
```

* extension non supportée
* path inexistant

---

# 6️⃣ Erreurs entre servers

### 🔴 Deux servers identiques

Même :

* hostname
* listen

👉 conflit → **erreur**

---

# 7️⃣ Résumé MINIMAL à parser (si tu veux valider 42)

### ❗ Obligatoire

* accolades
* `;`
* directives inconnues
* mauvais niveau
* valeurs invalides
* bloc site hors server
* pointeurs non définis (root manquant)

### ⭐ Bonus

* doublons
* conflits listen / hostname
* chemins invalides

---

## 🧠 Conseil d’implémentation

Fais un parser en **2 passes** :

1. **syntaxique** (structure, `;`, `{}`)
2. **sémantique** (logique, cohérence)

---


# 🧩 Parser de configuration – Checklist

## 1️⃣ Lecture & Pré-traitement
- [x] Ouvrir le fichier `.conf`
- [ ] Gérer erreur si fichier introuvable / illisible
- [x] Lire le fichier ligne par ligne
- [ ] Supprimer les espaces inutiles (trim)
- [ ] Ignorer les lignes vides
- [ ] Gérer les commentaires (`# ...`)

---

## 2️⃣ Vérifications syntaxiques (FATALES)

### Accolades
- [x] Vérifier l’équilibre `{` / `}`
- [x] Refuser `}` sans `{`
- [x] Refuser fin de fichier avec blocs ouverts
- [x] Interdire texte après `}` sur une ligne

### Point-virgule
- [x] Exiger `;` pour toute directive
- [x] Interdire `;` dans les blocs (`server {`, `site {`)
- [x] Détecter lignes sans `;`

### Structure globale
- [x] Autoriser uniquement `server {}` au niveau racine
- [x] Interdire directives hors `server`
- [x] Refuser bloc inconnu hors `server`

---

## 3️⃣ Parsing des blocs

### Bloc `server`
- [x] Créer une nouvelle instance `Server/Config`
- [x] Parser les directives autorisées :
  - [x] `hostname`
  - [x] `listen`
  - [x] `errorPage`
  - [x] `maxSize`
- [x] Autoriser plusieurs `listen`
- [x] Stocker les sites (`location`)

### Bloc `site` (location)
- [x] Détecter le nom du site
- [x] Refuser site sans nom
- [x] Refuser site hors `server`
- [x] Parser les directives autorisées :
  - [x] `methods`
  - [x] `listDirectory`
  - [x] `defaultFile`
  - [x] `uploadingFile`
  - [x] `root`
  - [x] `redirection`
  - [x] `CGI`

---

## 4️⃣ Vérifications de niveau (server / site)
- [x] Refuser directive `site` dans `server`
- [x] Refuser directive `server` dans `site`
- [x] Refuser directive inconnue
- [x] Message d’erreur clair avec ligne concernée

---

## 5️⃣ Validation des valeurs (SÉMANTIQUE)

### Boolean
- [x] Accepter uniquement `true` / `false`
- [x] Refuser toute autre valeur

### methods
- [x] Autoriser seulement `GET`, `POST`, `DELETE`
- [x] Refuser doublons
- [x] Refuser méthode inconnue
- [x] Refuser liste vide

### listen
- [x] Vérifier format `host:port`
- [x] Vérifier port ∈ [1–65535]
- [x] Refuser host vide

### errorPage
- [x] Vérifier codes HTTP ∈ [400–599]
- [x] Exiger au moins un code
- [x] Vérifier présence d’un path

### maxSize
- [x] Vérifier entier positif
- [x] Refuser valeurs négatives ou nulles

### paths (`root`, `defaultFile`, `CGI`)
- [x] Refuser path vide
- [x] Refuser espaces non échappés
- [ ] (Optionnel) Vérifier existence du fichier

---

## 6️⃣ Règles de cohérence

### server
- [x] Refuser plusieurs `maxSize`
- [ ] Refuser doublons `listen`
- [ ] Détecter conflit hostname + listen entre servers

### site
- [ ] Refuser deux sites avec le même nom
- [ ] Exiger `root`
- [ ] Avertir si `defaultFile` sans `root`

---

## 7️⃣ Gestion des erreurs
- [ ] Afficher la ligne fautive
- [ ] Indiquer le type d’erreur
- [ ] Arrêter le parsing sur erreur fatale
- [ ] Différencier erreur / warning

---

## 8️⃣ Nettoyage mémoire
- [ ] Libérer les objets alloués
- [ ] Éviter les fuites mémoire
- [ ] (Bonus) Passer à `std::unique_ptr`

---

## 9️⃣ Tests
- [ ] Fichier valide minimal
- [ ] Fichier sans `server`
- [ ] Accolades manquantes
- [ ] Directive inconnue
- [ ] Valeurs invalides
- [ ] Fichier avec plusieurs servers

---

## 🚀 Bonus (si temps)
- [ ] Lexer (tokenisation propre)
- [ ] Parser à états
- [ ] Support commentaires inline
- [ ] Messages d’erreur type nginx
