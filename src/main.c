#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trame.h"

// Définition de MAC si non définie dans trame.h
#ifndef MAC
typedef struct MAC {
    uint8_t octet[6];
} MAC;
#endif

MAC creer_mac(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5, uint8_t o6) {
    MAC mac;
    mac.octet[0] = o1;
    mac.octet[1] = o2;
    mac.octet[2] = o3;
    mac.octet[3] = o4;
    mac.octet[4] = o5;
    mac.octet[5] = o6;
    return mac;
}

void afficher_mac(MAC mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x", 
           mac.octet[0], mac.octet[1], mac.octet[2], 
           mac.octet[3], mac.octet[4], mac.octet[5]);
}

void afficher_trame(const trame *t) {
    if (t == NULL) {
        printf("Trame invalide\n");
        return;
    }
    
    printf("======= DÉTAILS DE LA TRAME =======\n");
    printf("Source      : ");
    afficher_mac(t->source);
    printf(" (");
    
    // On affiche le nom de la station en fonction de la MAC
    if (t->source.octet[0] == 0x11 && t->source.octet[1] == 0x22) {
        if (t->source.octet[5] == 0x01) printf("Homme de Fer");
        else if (t->source.octet[5] == 0x02) printf("Baggersee");
        else if (t->source.octet[5] == 0x03) printf("Krimmeri-Stade de la Meinau");
        else if (t->source.octet[5] == 0x04) printf("Étoile Bourse");
        else if (t->source.octet[5] == 0x05) printf("République");
        else if (t->source.octet[5] == 0x06) printf("Université");
        else if (t->source.octet[5] == 0x07) printf("Observatoire");
        else printf("Station inconnue");
    } else {
        printf("Non-station");
    }
    
    printf(")\n");
    
    printf("Destination : ");
    afficher_mac(t->destination);
    printf(" (");
    
    // Même chose pour la destination
    if (t->destination.octet[0] == 0x11 && t->destination.octet[1] == 0x22) {
        if (t->destination.octet[5] == 0x01) printf("Homme de Fer");
        else if (t->destination.octet[5] == 0x02) printf("Baggersee");
        else if (t->destination.octet[5] == 0x03) printf("Krimmeri-Stade de la Meinau");
        else if (t->destination.octet[5] == 0x04) printf("Étoile Bourse");
        else if (t->destination.octet[5] == 0x05) printf("République");
        else if (t->destination.octet[5] == 0x06) printf("Université");
        else if (t->destination.octet[5] == 0x07) printf("Observatoire");
        else printf("Station inconnue");
    } else {
        printf("Non-station");
    }
    
    printf(")\n");
    
    printf("Type        : 0x%04x ", t->type);
    
    // Affichage du type de protocole
    switch (t->type) {
        case 0x0800: printf("(IPv4)"); break;
        case 0x0806: printf("(ARP)"); break;
        case 0x86DD: printf("(IPv6)"); break;
        case 0x0027: printf("(STP)"); break;
        default: printf("(Inconnu)"); break;
    }
    printf("\n");
    
    printf("Données     : ");
    if (t->donnees != NULL) {
        printf("'%s' (%zu octets)\n", (char*)t->donnees, t->taille_donnees);
    } else {
        printf("NULL\n");
    }
    
    printf("=================================\n");
}

void simuler_message_tram(const char *source_nom, const char *dest_nom, const char *message) {
    // Attribution des MAC en fonction des noms
    MAC source = {0}, dest = {0};
    
    // MAC adresses pour les stations du tram
    // Format: 11:22:33:44:55:XX où XX est l'identifiant de la station
    if (strcmp(source_nom, "Homme de Fer") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x01);
    } else if (strcmp(source_nom, "Baggersee") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x02);
    } else if (strcmp(source_nom, "Krimmeri-Stade de la Meinau") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x03);
    } else if (strcmp(source_nom, "Étoile Bourse") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x04);
    } else if (strcmp(source_nom, "République") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x05);
    } else if (strcmp(source_nom, "Université") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x06);
    } else if (strcmp(source_nom, "Observatoire") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x07);
    } else {
        printf("Station source inconnue: %s\n", source_nom);
        return;
    }
    
    if (strcmp(dest_nom, "Homme de Fer") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x01);
    } else if (strcmp(dest_nom, "Baggersee") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x02);
    } else if (strcmp(dest_nom, "Krimmeri-Stade de la Meinau") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x03);
    } else if (strcmp(dest_nom, "Étoile Bourse") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x04);
    } else if (strcmp(dest_nom, "République") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x05);
    } else if (strcmp(dest_nom, "Université") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x06);
    } else if (strcmp(dest_nom, "Observatoire") == 0) {
        dest = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x07);
    } else {
        printf("Station destination inconnue: %s\n", dest_nom);
        return;
    }
    
    // Allouer de la mémoire pour le message
    uint8_t *donnees = (uint8_t *)malloc(strlen(message) + 1);
    if (donnees == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return;
    }
    strcpy((char *)donnees, message);
    
    // Créer la trame
    trame t;
    if (init_trame(&t, source, dest, 0x0800, donnees, strlen(message) + 1)) {
        printf("Message de %s à %s:\n", source_nom, dest_nom);
        afficher_trame(&t);
        
        // Pas besoin d'appeler deinit_trame ici car nous ne libérons pas les données
        // dans la fonction, pour éviter une double libération
    } else {
        printf("Erreur d'initialisation de la trame\n");
        free(donnees);
    }
}

void simuler_annonce(const char *source_nom, const char *message) {
    // La MAC pour les broadcasts est FF:FF:FF:FF:FF:FF
    MAC broadcast = creer_mac(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    
    // Déterminer la MAC source
    MAC source = {0};
    if (strcmp(source_nom, "Homme de Fer") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x01);
    } else if (strcmp(source_nom, "Baggersee") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x02);
    } else if (strcmp(source_nom, "Krimmeri-Stade de la Meinau") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x03);
    } else if (strcmp(source_nom, "Étoile Bourse") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x04);
    } else if (strcmp(source_nom, "République") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x05);
    } else if (strcmp(source_nom, "Université") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x06);
    } else if (strcmp(source_nom, "Observatoire") == 0) {
        source = creer_mac(0x11, 0x22, 0x33, 0x44, 0x55, 0x07);
    } else {
        printf("Station source inconnue: %s\n", source_nom);
        return;
    }
    
    // Allouer de la mémoire pour le message
    uint8_t *donnees = (uint8_t *)malloc(strlen(message) + 1);
    if (donnees == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return;
    }
    strcpy((char *)donnees, message);
    
    // Créer la trame
    trame t;
    if (init_trame(&t, source, broadcast, 0x0806, donnees, strlen(message) + 1)) {
        printf("Annonce de %s à toutes les stations:\n", source_nom);
        afficher_trame(&t);
        
        // Note: nous ne libérons pas les données ici pour éviter une double libération
    } else {
        printf("Erreur d'initialisation de la trame\n");
        free(donnees);
    }
}

int main(void) {
    printf("=== Programme de test pour le réseau de tram de Strasbourg ===\n\n");
    
    // Test 1: Message direct de Krimmeri-Stade de la Meinau à République
    simuler_message_tram("Krimmeri-Stade de la Meinau", "République", 
                         "Prochain tram dans 5 minutes");
    
    printf("\n");
    
    // Test 2: Message direct de République à Krimmeri-Stade de la Meinau
    simuler_message_tram("République", "Krimmeri-Stade de la Meinau", 
                         "Accusé de réception");
    
    printf("\n");
    
    // Test 3: Annonce de Homme de Fer à toutes les stations
    simuler_annonce("Homme de Fer", "Attention: perturbations sur la ligne A");
    
    printf("\n");
    
    // Test 4: Message de Baggersee à Université
    simuler_message_tram("Baggersee", "Université", 
                         "Tram en direction de l'Université en approche");
    
    printf("\n");
    
    // Test 5: Message d'Étoile Bourse à Observatoire
    simuler_message_tram("Étoile Bourse", "Observatoire", 
                         "Tram retardé de 10 minutes");
    
    printf("\n=== Fin des tests ===\n");
    
    return 0;
}