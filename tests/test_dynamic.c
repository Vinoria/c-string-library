#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "chaines.h"

static void attend_chaine(chaine_t *chaine, const char *attendu)
{
    assert(chaine != NULL);
    assert(ch_taille(chaine) == (int)strlen(attendu));
    assert(strcmp(ch_vers_C(chaine), attendu) == 0);
    assert(chaine->capacite >= chaine->taille + 1);
    assert(chaine->contenu[chaine->taille] == '\0');
}

static void test_creation_et_inversion(void)
{
    chaine_t *texte = ch_cree("abcdef");
    attend_chaine(texte, "abcdef");
    assert(texte->capacite == 7);

    chaine_t *identite = texte;
    assert(ch_renverse(texte) == identite);
    attend_chaine(texte, "fedcba");
    ch_libere(texte);

    chaine_t *vide = ch_cree(NULL);
    attend_chaine(vide, "");
    assert(vide->capacite == 1);
    ch_libere(vide);
    ch_libere(NULL);
}

static void test_sous_chaines(void)
{
    chaine_t *texte = ch_cree("abcdef");
    chaine_t *identite = texte;
    int capacite_initiale = texte->capacite;

    assert(ch_sous_chaine(texte, 1, 4) == identite);
    attend_chaine(texte, "bcde");
    assert(texte->capacite == capacite_initiale);
    ch_libere(texte);

    texte = ch_cree("0123456789abcdefghijklmnop");
    identite = texte;
    assert(ch_sous_chaine(texte, 10, 3) == identite);
    attend_chaine(texte, "abc");
    assert(texte->capacite == 4);
    ch_libere(texte);
}

static void test_insertions(void)
{
    chaine_t *texte = ch_cree("abc");
    chaine_t *ajout = ch_cree("XYZ");
    chaine_t *identite = texte;

    assert(ch_insere(texte, 1, ajout) == identite);
    attend_chaine(texte, "aXYZbc");
    assert(texte->capacite == 8);
    attend_chaine(ajout, "XYZ");
    ch_libere(ajout);
    ch_libere(texte);

    texte = ch_cree("abcd");
    identite = texte;
    assert(ch_insere(texte, 2, texte) == identite);
    attend_chaine(texte, "ababcdcd");
    ch_libere(texte);
}

static void test_suppressions(void)
{
    chaine_t *texte = ch_cree("abcdefghijklmnop");
    chaine_t *identite = texte;

    assert(ch_supprime(texte, 1, 14) == identite);
    attend_chaine(texte, "ap");
    assert(texte->capacite == 3);
    ch_libere(texte);

    texte = ch_cree("abcdef");
    assert(ch_supprime(texte, -2, 1) == texte);
    attend_chaine(texte, "abcdf");
    ch_libere(texte);
}

static void test_remplacements(void)
{
    chaine_t *texte = ch_cree("abcdef");
    chaine_t *remplacement = ch_cree("XYZ");
    chaine_t *vide = ch_cree("");
    chaine_t *identite = texte;

    assert(ch_remplace(texte, 2, 2, remplacement) == identite);
    attend_chaine(texte, "abXYZef");
    assert(texte->capacite == 14);

    assert(ch_remplace(texte, 0, -1, vide) == identite);
    attend_chaine(texte, "");
    assert(texte->capacite == 1);

    ch_libere(vide);
    ch_libere(remplacement);
    ch_libere(texte);

    texte = ch_cree("abcdef");
    assert(ch_remplace(texte, 2, 2, texte) == texte);
    attend_chaine(texte, "ababcdefef");
    ch_libere(texte);

    texte = ch_cree("abcdef");
    assert(ch_remplace(texte, 2, 2, NULL) == texte);
    attend_chaine(texte, "abef");
    ch_libere(texte);
}

int main(void)
{
    test_creation_et_inversion();
    test_sous_chaines();
    test_insertions();
    test_suppressions();
    test_remplacements();
    puts("dynamic: all tests passed");
    return 0;
}
