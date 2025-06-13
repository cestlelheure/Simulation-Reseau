#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "adresse.h"
#include "switch.h"

#define TYPE_IPV4 0x0800
#define TYPE_ARP  0x0806
#define TYPE_IPV6 0x86DD
#define TYPE_STP  0x0027

/**
 * Structure représentant une trame Ethernet
 * Contient les champs standard d'une trame Ethernet
 */
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

/**
 * Initialise une trame avec les paramètres fournis
 * Alloue la mémoire pour les données si nécessaire
 */
bool init_trame(trame *t, MAC src, MAC dest, uint16_t type, const uint8_t *donnees, size_t taille_donnees);

/**
 * Libère la mémoire allouée pour une trame
 * Nettoie les ressources sans libérer la trame elle-même
 */
void deinit_trame(trame *t);

/**
 * Affiche le contenu d'une trame sur la sortie standard
 * Montre les adresses MAC, le type et les données
 */
void afficher_trame(const trame *t);

/**
 * Envoie une trame via un switch
 * Gère le broadcast et la recherche dans la table de commutation
 */
void envoyer_trame(trame *t, switch_t *sw);

/**
 * Envoie une trame sur un port spécifique
 * Vérifie le FCS avant l'envoi
 */
void envoyer_trame_sur_port(trame *t, int port);

/**
 * Calcule le FCS (Frame Check Sequence) d'une trame
 * Utilise l'algorithme CRC-32
 */
uint32_t calculer_fcs(const trame *t);

/**
 * Vérifie si une adresse MAC est une adresse de broadcast
 * Une adresse broadcast contient uniquement des 0xFF
 */
bool est_broadcast(MAC mac);