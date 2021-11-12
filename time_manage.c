#include "time_manage.h"
#include "segment.h"
#include "cpu.h"
#include "console.h"
#include "stdio.h"
#include "processus.h"

#define CLOCKFREQ 50
#define QUARTZ 0x1234DD
#define ADR_VECTEUR_INT 0x1000
#define FLAG_FIN_IDT 0x8E00
#define PORT_DONNEES 0x21

uint64_t compteur = 0;
uint32_t time = 0;
uint32_t heures = 0;
uint32_t minutes = 0;
uint32_t secondes = 0;

void tic_PIT(void) {
    // Acquitter les interruptions
    outb(0x20, 0x20);

    // A chaque appel, le compteur est incrémenté
    ++compteur;

    if (compteur == CLOCKFREQ) {
        compteur = 0;
        ++secondes;
        time++;
        if (secondes == 60) {
            secondes = 0;
            ++minutes;
        }
        if (minutes == 60) {
            minutes = 0;
            ++heures;
        }
        if (heures == 24) {
            heures = 0;
        }
    }

    char heure_str[9];
    sprintf(heure_str, "%02u:%02u:%02u", heures, minutes, secondes);
    console_top_right(heure_str);
    console_top_left(table_processus);
    ordonnance();
}

void init_traitant_IT(int32_t num_IT, void (*traitant)(void)) {
    uint16_t *adresse = (uint16_t *) (0x1000 + (8 * num_IT));
    adresse[1] = KERNEL_CS;
    adresse[0] = (uint16_t) ((uint32_t) traitant & 0xFFFF);
    adresse[3] = (uint16_t) ((uint32_t) traitant >> 16);
    adresse[2] = FLAG_FIN_IDT;
}

void set_freq_prog() {
    outb(0x34, 0x43);
    outb((QUARTZ / CLOCKFREQ) % 256, 0x40);
    outb((QUARTZ / CLOCKFREQ) >> 8, 0x40);
}

void masque_IRQ(uint32_t num_IRQ, bool masque) {
    uint8_t valeur_masque = inb(PORT_DONNEES);
    if (masque) {
        // Forcer la valeur à 1
        valeur_masque = valeur_masque | (1 << num_IRQ);
    } else {
        // Forcer la valeur à 0
        valeur_masque = valeur_masque & (0xFF ^ (1 << num_IRQ));
    }
    outb(valeur_masque, PORT_DONNEES);
}

uint32_t nbr_secondes() {
    return time;
}