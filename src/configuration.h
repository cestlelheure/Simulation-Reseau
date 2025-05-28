#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "station.h"
#include "switch.h"
#include "graphe.h"

#define MAX_LIGNE 256

int charger_configuration(const char *nom_fichier, graphe *g);

#endif