#ifndef SEPC_OS_PROJECT_PROCESSUS_H
#define SEPC_OS_PROJECT_PROCESSUS_H

#include "inttypes.h"

#define TAILLE_NOM 20
#define NB_REGISTRES 5
#define TAILLE_PILE 512
#define TAILLE_TABLE 8

enum etat_processus {
    ELU,
    ACTIVABLE,
    ENDORMI,
    MOURANT
};

struct processus {
    int32_t pid;
    char nom_proc[TAILLE_NOM];
    enum etat_processus etat;
    int32_t zone_registre[NB_REGISTRES];
    int32_t pile_exec[TAILLE_PILE];
    uint32_t heure_reveil;
    struct processus * suiv;
};

struct processus * table_processus[TAILLE_TABLE];

extern void init_processus(void);

int32_t cree_processus(void (*code)(void), char *nom);

extern struct processus * extraction_proc_activable();

extern void insertion_proc_activable(struct processus * proc);

extern void idle(void);

extern void proc1(void);

extern void proc2(void);

extern void proc3(void);

extern void ordonnance(void);

extern void ctx_sw(int * ancien_contexte, int * nouveau_contexte);

void reveil();

void suppression_processus();

#endif //SEPC_OS_PROJECT_PROCESSUS_H
