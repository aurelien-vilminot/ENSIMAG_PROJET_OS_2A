#include "processus.h"
#include "stdio.h"
#include "tinyalloc.h"
#include "string.h"
#include "cpu.h"

#define TAILLE_NOM 20
#define NB_REGISTRES 5
#define TAILLE_PILE 512
#define TAILLE_TABLE 8

enum etat_processus {
    ELU,
    ACTIVABLE,
    ENDORMI
};

struct processus {
    int32_t pid;
    char nom_proc[TAILLE_NOM];
    enum etat_processus etat;
    int32_t zone_registre[NB_REGISTRES];
    int32_t pile_exec[TAILLE_PILE];
    int32_t heure_reveil;
    struct processus * suiv;
};

struct processus * table_processus[TAILLE_TABLE];
struct processus * tete_proc_activable;
struct processus * queue_proc_activable;
struct processus * tete_proc_endormi;
struct processus * queue_proc_endormi;
struct processus * processus_actif;
int32_t pid = 0;

void init_processus(void) {
    // Crée le processus 0 et le rend élu
    cree_processus(idle, "proc_0");
    table_processus[0]->etat = ELU;
    processus_actif = table_processus[0];

    cree_processus(proc1, "proc_1");
    cree_processus(proc2, "proc_2");
    cree_processus(proc3, "proc_3");
}

int32_t cree_processus(void (*code)(void), char *nom) {
    // Création du processus
    if (pid == TAILLE_TABLE) {
        return -1;
    }

    struct processus * processus = malloc(sizeof(struct processus));
    processus->pid = pid;
    strcpy(processus->nom_proc, nom);
    processus->etat = ACTIVABLE;
    processus->zone_registre[1] = (int32_t) &processus->pile_exec[TAILLE_PILE-1];
    processus->pile_exec[TAILLE_PILE - 1] = (int32_t) code;
    // Ajout du processus à la table des processus
    table_processus[pid] = processus;

    if (pid != 0) {
        insertion_proc_activable(processus);
    }

    return pid++;
}

struct processus * extraction_proc_activable() {
    if (tete_proc_activable == NULL) return NULL;
    struct processus* tete = tete_proc_activable;
    if (tete_proc_activable->suiv) {
        tete_proc_activable = tete->suiv;
    } else {
        tete_proc_activable = NULL;
        queue_proc_activable = NULL;
    }
    tete->etat = ELU;
    return tete;
}

void insertion_proc_activable(struct processus * proc) {
    if (tete_proc_activable == NULL) {
        tete_proc_activable = proc;
    } else {
        queue_proc_activable->suiv = proc;
    }
    proc->etat = ACTIVABLE;
    queue_proc_activable = proc;
}

void ordonnance(void) {
    struct processus * old = processus_actif;
    struct processus * new = extraction_proc_activable();
    // Pas de processus activable --> pas de changement de contexte
    if (new == NULL) {
        return;
    }
    processus_actif = new;
    insertion_proc_activable(old);
    ctx_sw(old->zone_registre, new->zone_registre);
}

void dors(uint32_t nbr_secs) {
    struct processus * processus = extraction_proc_activable();
    processus->etat = ENDORMI;
}

int32_t mon_pid(void) {
    return processus_actif->pid;
}

char *mon_nom(void) {
    return processus_actif->nom_proc;
}

void idle()
{
    for (;;) {
        sti();
        hlt();
        cli();
    }
}
void proc1(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
               mon_nom(), mon_pid());
        dors(2);
    }
}
void proc2(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
               mon_nom(), mon_pid());
        dors(3);
    }
}
void proc3(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
               mon_nom(), mon_pid());
        dors(5);
    }
}