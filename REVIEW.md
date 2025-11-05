# 🔍 REVIEW - get_next_line

## 📋 Résumé Général

Bon, j'ai un début de structure intéressante avec le système de stash statique, mais il y a des problèmes assez critiques qui me sautent aux yeux. Je vois des bugs logiques évidents, des memory leaks potentiels, et des includes qui n'ont rien à faire dans un rendu final. La logique générale semble correcte (lire → stocker → extraire ligne → nettoyer stash), mais l'implémentation a besoin de sérieux ajustements.

---

## 🔎 Réflexions par Fichier

### 📄 `get_next_line.h`

**Observations générales:**
- Header guard: ✅ OK
- Includes: ✅ `<unistd.h>` pour read()
- BUFFER_SIZE: ✅ Protection avec `#ifndef`

**Questions que je me pose:**

🤔 **Est-ce que j'ai besoin de `<stdlib.h>` dans le header?**
   - Je l'utilise dans le .c pour malloc/free
   - Normalement je mets les includes system dans le .c, pas dans le .h
   - Est-ce que le header devrait être minimal?

🤔 **Mes prototypes de fonctions utilitaires:**
   - Pourquoi ft_strchr retourne un `int` et pas un `char *` comme la vraie strchr?
   - Ai-je besoin de ft_strlcat pour ce projet? Est-ce que je l'utilise correctement?
   - Ces signatures sont-elles cohérentes avec mon implémentation?

---

### 📄 `get_next_line.c`

**⚠️ PROBLÈMES CRITIQUES DÉTECTÉS:**

#### 🚨 Ligne 14-15: Includes interdits
```c
#include <fcntl.h>
#include <stdio.h>
```
**Réflexion:** Pourquoi ai-je laissé ces includes? 
- `<fcntl.h>` et `<stdio.h>` sont pour les tests avec le main()
- Est-ce que je dois les virer avant le rendu?
- La norminette va-t-elle les accepter?

#### 🚨 Lignes 85-90: Main() dans le rendu
```c
int	main(void)
{
	int	fd;
	fd = open("text.txt", O_RDONLY);
}
```
**Réflexion:** OUPS! 
- Pourquoi j'ai laissé le main dans le fichier de rendu?
- Je dois absolument le retirer
- Que se passe-t-il quand le moulinette compile ça avec son propre main?

#### 🚨 Ligne 60: Opérateur bizarre
```c
if (BUFFER_SIZE <= 0 | read(fd, buffer, 0) == -1)
```
**Questions:**
- Pourquoi j'utilise `|` (bitwise OR) au lieu de `||` (logical OR)?
- Est-ce que ça va court-circuiter correctement?
- Que se passe-t-il si BUFFER_SIZE est invalide mais que je fais quand même un read()?

#### 🚨 Ligne 71: Double malloc()?!
```c
stash = malloc(malloc(sizeof(char) * BUFFER_SIZE));
```
**WTF moment:**
- Qu'est-ce que je voulais faire ici exactement?
- `malloc(malloc(...))` ça n'a AUCUN sens
- Je voulais faire un realloc? Un double pointeur?
- Est-ce que ce code compile même?

#### 🔴 Ligne 26: Logique inversée
```c
while (ft_strchr(stash, '\n') || read(fd, buffer, 0) > 0)
```
**Hmm, attends:**
- Je continue TANT QUE j'ai trouvé un '\n'?
- Ne devrais-je pas m'arrêter quand je trouve '\n'?
- La condition devrait être `!ft_strchr(stash, '\n')` non?
- Et `read(fd, buffer, 0)` ne lit jamais rien... c'est juste un check de validité

#### 🔴 Ligne 28: Read sans vérifier le retour
```c
read(fd, buffer, BUFFER_SIZE);
```
**Questions importantes:**
- Où je stocke le retour de read()?
- Comment je sais combien d'octets ont été lus?
- Que se passe-t-il si read() retourne -1 (erreur)?
- Que se passe-t-il si read() retourne 0 (EOF)?
- Mon buffer n'est pas null-terminé après le read, non?

#### 🔴 Ligne 29: ft_strlcat avec mauvais paramètre
```c
ft_strlcat(stash, buffer, len);
```
**Problèmes potentiels:**
- `len` est la longueur ACTUELLE de stash
- Mais ft_strlcat attend la taille TOTALE du buffer destination
- Est-ce que je ne risque pas un buffer overflow?
- Ai-je alloué assez de mémoire pour stash?

#### 🟡 Lignes 40-43: Boucle infinie?
```c
while (i != len)
{
    stash[i] = '\0';
}
```
**Attends une seconde:**
- Je mets stash[i] à '\0' mais je n'incrémente jamais i?
- C'est une boucle infinie non?
- Qu'est-ce que je voulais faire ici?

#### 🟡 Ligne 75: Calcul de taille
```c
line = malloc(sizeof(char) * len + 2);
```
**Parenthèses?**
- Est-ce que c'est `(sizeof(char) * len) + 2`?
- Ou `sizeof(char) * (len + 2)`?
- La priorité des opérateurs... ai-je raison?
- Pourquoi +2? (un pour '\n', un pour '\0'?)

#### 🟡 Ligne 80: Condition étrange
```c
if (read(fd, buffer, 0) > 1)
```
**Vraiment?**
- `read(fd, buffer, 0)` demande de lire 0 octets
- Ça ne peut JAMAIS retourner > 1
- Je voulais checker si EOF ou erreur?
- Quand est-ce que je dois free() le stash exactement?

#### ❌ Memory Management général:
**Questions cruciales:**
- Si malloc échoue ligne 64, je retourne NULL... mais ai-je libéré stash?
- Si malloc échoue ligne 75, je retourne NULL... ai-je libéré stash?
- Le stash est statique... comment je le libère à la fin du fichier?
- Que se passe-t-il si j'appelle get_next_line sur un fichier vide?
- Que se passe-t-il si fd est invalide (-1, 42000, etc.)?

---

### 📄 `get_next_line_utils.c`

#### 🟢 ft_strlen - LIGNE 13-23
**Semble OK mais:**
- Que se passe-t-il si `s` est NULL?
- Dois-je protéger contre ça?

#### 🚨 ft_strchr - LIGNES 25-44

**J'ai déjà mis des commentaires sur les bugs, mais rappelons:**

**Ligne 34: BUG ÉVIDENT**
```c
if (*s == (char)c)
```
- Je compare toujours le PREMIER caractère
- Je devrais comparer `s[i]`
- Ce bug rend la fonction TOTALEMENT cassée

**Ligne 39-40: Logique du '\0'**
```c
if ((char)c == '\0')
    return (0);
```
- Si je cherche '\0', je retourne 0 (début de string)
- Mais le '\0' est à la position `i` (fin de string)
- Est-ce le comportement que je veux pour mon GNL?

**Ligne 43: Valeur de retour "not found"**
```c
return (0);
```
- Je retourne 0 si pas trouvé
- Mais 0 c'est aussi un index valide!
- Comment je différencie "trouvé à l'index 0" de "pas trouvé"?
- Ma logique dans get_next_line.c va-t-elle planter à cause de ça?

**Type de retour:**
- Pourquoi je retourne `int` (position) au lieu de `char *` (pointeur)?
- Est-ce que c'est cohérent avec mon utilisation dans le code?
- Dans get_next_line.c ligne 26, je fais `ft_strchr(stash, '\n')` dans une condition
- En C, quelle valeur est "truthy"? 0 est false, donc si je retourne 0 = "pas trouvé", ma condition va s'arrêter... mais ma logique semble inversée!

#### 🟡 ft_strlcpy - LIGNES 46-62
**Questions:**
- Le paramètre `size` est en `int`, pas en `size_t` comme l'originale
- Est-ce voulu? Y a-t-il des cas où un int négatif poserait problème?
- Ma fonction gère-t-elle correctement un `dst` NULL?
- Ligne 55: si `size == 0`, je ne copie rien, mais je retourne quand même la longueur de src - est-ce correct?

#### 🟡 ft_strlcat - LIGNES 64-82
**Questions:**
- Encore une fois, `size` est un int
- Ligne 70-73: Si `size <= dst_len`, je retourne `size + src_len`
- Est-ce que j'ai bien compris la spec de strlcat?
- Comment je gère les buffers NULL?
- Est-ce que je l'utilise correctement dans mon get_next_line.c?

---

### 📄 Fichiers BONUS

**get_next_line_bonus.c, get_next_line_bonus.h, get_next_line_utils_bonus.c**

**PROBLÈME:** Tous vides!

**Réflexions:**
- Le bonus consiste à gérer plusieurs fd en même temps
- Je dois utiliser un tableau statique ou une liste chaînée?
- Dois-je faire `static char *stash[MAX_FD]` ou `static char *stash[1024]`?
- Comment je gère la libération mémoire pour plusieurs fd?
- Est-ce que je dois dupliquer tout le code ou puis-je partager les utils?

---

## 📊 Checklist de Vérification

### ✅ Norme & Style
- [ ] **Retirer le main() du fichier de rendu**
- [ ] **Retirer les includes <fcntl.h> et <stdio.h>**
- [ ] Passer la norminette sur tous les fichiers
- [ ] Vérifier que toutes les fonctions font moins de 25 lignes
- [ ] Vérifier les noms de variables (norminette compliant?)
- [ ] Pas de variables globales (sauf static dans les fonctions)

### 🔧 Logique & Algorithme
- [ ] Corriger la boucle infinie dans `clean_stash()` (ligne 40-43)
- [ ] Fixer la logique inversée dans `read_to_stash()` (ligne 26)
- [ ] Comprendre et refaire le double malloc ligne 71
- [ ] Revoir toute la logique de `read_to_stash()` - elle ne lit jamais vraiment?
- [ ] Corriger l'opérateur `|` en `||` ligne 60
- [ ] Vérifier le calcul de taille malloc ligne 75 (parenthèses!)
- [ ] Revoir la condition ligne 80 pour free le stash

### 🐛 Bugs Critiques
- [ ] **FIX ft_strchr bug ligne 34: `*s` → `s[i]`**
- [ ] Décider de la valeur de retour de ft_strchr si "pas trouvé" (-1? ou garder 0?)
- [ ] Vérifier que ft_strchr pour '\0' retourne la bonne valeur
- [ ] S'assurer que read() est appelé et son retour vérifié
- [ ] Null-terminer le buffer après chaque read()

### 💾 Memory Management
- [ ] Vérifier tous les chemins où malloc peut échouer
- [ ] M'assurer que je free() le stash dans tous les cas (erreur, EOF, etc.)
- [ ] Protéger contre les double-free
- [ ] Test avec valgrind: `valgrind --leak-check=full ./program`
- [ ] Que se passe-t-il si j'appelle GNL après avoir atteint EOF?

### 🧪 Edge Cases à Tester
- [ ] fd invalide (-1, -42, 10000)
- [ ] BUFFER_SIZE invalide (0, -1, 1, INT_MAX)
- [ ] Fichier vide
- [ ] Fichier sans '\n' (une seule ligne)
- [ ] Fichier avec plusieurs '\n' consécutifs
- [ ] Ligne plus longue que BUFFER_SIZE
- [ ] Ligne exactement de taille BUFFER_SIZE
- [ ] Malloc qui échoue (comment simuler?)
- [ ] Read qui retourne -1 (erreur I/O)
- [ ] EOF au milieu de la lecture

### 📦 Gestion du Stash
- [ ] Revoir toute la logique d'allocation dynamique du stash
- [ ] Comment je gère le redimensionnement si la ligne est très longue?
- [ ] Initialisation du stash: faut-il mettre stash[0] = '\0'?
- [ ] Dans clean_stash(), comment je décale correctement les données restantes?

### 🎯 Bonus (Multiple FD)
- [ ] Implémenter le système multi-fd
- [ ] Décider: tableau statique ou liste chaînée?
- [ ] Gérer la mémoire pour chaque fd indépendamment
- [ ] Tests: ouvrir 3 fichiers, lire en alternance

### 🚀 Tests Recommandés
```bash
# Compiler avec différentes BUFFER_SIZE
gcc -Wall -Wextra -Werror -D BUFFER_SIZE=1 ...
gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42 ...
gcc -Wall -Wextra -Werror -D BUFFER_SIZE=9999 ...

# Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./test

# Norminette
norminette *.c *.h

# Tests avec francinette ou gnlTester
```

---

## 🎓 Réflexions Finales

Bon, soyons honnêtes avec moi-même:

**Ce qui est bien:**
- J'ai compris le concept général (stash statique, lecture en boucle, extraction)
- Les fonctions utilitaires de base sont là
- La structure du header est propre

**Ce qui DOIT être fixé AVANT de rendre:**
1. **URGENCE ABSOLUE**: Le bug de ft_strchr ligne 34 qui casse TOUT
2. **URGENCE ABSOLUE**: Le double malloc() ligne 71 qui compile pas
3. **URGENCE ABSOLUE**: La boucle infinie ligne 40-43
4. **URGENCE ABSOLUE**: Retirer le main() et les includes de test
5. Revoir TOUTE la logique de read_to_stash() qui ne lit rien actuellement
6. Faire un vrai test avec valgrind

**Questions à me poser en fixant:**
- Est-ce que je comprends vraiment comment fonctionne le stash statique entre les appels?
- Ai-je besoin de strlcat ou puis-je faire plus simple?
- Ma gestion d'erreur est-elle complète?
- Comment je teste que mon code fonctionne sur des edge cases?

**Prochaines étapes:**
1. Fixer les bugs critiques un par un
2. Tester avec un petit main simple
3. Valgrind sur chaque test
4. Norminette
5. Tests avec différents BUFFER_SIZE
6. Implémenter le bonus seulement quand le mandatory est SOLIDE

---

**Courage! Le concept est là, il faut juste soigner l'implémentation. Chaque bug est une opportunité d'apprendre! 💪**
