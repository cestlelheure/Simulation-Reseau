#include "adresse.h"

// Structure représentant une station du réseau
typedef struct {
    MAC mac;
    IPv4 ip;
} station_t;

// Fonction d'affichage
void afficher_station(station_t s);

