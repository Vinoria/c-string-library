#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "chaines.h"

static chaine_t *alloue_chaine(int taille)
{
    chaine_t *chaine = malloc(sizeof *chaine);
    if (chaine == NULL) {
        return NULL;
    }

    chaine->contenu = malloc((size_t)taille + 1U);
    if (chaine->contenu == NULL) {
        free(chaine);
        return NULL;
    }

    chaine->taille = taille;
    chaine->contenu[taille] = '\0';
    return chaine;
}

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

chaine_t *ch_cree(char *s)
{
    const char *source = s == NULL ? "" : s;
    size_t taille = strlen(source);

    if (taille > (size_t)INT_MAX) {
        return NULL;
    }

    chaine_t *chaine = alloue_chaine((int)taille);
    if (chaine == NULL) {
        return NULL;
    }

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
    chaine_t *resultat = ch_cree(ch_vers_C(a));
    if (resultat == NULL) {
        return NULL;
    }

    for (int i = 0; i < resultat->taille / 2; ++i) {
        char temporaire = resultat->contenu[i];
        resultat->contenu[i] = resultat->contenu[resultat->taille - 1 - i];
        resultat->contenu[resultat->taille - 1 - i] = temporaire;
    }

    return resultat;
}

chaine_t *ch_sous_chaine(chaine_t *a, int position, int longueur)
{
    int taille = ch_taille(a);
    position = normalise_position(position, taille);
    longueur = normalise_longueur(longueur, taille - position);

    chaine_t *resultat = alloue_chaine(longueur);
    if (resultat == NULL) {
        return NULL;
    }

    if (longueur > 0) {
        memcpy(resultat->contenu, ch_vers_C(a) + position, (size_t)longueur);
    }
    return resultat;
}

chaine_t *ch_insere(chaine_t *a, int position, chaine_t *b)
{
    int taille_a = ch_taille(a);
    int taille_b = ch_taille(b);
    position = normalise_position(position, taille_a);

    if (taille_b > INT_MAX - taille_a) {
        return NULL;
    }

    int nouvelle_taille = taille_a + taille_b;
    chaine_t *resultat = alloue_chaine(nouvelle_taille);
    if (resultat == NULL) {
        return NULL;
    }

    memcpy(resultat->contenu, ch_vers_C(a), (size_t)position);
    memcpy(resultat->contenu + position, ch_vers_C(b), (size_t)taille_b);
    memcpy(resultat->contenu + position + taille_b,
           ch_vers_C(a) + position,
           (size_t)(taille_a - position));

    return resultat;
}

chaine_t *ch_supprime(chaine_t *a, int position, int longueur)
{
    int taille = ch_taille(a);
    position = normalise_position(position, taille);
    longueur = normalise_longueur(longueur, taille - position);

    int nouvelle_taille = taille - longueur;
    chaine_t *resultat = alloue_chaine(nouvelle_taille);
    if (resultat == NULL) {
        return NULL;
    }

    memcpy(resultat->contenu, ch_vers_C(a), (size_t)position);
    memcpy(resultat->contenu + position,
           ch_vers_C(a) + position + longueur,
           (size_t)(taille - position - longueur));

    return resultat;
}

chaine_t *ch_remplace(chaine_t *a, int position, int longueur, chaine_t *b)
{
    int taille_a = ch_taille(a);
    int taille_b = ch_taille(b);
    position = normalise_position(position, taille_a);
    longueur = normalise_longueur(longueur, taille_a - position);

    int taille_conservee = taille_a - longueur;
    if (taille_b > INT_MAX - taille_conservee) {
        return NULL;
    }

    int nouvelle_taille = taille_conservee + taille_b;
    chaine_t *resultat = alloue_chaine(nouvelle_taille);
    if (resultat == NULL) {
        return NULL;
    }

    memcpy(resultat->contenu, ch_vers_C(a), (size_t)position);
    memcpy(resultat->contenu + position, ch_vers_C(b), (size_t)taille_b);
    memcpy(resultat->contenu + position + taille_b,
           ch_vers_C(a) + position + longueur,
           (size_t)(taille_a - position - longueur));

    return resultat;
}
