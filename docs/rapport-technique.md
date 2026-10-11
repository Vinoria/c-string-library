# Rapport technique — Bibliothèque de chaînes en C

## 1. Résumé du projet

Ce projet réalisé dans le cadre du module **PROG5** consiste à développer une abstraction de chaîne de caractères en C sans s'appuyer sur un type de haut niveau. Deux stratégies ont été mises en œuvre avec la même interface publique :

1. une version **immuable**, dans laquelle chaque transformation produit une nouvelle chaîne ;
2. une version **dynamique**, dans laquelle les transformations modifient une chaîne existante et réutilisent son espace mémoire.

Les deux variantes ont obtenu **100/100** lors de leur évaluation. Cette édition destinée au portfolio conserve leurs principes algorithmiques, clarifie le code et ajoute une suite de tests indépendante, une compilation stricte, des sanitizers et une intégration continue.

## 2. Objectifs techniques

Le travail répond à quatre objectifs principaux :

- gérer explicitement l'allocation, la réallocation et la libération de mémoire ;
- proposer des opérations usuelles sur les chaînes sans fuite ni accès hors limites ;
- comparer deux modèles de mutation et leurs effets sur les allocations ;
- garantir un comportement cohérent pour les positions négatives, les bornes dépassées et les longueurs invalides.

L'API comprend la création, la destruction, la lecture de la taille, la conversion vers une chaîne C, l'inversion, l'extraction, l'insertion, la suppression et le remplacement.

## 3. Représentation des données

### 3.1 Version immuable

```c
typedef struct {
    char *contenu;
    int taille;
} chaine_t;
```

Chaque valeur possède son propre tampon terminé par `\0`. Une transformation alloue une nouvelle structure et un nouveau tampon. La chaîne source reste donc inchangée, ce qui rend la propriété des valeurs simple à raisonner : l'appelant libère séparément chaque résultat.

### 3.2 Version dynamique

```c
typedef struct {
    char *contenu;
    int taille;
    int capacite;
} chaine_t;
```

La taille correspond au nombre de caractères utiles, tandis que la capacité représente le nombre total d'octets réservés, terminateur `\0` compris. Cette distinction permet de conserver de l'espace libre après une croissance et d'éviter une nouvelle allocation à chaque modification.

L'invariant central est le suivant :

```text
capacite >= taille + 1
contenu[taille] == '\0'
```

Les tests vérifient cet invariant après chaque transformation.

## 4. Normalisation des paramètres

Les deux variantes appliquent les mêmes règles :

- une position comprise entre `-taille` et `-1` est interprétée depuis la fin ;
- une position trop petite devient `0` ;
- une position supérieure à la taille devient `taille` ;
- une longueur négative ou supérieure à la partie disponible sélectionne tout le suffixe restant.

Deux fonctions internes centralisent ces règles. Cette factorisation supprime les duplications et garantit un comportement identique pour l'extraction, l'insertion, la suppression et le remplacement.

## 5. Algorithmes

### Création et libération

`ch_cree` alloue d'abord la structure, puis un tampon dimensionné pour le contenu et son terminateur. Si la seconde allocation échoue, la structure déjà créée est libérée avant de retourner `NULL`. `ch_libere` accepte également `NULL`, ce qui simplifie le nettoyage côté appelant.

### Inversion

L'algorithme échange les caractères symétriques jusqu'au milieu de la chaîne. La version immuable commence par dupliquer la valeur ; la version dynamique réalise les échanges directement dans le tampon existant.

### Sous-chaîne

La version immuable alloue exactement `longueur + 1` octets et copie la plage demandée. La version dynamique déplace cette plage au début du tampon. Elle réduit également l'allocation lorsque la capacité existante devient nettement supérieure au besoin.

### Insertion

La version immuable construit successivement le préfixe, le texte inséré et le suffixe dans un nouveau tampon. La version dynamique :

1. augmente la capacité si nécessaire ;
2. déplace le suffixe vers la droite avec `memmove` ;
3. copie le texte inséré dans l'espace libéré.

`memmove` est utilisé à la place de `memcpy` lorsque les zones peuvent se chevaucher, notamment lorsqu'une chaîne est insérée dans elle-même.

### Suppression

La partie située après la plage supprimée est replacée à la position de départ. Dans la version dynamique, le terminateur est déplacé avec le suffixe afin de conserver immédiatement une représentation C valide.

### Remplacement

Le remplacement combine suppression et insertion. Sa nouvelle taille est calculée avant toute modification :

```text
nouvelle_taille = ancienne_taille - longueur_supprimee + taille_remplacement
```

Le code vérifie les risques de dépassement de `int` avant le calcul et n'écrase le pointeur du tampon qu'après le succès de `realloc`.

## 6. Stratégie mémoire de la version dynamique

Lorsque le tampon est trop petit, sa capacité est doublée jusqu'à couvrir le besoin. Cette croissance géométrique réduit le nombre de réallocations lors d'une série d'insertions. Un garde-fou évite le dépassement lors du doublement et utilise directement la capacité requise près de `INT_MAX`.

À l'inverse, un tampon est réduit lorsque sa capacité dépasse deux fois la taille nécessaire. Cette règle limite le gaspillage de mémoire sans provoquer une réallocation après chaque petite suppression.

La stratégie produit donc un compromis entre :

- la mémoire immédiatement utilisée ;
- le nombre d'allocations ;
- le coût des copies lors de modifications successives.

## 7. Complexité

Soit `n` la taille de la chaîne principale, `m` celle de la chaîne ajoutée et `k` la taille extraite.

| Opération | Temps | Mémoire supplémentaire immuable | Mémoire supplémentaire dynamique |
|---|---:|---:|---:|
| Création | `O(n)` | `O(n)` | `O(n)` |
| Inversion | `O(n)` | `O(n)` | `O(1)` |
| Sous-chaîne | `O(k)` | `O(k)` | `O(k)` seulement en cas de réduction |
| Insertion | `O(n + m)` | `O(n + m)` | `O(n + m)` en cas de croissance, sinon `O(1)` hors tampon |
| Suppression | `O(n)` | `O(n)` | `O(n)` en cas de réduction, sinon `O(1)` hors tampon |
| Remplacement | `O(n + m)` | `O(n + m)` | dépend d'une croissance ou d'une réduction du tampon |

Même sans allocation, une insertion ou une suppression reste linéaire à cause du déplacement du suffixe. Le bénéfice de la version dynamique porte surtout sur le nombre d'allocations et la réutilisation du tampon.

## 8. Validation

La validation a été réalisée en plusieurs couches :

| Couche | Vérifications |
|---|---|
| Banc d'évaluation d'origine | scénarios fonctionnels 0 à 7, suivi des allocations et contrôle de la consommation mémoire |
| Tests indépendants | création, valeurs vides, positions négatives, bornes dépassées, insertion dans soi-même, croissance et réduction |
| Compilation stricte | C11, `-Wall`, `-Wextra`, `-Wpedantic`, `-Werror` |
| Analyse dynamique | AddressSanitizer et UndefinedBehaviorSanitizer |
| Intégration continue | GCC et Clang sur chaque push et pull request |

Les deux archives d'origine passent tous les scénarios 0 à 7 sans échec de contrôle ni erreur AddressSanitizer. La suite indépendante valide en plus les invariants de capacité de la version dynamique et vérifie explicitement que la version immuable ne modifie pas ses opérandes.

Le banc fourni dans le cadre du cours n'est pas publié : le dépôt ne contient que le code du projet et des tests écrits indépendamment pour cette édition.

## 9. Robustesse et limites

Les échecs d'allocation sont propagés par `NULL` et le pointeur d'origine n'est jamais perdu lors d'un `realloc`. Les tailles sont contrôlées avant les additions susceptibles de dépasser `INT_MAX`.

Le projet reste volontairement limité par son cadre pédagogique :

- il traite des octets et non des points de code Unicode ;
- la structure est visible dans l'en-tête, conformément à l'interface du devoir ;
- les fonctions ne sont pas conçues pour des accès concurrents ;
- `ch_vers_C` expose le tampon interne et suppose que l'appelant respecte le modèle de propriété ;
- le type `int` borne la taille maximale, là où une bibliothèque généraliste emploierait plutôt `size_t`.

## 10. Bilan

La comparaison met en évidence un choix de conception classique. L'immuabilité facilite le raisonnement sur les valeurs et évite les effets de bord, mais entraîne davantage d'allocations. La mutation en place demande des invariants plus stricts et une gestion précise des zones mémoire, mais réduit les allocations lors de modifications répétées.

Au-delà des opérations sur les chaînes, ce projet démontre la maîtrise de la propriété mémoire, des erreurs d'allocation, des copies chevauchantes, de la complexité algorithmique et de la validation automatisée en C.
