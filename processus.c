#include "processus.h"
#include "stdio.h"
#include "tinyalloc.h"
#include "string.h"
#include "cpu.h"
#include "time_manage.h"

struct processus * tete_proc_activable = NULL;
struct processus * queue_proc_activable = NULL;
struct processus * liste_proc_endormi = NULL;
struct processus * liste_proc_morts = NULL;
struct processus * processus_actif;
int32_t pid = 0;

void init_processus(void) {
    // Crée le processus 0 et le rend élu
    cree_processus(idle, "proc_0");
    table_processus[0]->etat = ELU;
    processus_actif = table_processus[0];

    cree_processus(proc1, "proc_1");
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
    if (tete_proc_activable->suiv == NULL) {
        queue_proc_activable = NULL;
    }
    struct processus* tete = tete_proc_activable;
    tete_proc_activable = tete_proc_activable->suiv;
    tete->etat = ELU;
    tete->suiv = NULL;
    return tete;
}

void insertion_proc_activable(struct processus * proc) {
    proc->suiv = NULL;
    if (tete_proc_activable == NULL) {
        tete_proc_activable = proc;
    } else {
        queue_proc_activable->suiv = proc;
    }
    proc->etat = ACTIVABLE;
    queue_proc_activable = proc;
}

void insertion_proc_endormis(struct processus * proc) {
    proc->etat = ENDORMI;
    if (liste_proc_endormi == NULL) {
        liste_proc_endormi = proc;
        proc->suiv = NULL;
    } else {
        struct processus * prec_proc = liste_proc_endormi;
        for (struct processus * current_proc = liste_proc_endormi ; current_proc != NULL ; current_proc = current_proc->suiv) {
            if (current_proc->heure_reveil >= proc->heure_reveil) {
                if (prec_proc == current_proc) {
                    proc->suiv = liste_proc_endormi;
                    liste_proc_endormi = proc;
                } else {
                    prec_proc->suiv = proc;
                    proc->suiv = current_proc;
                }
                return;
            }
            if (current_proc->suiv == NULL) {
                // Si le processus doit être inséré à la fin de la liste
                current_proc->suiv = proc;
                proc->suiv = NULL;
                return;
            }
            prec_proc = current_proc;
        }
    }
}

void insertion_proc_mort(struct processus * proc) {
    proc->etat = MOURANT;
    proc->suiv = liste_proc_morts;
    liste_proc_morts = proc;
}

void ordonnance(void) {
    reveil();
    suppression_processus();
    struct processus * old = processus_actif;
    struct processus * new = extraction_proc_activable();
    // Pas de processus activable --> pas de changement de contexte
    if (new == NULL) {
        return;
    }
    processus_actif = new;
    if (old->etat != ENDORMI) {
        insertion_proc_activable(old);
    }

    ctx_sw(old->zone_registre, new->zone_registre);
}

void reveil() {
    uint32_t current_time = nbr_secondes();
    while (liste_proc_endormi != NULL) {
        if (liste_proc_endormi->heure_reveil > current_time) {
            return;
        }
        struct processus * tmp = liste_proc_endormi;
        liste_proc_endormi = liste_proc_endormi->suiv;
        insertion_proc_activable(tmp);
    }
}

void dors(uint32_t nbr_secs) {
    processus_actif->heure_reveil = nbr_secondes() + nbr_secs;
    insertion_proc_endormis(processus_actif);
    ordonnance();
}

void fin_processus(void) {
    insertion_proc_mort(processus_actif);
    struct processus * old = processus_actif;
    struct processus * new = extraction_proc_activable();
    processus_actif = new;
    ctx_sw(old->zone_registre, new->zone_registre);
}

void suppression_processus() {
    struct processus * ptr = liste_proc_morts;
    while(ptr != NULL) {
        struct processus * tmp_to_suprr = ptr;
        ptr = ptr->suiv;
        free(tmp_to_suprr);
        tmp_to_suprr = NULL;
    }
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
    for (int32_t i = 0; i < 2; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
               mon_nom(), mon_pid());
        dors(2);
    }
    fin_processus();
}