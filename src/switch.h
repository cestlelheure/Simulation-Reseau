#pragma once
#include "adresse.h"

typedef struct table_entree {
    MAC mac;
    int port;
} table_entree_t;

typedef struct table_commutation {
    table_entree_t* entrees;
    int taille;
} table_commutation_t;

typedef struct switch_ethernet {
    MAC mac;
    int nb_ports;
    int priorite;
    table_commutation_t table;
} switch_t;

void afficher_switch(switch_t s);
void afficher_table(table_commutation_t table);
