#pragma once

#include <stdlib.h>
#include <stdbool.h>


typedef struct IPv4{
    unsigned char IP[4];
}IPv4;

typedef struct MAC{
    unsigned char MAC[6];
}MAC;


static const size_t UNKNOWN_INDEX = -1;

//fonctions

void init_IPv4(IPv4 *adresse);
void init_MAC(IPv4 *adresse);
char * mac_to_string(MAC M, char * str_mac);

char afficher_IPv4(const IPv4 *adresse);
char afficher_MAC(const MAC *adresse);
char * IPv4_To_String(MAC M, char * str_ipv4);


//peut etre utile : 
//MAC_to_binary
//IPv4_to_binary