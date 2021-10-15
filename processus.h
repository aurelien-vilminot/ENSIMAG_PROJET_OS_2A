#ifndef SEPC_OS_PROJECT_PROCESSUS_H
#define SEPC_OS_PROJECT_PROCESSUS_H

#include "inttypes.h"

extern void init_processus(void);

extern void idle(void);

extern void proc1(void);

extern void ordonnance(void);

extern void ctx_sw(int * ancien_contexte, int * nouveau_contexte);

#endif //SEPC_OS_PROJECT_PROCESSUS_H
