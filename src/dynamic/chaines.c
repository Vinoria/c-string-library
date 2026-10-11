#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "chaines.h"

static int normalise_position(int position, int taille)
{
    if (position < -taille) {
        return 0;
    }
    if (position < 0) {
        return position + taille;
    }
    if (position > taille) {
        return taille;
    }
    return position;
}

static int normalise_longueur(int longueur, int disponible)
{
    if (longueur < 0 || longueur > disponible) {
        return disponible;
    }
    return longueur;
}

static int assure_capacite(chaine_t *chaine, int besoin)
{
    if (besoin <= chaine->capacite) {
        return 1;
    }

    int nouvelle_capacite = chaine->capacite;
    while (nouvelle_capacite < besoin) {
        if (nouvelle_capacite > INT_MAX / 2) {
            nouvelle_capacite = besoin;
            break;
        }
        nouvelle_capacite *= 2;
    }

    char *nouveau = realloc(chaine->contenu, (size_t)nouvelle_capacite);
    if (nouveau == NULL) {
        return 0;
    }

    chaine->contenu = nouveau;
    chaine->capacite = nouvelle_capacite;
    return 1;
}

static int doit_reduire(const chaine_t *chaine, int besoin)
{
    return (size_t)chaine->capacite > 2U * (size_t)besoin;
}

chaine_t *ch_cree(char *s)
{
    const char *source = s == NULL ? "" : s;
    size_t taille = strlen(source);

    if (taille >= (size_t)INT_MAX) {
        return NULL;
    }

    chaine_t *chaine = malloc(sizeof *chaine);
    if (chaine == NULL) {
        return NULL;
    }

    chaine->capacite = (int)taille + 1;
    chaine->contenu = malloc((size_t)chaine->capacite);
    if (chaine->contenu == NULL) {
        free(chaine);
        return NULL;
    }

    chaine->taille = (int)taille;
    memcpy(chaine->contenu, source, taille + 1U);
    return chaine;
}

void ch_libere(chaine_t *s)
{
    if (s == NULL) {
        return;
    }

    free(s->contenu);
    free(s);
}

int ch_taille(chaine_t *s)
{
    return s == NULL ? 0 : s->taille;
}

char *ch_vers_C(chaine_t *s)
{
    return s == NULL ? "" : s->contenu;
}

chaine_t *ch_renverse(chaine_t *a)
{
    if (a == NULL) {
        return NULL;
    }

    for (int i = 0; i < a->taille / 2; ++i) {
        char temporaire = a->contenu[i];
        a->contenu[i] = a->contenu[a->taille - 1 - i];
        a->contenu[a->taille - 1 - i] = temporaire;
    }

    return a;
}

chaine_t *ch_sous_chaine(chaine_t *a, int position, int longueur)
{
    if (a == NULL) {
        return NULL;
    }

    position = normalise_position(position, a->taille);
    longueur = normalise_longueur(longueur, a->taille - position);
    int besoin = longueur + 1;

    if (doit_reduire(a, besoin)) {
        char *nouveau = malloc((size_t)besoin);
        if (nouveau == NULL) {
            return NULL;
        }

        memcpy(nouveau, a->contenu + position, (size_t)longueur);
        nouveau[longueur] = '\0';
        free(a->contenu);
        a->contenu = nouveau;
        a->capacite = besoin;
    } else {
        memmove(a->contenu, a->contenu + position, (size_t)longueur);
        a->contenu[longueur] = '\0';
    }

    a->taille = longueur;
    return a;
}

chaine_t *ch_insere(chaine_t *a, int position, chaine_t *b)
{
    if (a == NULL) {
        return NULL;
    }

    int taille_b = ch_taille(b);
    position = normalise_position(position, a->taille);

    if (taille_b > INT_MAX - 1 - a->taille) {
        return NULL;
    }

    int ancienne_taille = a->taille;
    int nouvelle_taille = ancienne_taille + taille_b;
    if (!assure_capacite(a, nouvelle_taille + 1)) {
        return NULL;
    }

    const char *source_b = ch_vers_C(b);
    memmove(a->contenu + position + taille_b,
            a->contenu + position,
            (size_t)(ancienne_taille - position + 1));
    memmove(a->contenu + position, source_b, (size_t)taille_b);

    a->taille = nouvelle_taille;
    return a;
}

chaine_t *ch_supprime(chaine_t *a, int position, int longueur)
{
    if (a == NULL) {
        return NULL;
    }

    int ancienne_taille = a->taille;
    position = normalise_position(position, ancienne_taille);
    longueur = normalise_longueur(longueur, ancienne_taille - position);

    int nouvelle_taille = ancienne_taille - longueur;
    int besoin = nouvelle_taille + 1;

    if (doit_reduire(a, besoin)) {
        char *nouveau = malloc((size_t)besoin);
        if (nouveau == NULL) {
            return NULL;
        }

        memcpy(nouveau, a->contenu, (size_t)position);
        memcpy(nouveau + position,
               a->contenu + position + longueur,
               (size_t)(ancienne_taille - position - longueur));
        nouveau[nouvelle_taille] = '\0';

        free(a->contenu);
        a->contenu = nouveau;
        a->capacite = besoin;
    } else {
        memmove(a->contenu + position,
                a->contenu + position + longueur,
                (size_t)(ancienne_taille - position - longueur + 1));
    }

    a->taille = nouvelle_taille;
    return a;
}

chaine_t *ch_remplace(chaine_t *a, int position, int longueur, chaine_t *b)
{
    if (a == NULL) {
        return NULL;
    }

    int ancienne_taille = a->taille;
    int taille_b = ch_taille(b);
    position = normalise_position(position, ancienne_taille);
    longueur = normalise_longueur(longueur, ancienne_taille - position);

    int taille_conservee = ancienne_taille - longueur;
    if (taille_b > INT_MAX - 1 - taille_conservee) {
        return NULL;
    }

    int nouvelle_taille = taille_conservee + taille_b;
    int besoin = nouvelle_taille + 1;

    if (!assure_capacite(a, besoin)) {
        return NULL;
    }

    const char *source_b = ch_vers_C(b);
    if (doit_reduire(a, besoin)) {
        char *nouveau = malloc((size_t)besoin);
        if (nouveau == NULL) {
            return NULL;
        }

        memcpy(nouveau, a->contenu, (size_t)position);
        memcpy(nouveau + position, source_b, (size_t)taille_b);
        memcpy(nouveau + position + taille_b,
               a->contenu + position + longueur,
               (size_t)(ancienne_taille - position - longueur));
        nouveau[nouvelle_taille] = '\0';

        free(a->contenu);
        a->contenu = nouveau;
        a->capacite = besoin;
    } else {
        memmove(a->contenu + position + taille_b,
                a->contenu + position + longueur,
                (size_t)(ancienne_taille - position - longueur + 1));
        memmove(a->contenu + position, source_b, (size_t)taille_b);
    }

    a->taille = nouvelle_taille;
    return a;
}
