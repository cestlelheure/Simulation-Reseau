#include "adresse.h"
#include <stdio.h>


void initIPv4(IPv4 *adresse){
    //à completer
}

void initMAC(IPv4 *adresse){
    //à completer
}

char afficherIPv4(const IPv4 *adresse){
    //à completer
}

char afficherMAC(const MAC *adresse){
    //à completer
}

char * IPv4_to_string(IPv4 ip, char * str_ipv4){
    if (!str_ipv4){
        return NULL;
    }
    snprintf(str_ipv4, 16, "%u.%u.%u.%u", ip.IP[0], ip.IP[1], ip.IP[2], ip.IP[3]);
    return str_ipv4;
}

char * MAC_to_string(MAC M, char * str_mac){
    if (!str_mac){ 
        return NULL;
    }
    snprintf(str_mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X", 
             M.MAC[0], M.MAC[1], M.MAC[2], M.MAC[3], M.MAC[4], M.MAC[5]);
    return str_mac;
}
