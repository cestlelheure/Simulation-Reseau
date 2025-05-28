#pragma once
#include "adresse.h"

typedef struct station {
    MAC mac;
    IPv4 ip;
} station_t;

void afficher_station(station_t s);
