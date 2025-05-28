#include <stdio.h>
#include <stdlib.h>
#include "switch.h"

void afficher_table(table_commutation_t table) {
    printf("Table de commutation (%d entrées):\n", table.taille);
    for (int i = 0; i < table.taille; i++) {
        printf("  - MAC: ");
        afficher_mac(table.entrees[i].mac);
        printf(" -> Port: %d\n", table.entrees[i].port);
    }
}

void afficher_switch(switch_t s) {
    printf("Switch - MAC: ");
    afficher_mac(s.mac);
    printf(" | Ports: %d | Priorité: %d\n", s.nb_ports, s.priorite);
    afficher_table(s.table);
}
