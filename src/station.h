#include "adresse.h"

// Structure représentant une station du réseau
typedef struct {
    mac_addr_t mac;
    ip_addr_t ip;
} station_t;

// Fonction d'affichage
void afficher_station(station_t s);

