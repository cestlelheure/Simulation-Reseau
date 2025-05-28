#include <stdio.h>
#include "station.h"

void afficher_station(station_t s) {
    printf("Station - MAC: ");
    afficher_mac(s.mac);
    printf(" | IP: ");
    afficher_ip(s.ip);
    printf("\n");
}
