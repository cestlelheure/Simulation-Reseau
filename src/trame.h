#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "adresse.h"

typedef struct {
    uint8_t preambule[7];
    uint8_t sfd;
    MAC destination;
    MAC source;
    uint16_t type;
    uint8_t *donnees;
    size_t taille_donnees;
    uint32_t fcs;
} trame;


bool init_trame(trame *t, MAC src, MAC dest, uint16_t type, const uint8_t *donnees, size_t taille_donnees);
void deinit_trame(trame *t);
