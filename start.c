#include <cpu.h>
#include <inttypes.h>
#include "console.h"
#include "time_manage.h"


// on peut s'entrainer a utiliser GDB avec ce code de base
// par exemple afficher les valeurs de x, n et res avec la commande display

// une fonction bien connue
uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}

void kernel_start(void)
{
    // Effacer le contenu de l'écran
    traite_car(12);

    // Initialiser le traitant de l'interruption 32
    init_traitant_IT(32, traitant_IT_32);

    // Régler la fréquence de l'horloge
    set_freq_prog();

    // Démasquer les signaux de l'horloge
    masque_IRQ(0, (bool)0);

    // Démasquer des interruptions externes
    sti();
    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}

