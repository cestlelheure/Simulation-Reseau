#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "station.h"
#include "switch.h"
#include "graphe.h"

/**
 * Structure pour stocker la configuration réseau chargée
 */
typedef struct {
    switch_t *switches;
    station_t *stations;
    int nb_switches;
    int nb_stations;
    graphe *g;
} configuration_reseau_t;


int charger_configuration(const char *nom_fichier, graphe *g);
int charger_configuration_complete(const char *nom_fichier, configuration_reseau_t *config);
void liberer_configuration(configuration_reseau_t *config);

#endif