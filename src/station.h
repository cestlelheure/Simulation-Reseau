#pragma once

#include "adresse.h"

typedef struct {
    mac_addr_t mac;
    ip_addr_t ip;
} station_t;

void init_station(station_t *station);
station_t creer_station(mac_addr_t mac, ip_addr_t ip);

void afficher_station(const station_t *station);

bool station_equals(const station_t *s1, const station_t *s2);
void copier_station(station_t *dest, const station_t *src);