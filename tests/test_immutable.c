#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "chaines.h"

static void attend_chaine(chaine_t *chaine, const char *attendu)
{
    assert(chaine != NULL);
    assert(ch_taille(chaine) == (int)strlen(attendu));
    assert(strcmp(ch_vers_C(chaine), attendu) == 0);
}

static void test_creation(void)
{
    chaine_t *texte = ch_cree("bonjour");
    attend_chaine(texte, "bonjour");
    ch_libere(texte);

    chaine_t *vide = ch_cree(NULL);
    attend_chaine(vide, "");
    ch_libere(vide);
    ch_libere(NULL);
}

static void test_transformations(void)
{
    chaine_t *origine = ch_cree("abcdef");
    chaine_t *remplacement = ch_cree("XYZ");
    assert(origine != NULL);
    assert(remplacement != NULL);

    chaine_t *resultat = ch_renverse(origine);
    assert(resultat != origine);
    attend_chaine(resultat, "fedcba");
    ch_libere(resultat);

    resultat = ch_sous_chaine(origine, 2, 3);
    attend_chaine(resultat, "cde");
    ch_libere(resultat);

    resultat = ch_sous_chaine(origine, -2, -1);
    attend_chaine(resultat, "ef");
    ch_libere(resultat);

    resultat = ch_sous_chaine(origine, -100, 2);
    attend_chaine(resultat, "ab");
    ch_libere(resultat);

    resultat = ch_sous_chaine(origine, 100, 4);
    attend_chaine(resultat, "");
    ch_libere(resultat);

    resultat = ch_insere(origine, 2, remplacement);
    attend_chaine(resultat, "abXYZcdef");
    ch_libere(resultat);

    resultat = ch_insere(origine, -1, remplacement);
    attend_chaine(resultat, "abcdeXYZf");
    ch_libere(resultat);

    resultat = ch_insere(origine, 100, remplacement);
    attend_chaine(resultat, "abcdefXYZ");
    ch_libere(resultat);

    resultat = ch_supprime(origine, 2, 2);
    attend_chaine(resultat, "abef");
    ch_libere(resultat);

    resultat = ch_supprime(origine, -2, 1);
    attend_chaine(resultat, "abcdf");
    ch_libere(resultat);

    resultat = ch_supprime(origine, 3, -1);
    attend_chaine(resultat, "abc");
    ch_libere(resultat);

    resultat = ch_remplace(origine, 1, 3, remplacement);
    attend_chaine(resultat, "aXYZef");
    ch_libere(resultat);

    resultat = ch_remplace(origine, -2, 1, remplacement);
    attend_chaine(resultat, "abcdXYZf");
    ch_libere(resultat);

    resultat = ch_remplace(origine, 2, 2, NULL);
    attend_chaine(resultat, "abef");
    ch_libere(resultat);

    attend_chaine(origine, "abcdef");
    attend_chaine(remplacement, "XYZ");
    ch_libere(remplacement);
    ch_libere(origine);
}

static void test_valeurs_nulles(void)
{
    chaine_t *resultat = ch_renverse(NULL);
    attend_chaine(resultat, "");
    ch_libere(resultat);

    resultat = ch_sous_chaine(NULL, 4, 2);
    attend_chaine(resultat, "");
    ch_libere(resultat);
}

int main(void)
{
    test_creation();
    test_transformations();
    test_valeurs_nulles();
    puts("immutable: all tests passed");
    return 0;
}
