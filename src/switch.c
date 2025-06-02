#include "trame.h"
#include <stdio.h>
#include <string.h>

bool init_trame(trame *t, MAC src, MAC dest, uint16_t type, const uint8_t *donnees, size_t taille_donnees){
    if(t == NULL || donnees == NULL){
        return false;
    }
    t->source = src;
    t->destination = dest;
    t->type = type;
    t->donnees = donnees;
    t->taille_donnees = taille_donnees;
    return true;
}

void afficher_trame(const trame *t){
    if(t == NULL){
        return;
    }
    char str_mac[18];
    printf("Trame: %s\n", t->donnees);
    MAC_to_string(t->source, str_mac);
    printf("Source: %s\n", str_mac);
    MAC_to_string(t->destination, str_mac);
    printf("Destination: %s\n", str_mac);
    printf("Type: %d\n", t->type);
    printf("Taille: %zu\n", t->taille_donnees);
    printf("FCS: %d\n", t->fcs);
}

void deinit_trame(trame *t){
    if(t == NULL){
        return;
    }
    free(t->donnees);
    t->donnees = NULL;
    t->taille_donnees = 0;
}
