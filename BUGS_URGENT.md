# BUGS URGENT - get_next_line.c

## ERREURS DE COMPILATION (bloquantes)

### 1. L20: Variable `i` déclarée mais inutilisée
**Fix:** Supprimer la déclaration `int i;` (ligne 20)

### 2. L60: Bitwise `|` au lieu de logique `||`
**Fix:** Remplacer `if (BUFFER_SIZE <= 0 | read(fd, buffer, 0) == -1)` par `if (BUFFER_SIZE <= 0 || read(fd, buffer, 0) == -1)`

### 3. L87: Variable `fd` déclarée mais inutilisée dans main
**Fix:** Soit utiliser `fd` avec get_next_line(fd), soit supprimer la déclaration

---

## BUGS LOGIQUES CRITIQUES (ordre de priorité)

### 1. L40-43: Boucle infinie → Crash garanti
**Problème:** `while (i != len) { stash[i] = '\0'; }` - pas d'incrémentation de `i`

**Pourquoi critique:** Je me demande si ce programme peut survivre plus de quelques microsecondes avant de planter... Cette boucle tourne à l'infini en réécrivant indéfiniment `stash[0] = '\0'`. Le CPU va chauffer pour rien.

**Piste:** Je me demande si j'ai oublié quelque chose entre les accolades qui ferait progresser ma condition de sortie...

---

### 2. L29: Buffer overflow → Corruption mémoire
**Problème:** `ft_strlcat(stash, buffer, len)` où `len` est la longueur ACTUELLE du stash

**Pourquoi critique:** Je me demande si je confonds la taille du buffer avec l'espace disponible... `ft_strlcat` attend la taille TOTALE du buffer destination (pas la longueur du contenu actuel). Si stash fait 1024 octets mais contient 50 caractères, passer `len=50` va tronquer mes données et potentiellement écrire hors limites.

**Piste:** Je me demande quelle est la différence entre "longueur du contenu" et "taille totale allouée"...

---

### 3. L71: Memory leak → Fuite mémoire à chaque appel
**Problème:** `stash = malloc(...)` sans `free` de l'ancien pointeur

**Pourquoi critique:** Je me demande où va la mémoire précédemment allouée quand je réassigne `stash` directement... À chaque appel qui passe par cette condition, je perds définitivement l'adresse de l'ancien bloc. Valgrind va hurler.

**Piste:** Je me demande si je ne devrais pas libérer avant de réallouer, ou utiliser `realloc` pour étendre proprement...

---

### 4. L26: Logique inversée → Boucle ne s'exécute jamais correctement
**Problème:** `while (ft_strchr(stash, '\n') || read(fd, buffer, 0) > 0)`

**Pourquoi critique:** Je me demande si cette condition fait ce que je pense... Si `ft_strchr` trouve un `\n`, elle retourne un pointeur (vrai), donc je CONTINUE à lire. Mais je veux justement m'ARRÊTER quand j'ai trouvé `\n`. Et `read(fd, buffer, 0)` demande 0 octets, donc retourne toujours 0 (jamais > 0).

**Piste:** Je me demande si je ne devrais pas inverser la logique (`!ft_strchr`) et utiliser une vraie lecture avec BUFFER_SIZE...

---

### 5. L28: read() sans vérification du retour → Données non contrôlées
**Problème:** `read(fd, buffer, BUFFER_SIZE)` - valeur de retour ignorée

**Pourquoi critique:** Je me demande ce qui se passe si `read` retourne -1 (erreur) ou 0 (EOF)... Je continue comme si de rien n'était et je concatène du garbage dans mon stash. Impossible de détecter la fin de fichier proprement.

**Piste:** Je me demande si je ne devrais pas stocker ce retour dans une variable et adapter ma logique selon le résultat...

---

## BONUS: Autres problèmes à investiguer

- **L38:** `len = ft_strchr(stash, '\n')` - je me demande si ft_strchr retourne un entier ou un pointeur...
- **L47:** `stash[i + len]` - je me demande si cet accès est toujours dans les limites de mon allocation...
- **L75:** `malloc(sizeof(char) * len + 2)` - je me demande si la précédence des opérateurs fait ce que j'attends (`len + 2` ou `sizeof(char) * len` puis `+ 2`?)...
- **L80:** `if (read(fd, buffer, 0) > 1)` - je me demande pourquoi demander 0 octets et vérifier si c'est > 1 (impossible)...
