#pragma once

#include "adresse.h"
#include <stdbool.h>

// Structure pour une entrée de la table de commutation
typedef struct {
    mac_addr_t mac;
    int port;
} table_entree_t;

// Structure pour la table de commutation
typedef struct {
    table_entree_t *entrees;
    int taille;
    int capacite;
} table_commutation_t;

// Énumération pour l'état des ports STP
typedef enum {
    PORT_INCONNU = 0,
    PORT_RACINE,
    PORT_DESIGNE,
    PORT_BLOQUE
} etat_port_t;

typedef struct {
    etat_port_t etat;
    bool actif;
} port_info_t;

typedef struct {
    mac_addr_t mac;
    int nb_ports;
    int priorite;
    table_commutation_t table;
    port_info_t *ports;
} switch_t;

void init_table_commutation(table_commutation_t *table);
void deinit_table_commutation(table_commutation_t *table);
bool ajouter_entree_table(table_commutation_t *table, mac_addr_t mac, int port);
int chercher_port_mac(const table_commutation_t *table, mac_addr_t mac);
void vider_table_commutation(table_commutation_t *table);
void afficher_table_commutation(const table_commutation_t *table);

void init_switch(switch_t *sw);
void deinit_switch(switch_t *sw);
switch_t creer_switch(mac_addr_t mac, int nb_ports, int priorite);
void afficher_switch(const switch_t *sw);

void init_ports(switch_t *sw);
void set_etat_port(switch_t *sw, int port, etat_port_t etat);
etat_port_t get_etat_port(const switch_t *sw, int port);
void activer_port(switch_t *sw, int port);
void desactiver_port(switch_t *sw, int port);
bool port_est_actif(const switch_t *sw, int port);

bool switch_equals(const switch_t *s1, const switch_t *s2);
const char* etat_port_to_string(etat_port_t etat);