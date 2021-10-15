#include "processus.h"
#include "stdio.h"
#include "cpu.h"
#include "string.h"

#define TAILLE_NOM 20
#define NB_REGISTRES 5
#define TAILLE_PILE 512
#define TAILLE_TABLE 2

enum etat_processus {
    ELU,
    ACTIVABLE
};

struct processus {
    int32_t pid;
    char nom_proc[TAILLE_NOM];
    enum etat_processus etat;
    int32_t zone_registre[NB_REGISTRES];
    int32_t pile_exec[TAILLE_PILE];
};

struct processus table_processus[TAILLE_TABLE];
int32_t indice_table_processus_actif;

void init_processus(void) {
    // Création du processus 0
    struct processus processus_0;
    processus_0.pid = 0;
    strcpy(processus_0.nom_proc, "chef");
    processus_0.etat = ELU;

    // Ajout du processus 0 à la table des processus
    table_processus[0] = processus_0;

    // Création du processus 1
    struct processus processus_1;
    processus_1.pid = 15;
    strcpy(processus_1.nom_proc, "sous-chef");
    processus_1.etat = ACTIVABLE;
    processus_1.zone_registre[1] = (int32_t) &processus_1.pile_exec[TAILLE_PILE-1];
    processus_1.pile_exec[TAILLE_PILE - 1] = (int32_t) proc1;

    // Ajout du processus 1 à la table des processus
    table_processus[1] = processus_1;
}

void ordonnance(void) {
    uint32_t indice_table_ancien_processus_actif = indice_table_processus_actif;
    indice_table_processus_actif = (indice_table_processus_actif + 1) % TAILLE_TABLE;
    table_processus[indice_table_ancien_processus_actif].etat = ACTIVABLE;
    table_processus[indice_table_processus_actif].etat = ELU;
    ctx_sw(table_processus[indice_table_ancien_processus_actif].zone_registre, table_processus[indice_table_processus_actif].zone_registre);
}

int32_t mon_pid(void) {
    return table_processus[indice_table_processus_actif].pid;
}

char *mon_nom(void) {
    return table_processus[indice_table_processus_actif].nom_proc;
}

void idle(void)
{
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}
void proc1(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}