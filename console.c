#include <string.h>
#include "console.h"
#include "cpu.h"
#include "stdio.h"

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
uint32_t LIG_CURSEUR = 0;
uint32_t COL_CURSEUR = 0;
uint8_t COULEUR_TEXTE = 0x0F;

void modif_pos_curseur(uint32_t lig, uint32_t col) {
    if (col > NB_COL) {
        col = col % NB_COL;
        lig++;
    }

    if (lig >= NB_LIG) {
        lig = NB_LIG - 1;
        defilement();
    }

    LIG_CURSEUR = lig;
    COL_CURSEUR = col;
}

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
    modif_pos_curseur(lig, col);
    outb(COMMANDE_PARTIE_BASSE_CURSEUR, PORT_COMMANDE_CURSEUR);
    outb((uint8_t) position_curseur & 0xFF, PORT_DONNEES_CURSEUR);
    outb(COMMANDE_PARTIE_HAUTE_CURSEUR, PORT_COMMANDE_CURSEUR);
    outb((uint8_t) position_curseur >> 8, PORT_DONNEES_CURSEUR);
}

void traite_car(char c) {
    switch (c) {
        case 8:
            if (COL_CURSEUR > 0) {
                place_curseur(LIG_CURSEUR, COL_CURSEUR - 1);
            }
            break;
        case 9:
            place_curseur(LIG_CURSEUR, ((COL_CURSEUR + 8)/8)*8);
            break;
        case 10:
            place_curseur(LIG_CURSEUR + 1, 0);
            break;
        case 12:
            efface_ecran();
            place_curseur(0, 0);
            break;
        case 13:
            place_curseur(LIG_CURSEUR, 0);
            break;
        default:
            break;
    }

    if (31 < c && c < 127) {
        ecrit_car(LIG_CURSEUR, COL_CURSEUR, c, COULEUR_TEXTE);
        place_curseur(LIG_CURSEUR, COL_CURSEUR + 1);
    }
}

void defilement(void) {
    memmove(ptr_mem(0,0), ptr_mem(1,0), 2*((NB_LIG - 1) * NB_COL));
    for (uint32_t col = 0 ; col < NB_COL ; ++col) {
        ecrit_car(NB_LIG - 1, col, ' ', 0x0F);
    }
}

void console_top_right(const char *s) {
    size_t longueur_chaine = strlen(s);
    uint32_t sauvegarde_lig_curseur = LIG_CURSEUR;
    uint32_t sauvegarde_col_curseur = COL_CURSEUR;
    LIG_CURSEUR = 0;
    COL_CURSEUR = NB_COL - longueur_chaine;
    console_putbytes(s, (int) longueur_chaine);
    place_curseur(sauvegarde_lig_curseur, sauvegarde_col_curseur);
}

void console_top_left(struct processus * liste[]) {
    uint32_t sauvegarde_lig_curseur = LIG_CURSEUR;
    uint32_t sauvegarde_col_curseur = COL_CURSEUR;

    LIG_CURSEUR = 0;
    COL_CURSEUR = 0;

    for (uint32_t i = 0 ; liste[i] != NULL ; ++i) {
        char process_state[10];
        sprintf(process_state, "PID %u : %u", liste[i]->pid, liste[i]->etat);
        size_t longueur_chaine = strlen(process_state);
        console_putbytes(process_state, (int) longueur_chaine);
        LIG_CURSEUR++;
        COL_CURSEUR = 0;
        if (sauvegarde_lig_curseur == i) {
            sauvegarde_lig_curseur = LIG_CURSEUR;
        }
    }

    place_curseur(sauvegarde_lig_curseur, sauvegarde_col_curseur);
}