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
#include "adresse.h"
#include <string.h>

void init_mac(mac_addr_t *mac) {
    if (mac == NULL) return;
    memset(mac->octet, 0, 6);
}

void init_ip(ip_addr_t *ip) {
    if (ip == NULL) return;
    memset(ip->octet, 0, 4);
}

mac_addr_t creer_mac(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5, uint8_t o6) {
    mac_addr_t mac;
    mac.octet[0] = o1;
    mac.octet[1] = o2;
    mac.octet[2] = o3;
    mac.octet[3] = o4;
    mac.octet[4] = o5;
    mac.octet[5] = o6;
    return mac;
}

ip_addr_t creer_ip(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4) {
    ip_addr_t ip;
    ip.octet[0] = o1;
    ip.octet[1] = o2;
    ip.octet[2] = o3;
    ip.octet[3] = o4;
    return ip;
}

void afficher_mac(mac_addr_t mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x", 
           mac.octet[0], mac.octet[1], mac.octet[2], 
           mac.octet[3], mac.octet[4], mac.octet[5]);
}

void afficher_ip(ip_addr_t ip) {
    printf("%u.%u.%u.%u", 
           ip.octet[0], ip.octet[1], ip.octet[2], ip.octet[3]);
}

char* mac_to_string(mac_addr_t mac, char *str_mac) {
    if (str_mac == NULL) return NULL;
    
    sprintf(str_mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
            mac.octet[0], mac.octet[1], mac.octet[2], 
            mac.octet[3], mac.octet[4], mac.octet[5]);
    return str_mac;
}

char* ip_to_string(ip_addr_t ip, char *str_ip) {
    if (str_ip == NULL) return NULL;
    
    sprintf(str_ip, "%u.%u.%u.%u", 
            ip.octet[0], ip.octet[1], ip.octet[2], ip.octet[3]);
    return str_ip;
}

bool parse_mac(const char *str, mac_addr_t *mac) {
    if (str == NULL || mac == NULL) return false;
    
    unsigned int octets[6];
    int result = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
                       &octets[0], &octets[1], &octets[2],
                       &octets[3], &octets[4], &octets[5]);
    
    if (result != 6) return false;
    
    for (int i = 0; i < 6; i++) {
        if (octets[i] > 255) return false;
        mac->octet[i] = (uint8_t)octets[i];
    }
    
    return true;
}

bool parse_ip(const char *str, ip_addr_t *ip) {
    if (str == NULL || ip == NULL) return false;
    
    unsigned int octets[4];
    int result = sscanf(str, "%u.%u.%u.%u",
                       &octets[0], &octets[1], &octets[2], &octets[3]);
    
    if (result != 4) return false;
    
    for (int i = 0; i < 4; i++) {
        if (octets[i] > 255) return false;
        ip->octet[i] = (uint8_t)octets[i];
    }
    
    return true;
}

bool mac_equals(mac_addr_t mac1, mac_addr_t mac2) {
    return memcmp(mac1.octet, mac2.octet, 6) == 0;
}

bool ip_equals(ip_addr_t ip1, ip_addr_t ip2) {
    return memcmp(ip1.octet, ip2.octet, 4) == 0;
}