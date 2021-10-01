#ifndef SEPC_OS_PROJECT_TIME_MANAGE_H
#define SEPC_OS_PROJECT_TIME_MANAGE_H

#include "inttypes.h"
#include "stdbool.h"

void tic_PIT(void);

void init_traitant_IT(int32_t num_IT, void (*traitant)(void));

extern void traitant_IT_32();

void set_freq_prog();

void masque_IRQ(uint32_t num_IRQ, bool masque);

#endif //SEPC_OS_PROJECT_TIME_MANAGE_H
