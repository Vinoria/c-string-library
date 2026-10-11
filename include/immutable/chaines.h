#ifndef IMMUTABLE_CHAINES_H
#define IMMUTABLE_CHAINES_H

/**
 * Immutable string value used by the first implementation.
 *
 * Every transformation returns a newly allocated string and leaves its
 * operands unchanged.
 */
typedef struct {
    char *contenu;
    int taille;
} chaine_t;

chaine_t *ch_cree(char *s);
void ch_libere(chaine_t *ch);

int ch_taille(chaine_t *ch);
char *ch_vers_C(chaine_t *ch);

chaine_t *ch_renverse(chaine_t *a);
chaine_t *ch_sous_chaine(chaine_t *a, int position, int longueur);
chaine_t *ch_insere(chaine_t *a, int position, chaine_t *b);
chaine_t *ch_supprime(chaine_t *a, int position, int longueur);
chaine_t *ch_remplace(chaine_t *a, int position, int longueur, chaine_t *b);

#endif
