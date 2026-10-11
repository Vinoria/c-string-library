#ifndef DYNAMIC_CHAINES_H
#define DYNAMIC_CHAINES_H

/**
 * Mutable string backed by a dynamically sized character buffer.
 *
 * Transformations update the supplied value in place. Capacity includes the
 * byte reserved for the terminating null character.
 */
typedef struct {
    char *contenu;
    int taille;
    int capacite;
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
