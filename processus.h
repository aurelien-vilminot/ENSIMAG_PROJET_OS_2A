#ifndef SEPC_OS_PROJECT_PROCESSUS_H
#define SEPC_OS_PROJECT_PROCESSUS_H

#include "inttypes.h"

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

#endif //SEPC_OS_PROJECT_PROCESSUS_H
