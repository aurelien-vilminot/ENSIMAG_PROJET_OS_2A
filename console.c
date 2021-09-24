#include <string.h>

#include "console.h"
#include "cpu.h"

/*
 * Définition des constantes
 */
#define NB_LIG 25
#define NB_COL 80
#define COMMANDE_PARTIE_BASSE_CURSEUR 0x0F
#define COMMANDE_PARTIE_HAUTE_CURSEUR 0x0E
#define PORT_COMMANDE_CURSEUR 0x3D4
#define PORT_DONNEES_CURSEUR 0x3D5
#define ADR_MEM_VIDEO 0xB8000

/*
 * Définition des variables globales
 */
uint32_t LIG_CURSEUR = 0
uint32_t COL_CURSEUR = 0;
uint8_t COULEUR_TEXTE = 0x0F;

void console_putbytes(const char *s, int len) {
    for (uint32_t caractere = 0 ; caractere < len; ++caractere) {
        traite_car(s[caractere]);
    }
}

uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
    return (uint16_t *) (ADR_MEM_VIDEO + 2*(lig*NB_COL + col));
}

void ecrit_car(uint32_t lig, uint32_t col, char c, uint8_t color) {
    uint8_t * char_ptr = (uint8_t *) ptr_mem(lig, col);
    char_ptr[0] = c;
    char_ptr[1] = color;
}

void efface_ecran(void) {
    for (uint32_t lig = 0 ; lig < NB_LIG ; ++lig) {
        for (uint32_t col = 0 ; col < NB_COL ; ++col) {
            ecrit_car(lig, col, ' ', 0x0F);
        }
    }
}

void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t position_curseur = col + lig * NB_COL;
    LIG_CURSEUR = lig;
    COL_CURSEUR = col;
    outb(COMMANDE_PARTIE_BASSE_CURSEUR, PORT_COMMANDE_CURSEUR);
    outb((uint8_t) position_curseur & 0xFF, PORT_DONNEES_CURSEUR);
    outb(COMMANDE_PARTIE_HAUTE_CURSEUR, PORT_COMMANDE_CURSEUR);
    outb((uint8_t) position_curseur >> 8, PORT_DONNEES_CURSEUR);
}

void traite_car(char c) {
    switch (c) {
        case 8:
            if (COL_CURSEUR > 0) {
                place_curseur(LIG_CURSEUR, --COL_CURSEUR);
            }
            break;
        case 9:
            COL_CURSEUR = ((COL_CURSEUR + 8)/8)*8;
            place_curseur(LIG_CURSEUR, COL_CURSEUR);
            break;
        case 10:
            COL_CURSEUR = 0;
            place_curseur(++LIG_CURSEUR, COL_CURSEUR);
            break;
        case 12:
            efface_ecran();
            LIG_CURSEUR = 0;
            COL_CURSEUR = 0;
            place_curseur(LIG_CURSEUR, COL_CURSEUR);
            break;
        case 13:
            COL_CURSEUR = 0;
            place_curseur(LIG_CURSEUR, COL_CURSEUR);
            break;
    }

    if (31 < c && c < 127) {
        ecrit_car(LIG_CURSEUR, COL_CURSEUR, c, COULEUR_TEXTE);
        place_curseur(LIG_CURSEUR, ++COL_CURSEUR);
    }
}

void defilement(void) {
    memmove(ptr_mem(0,0), ptr_mem(1,0), 2*((NB_LIG - 1) * NB_COL));
    for (uint32_t col = 0 ; col < NB_COL ; ++col) {
        ecrit_car(NB_LIG - 1, col, ' ', 0x0F);
    }
}