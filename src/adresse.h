#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    uint8_t octet[6];
} mac_addr_t;

typedef struct {
    uint8_t octet[4];
} ip_addr_t;

typedef mac_addr_t MAC;

void init_mac(mac_addr_t *mac);
void init_ip(ip_addr_t *ip);

mac_addr_t creer_mac(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5, uint8_t o6);
ip_addr_t creer_ip(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4);

void afficher_mac(mac_addr_t mac);
void afficher_ip(ip_addr_t ip);

char* mac_to_string(mac_addr_t mac, char *str_mac);
char* ip_to_string(ip_addr_t ip, char *str_ip);

bool parse_mac(const char *str, mac_addr_t *mac);
bool parse_ip(const char *str, ip_addr_t *ip);

bool mac_equals(mac_addr_t mac1, mac_addr_t mac2);
bool ip_equals(ip_addr_t ip1, ip_addr_t ip2);