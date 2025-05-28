#include "adresse.h"

typedef struct {
    mac_addr_t mac;
    int port;
} table_entree_t;

typedef struct {
    table_entree_t* entrees;
    int taille;
} table_commutation_t;

typedef struct {
    mac_addr_t mac;
    int nb_ports;
    int priorite;
    table_commutation_t table;
} switch_t;

void afficher_switch(switch_t s);
void afficher_table(table_commutation_t table);

#endif
