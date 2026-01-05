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

Si tu veux, je peux :

* te faire **une checklist officielle type webserv**
* te proposer une **grammar BNF**
* t’aider à structurer un parser propre (lexer + parser)

Dis-moi 🔥
