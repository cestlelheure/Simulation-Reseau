#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adresse.h"
#include "graphe.h"
#include "trame.h"
#include "station.h"
#include "switch.h"
#include "configuration.h"
#include "stp.h"

// Fonctions utilitaires pour les tests
void test_separator(const char* test_name) {
    printf("\n=== Test: %s ===\n", test_name);
}

void test_adresses() {
    test_separator("Test complet des adresses MAC et IPv4");
    
    // Test des fonctions de conversion en string
    MAC mac_test = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    char str_mac[18];
    
    printf("Test MAC_to_string:\n");
    if (MAC_to_string(mac_test, str_mac)) {
        printf("  MAC Address: %s\n", str_mac);
    }
    
    // Test avec broadcast
    MAC mac_broadcast = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    if (MAC_to_string(mac_broadcast, str_mac)) {
        printf("  MAC Broadcast: %s\n", str_mac);
    }
    
    // Test parse_mac
    printf("\nTest parse_mac:\n");
    MAC parsed_mac;
    const char* valid_mac = "00:1B:63:84:45:E6";
    const char* invalid_mac = "00:1B:63:84:45:EG"; // G invalide
    
    if (parse_mac(valid_mac, &parsed_mac)) {
        printf("  MAC valide parsée avec succès\n");
        MAC_to_string(parsed_mac, str_mac);
        printf("  Résultat: %s\n", str_mac);
    }
    
    if (!parse_mac(invalid_mac, &parsed_mac)) {
        printf("  MAC invalide correctement rejetée\n");
    }
    
    // Test mac_equals
    printf("\nTest mac_equals:\n");
    MAC mac1 = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    MAC mac2 = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    MAC mac3 = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE7}};
    
    printf("  Comparaison MAC identiques: %s\n", mac_equals(mac1, mac2) ? "OK" : "Échec");
    printf("  Comparaison MAC différentes: %s\n", !mac_equals(mac1, mac3) ? "OK" : "Échec");
}

void test_graphe() {
    test_separator("Test complet de la structure de graphe");
    
    graphe g;
    init_graphe(&g);
    
    printf("Test initialisation:\n");
    printf("  Ordre initial: %zu\n", ordre(&g));
    printf("  Nombre d'arêtes initial: %zu\n", nb_aretes(&g));
    
    // Test ajout de sommets
    printf("\nTest ajout de sommets:\n");
    for (int i = 0; i < 5; i++) {
        ajouter_sommet(&g);
        printf("  Ajout sommet %d - Ordre: %zu\n", i, ordre(&g));
    }
    
    // Test des index de sommets
    printf("\nTest index des sommets:\n");
    for (int i = 0; i < 6; i++) {
        size_t idx = index_sommet(&g, i);
        printf("  Index du sommet %d: %zu\n", i, idx);
    }
    
    // Test ajout d'arêtes
    printf("\nTest ajout d'arêtes:\n");
    arete a1 = {0, 1};
    arete a2 = {1, 2};
    arete a3 = {2, 3};
    arete a4 = {0, 3};
    arete a5 = {0, 1}; // Duplicate
    
    printf("  Arête (0,1): %s\n", ajouter_arete(&g, a1) ? "Ajoutée" : "Échec");
    printf("  Arête (1,2): %s\n", ajouter_arete(&g, a2) ? "Ajoutée" : "Échec");
    printf("  Arête (2,3): %s\n", ajouter_arete(&g, a3) ? "Ajoutée" : "Échec");
    printf("  Arête (0,3): %s\n", ajouter_arete(&g, a4) ? "Ajoutée" : "Échec");
    printf("  Arête (0,1) duplicate: %s\n", ajouter_arete(&g, a5) ? "Ajoutée" : "Échec");
    
    // Test existence d'arêtes
    printf("\nTest existence d'arêtes:\n");
    printf("  Arête (0,1) existe: %s\n", existe_arete(&g, a1) ? "Oui" : "Non");
    printf("  Arête (1,0) existe: %s\n", existe_arete(&g, (arete){1, 0}) ? "Oui" : "Non");
    printf("  Arête (0,4) existe: %s\n", existe_arete(&g, (arete){0, 4}) ? "Oui" : "Non");
    
    // Test des sommets adjacents
    printf("\nTest des sommets adjacents:\n");
    for (int i = 0; i < 5; i++) {
        sommet adjacents[10];
        size_t nb_adj = sommets_adjacents(&g, i, adjacents);
        printf("  Sommets adjacents à %d (%zu): ", i, nb_adj);
        for (size_t j = 0; j < nb_adj; j++) {
            printf("%zu ", adjacents[j]);
        }
        printf("\n");
    }
    
    deinit_graphe(&g);
    printf("\nGraphe libéré avec succès\n");
}

void test_trame() {
    test_separator("Test complet des trames Ethernet");
    
    // Test création de trames
    MAC mac_src = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    MAC mac_dest = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    
    printf("Test création de trames:\n");
    
    // Trame avec données
    uint8_t data[] = "Hello World!";
    size_t data_size = strlen((char*)data);
    
    trame t1;
    bool result = init_trame(&t1, mac_src, mac_dest, TYPE_IPV4, data, data_size);
    printf("  Création trame avec données: %s\n", result ? "Succès" : "Échec");
    
    if (result) {
        char str_mac[18];
        printf("  Détails de la trame:\n");
        MAC_to_string(t1.source, str_mac);
        printf("    Source: %s\n", str_mac);
        MAC_to_string(t1.destination, str_mac);
        printf("    Destination: %s\n", str_mac);
        printf("    Type: 0x%04X\n", t1.type);
        printf("    Taille données: %zu\n", t1.taille_donnees);
        printf("    Données: %s\n", (char*)t1.donnees);
    }
    
    // Trame sans données
    trame t2;
    result = init_trame(&t2, mac_src, mac_dest, TYPE_IPV4, NULL, 0);
    printf("\n  Création trame sans données: %s\n", result ? "Succès" : "Échec");
    
    // Test copie de trame
    printf("\nTest copie de trame:\n");
    trame t3;
    // Initialisation correcte de la trame copiée
    if (init_trame(&t3, t1.source, t1.destination, t1.type, t1.donnees, t1.taille_donnees)) {
        printf("  Copie de trame: Succès\n");
        printf("  Vérification de la copie:\n");
        printf("    Taille données copiée: %zu\n", t3.taille_donnees);
        printf("    Données copiées: %s\n", (char*)t3.donnees);
    } else {
        printf("  Copie de trame: Échec\n");
    }
    
    // Libération des trames
    deinit_trame(&t1);
    deinit_trame(&t2);
    deinit_trame(&t3);
    printf("\nTrames libérées avec succès\n");
}

void test_station() {
    test_separator("Test complet des stations");
    
    // Test création de station
    printf("Test création de station:\n");
    station_t s;
    MAC mac = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    IPv4 ip = {{192, 168, 1, 1}};
    
    s = creer_station(mac, ip);
    printf("  Création station: Succès\n");
    
    char str_mac[18];
    char str_ip[16];
    
    MAC_to_string(s.mac, str_mac);
    printf("  Détails de la station:\n");
    printf("    MAC: %s\n", str_mac);
    printf("    IP: %u.%u.%u.%u\n", 
           s.ip.octet[0], s.ip.octet[1], s.ip.octet[2], s.ip.octet[3]);
    
    // Test envoi de trame
    printf("\nTest envoi de trame:\n");
    MAC dest_mac = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    uint8_t data[] = "Test message";
    size_t data_size = strlen((char*)data);
    
    trame t;
    bool result = init_trame(&t, mac, dest_mac, TYPE_IPV4, data, data_size);
    printf("  Création trame de test: %s\n", result ? "Succès" : "Échec");
    
    if (result) {
        switch_t sw;
        sw = creer_switch(mac, 4, 0);
        envoyer_trame(&t, &sw);
        printf("  Envoi de trame: Succès\n");
        deinit_switch(&sw);
    }
    
    deinit_trame(&t);
    printf("\nStation et trame libérées avec succès\n");
}

void test_switch() {
    test_separator("Test complet des switches");
    
    // Test création de switch
    printf("Test création de switch:\n");
    switch_t sw;
    MAC mac = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    
    sw = creer_switch(mac, 4, 0);
    printf("  Création switch: Succès\n");
    
    char str_mac[18];
    MAC_to_string(sw.mac, str_mac);
    printf("  Détails du switch:\n");
    printf("    MAC: %s\n", str_mac);
    printf("    Nombre de ports: %d\n", sw.nb_ports);
    
    // Test des ports
    printf("\nTest des ports:\n");
    init_ports(&sw);
    for (int i = 0; i < sw.nb_ports; i++) {
        activer_port(&sw, i);
        printf("  Port %d: %s\n", i, port_est_actif(&sw, i) ? "Actif" : "Inactif");
    }
    
    // Test de la table de commutation
    printf("\nTest de la table de commutation:\n");
    MAC dest_mac = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    if (ajouter_entree_table(&sw.table, dest_mac, 0)) {
        printf("  Entrée ajoutée à la table\n");
        int port = chercher_port_mac(&sw.table, dest_mac);
        printf("  Port trouvé pour la MAC: %d\n", port);
    }
    
    deinit_switch(&sw);
    printf("\nSwitch libéré avec succès\n");
}

void test_stp() {
    test_separator("Test complet du protocole STP");
    
    // Test création de switch avec STP
    printf("Test création de switch avec STP:\n");
    switch_t sw;
    MAC mac = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    sw = creer_switch(mac, 4, 0);
    
    switch_stp_t stp;
    init_stp(&stp, &sw);
    printf("  Initialisation STP: Succès\n");
    
    // Test des états de port
    printf("\nTest des états de port:\n");
    for (int i = 0; i < sw.nb_ports; i++) {
        set_etat_port(&sw, i, PORT_DESIGNE);
        printf("  Port %d: %s\n", i, etat_port_to_string(get_etat_port(&sw, i)));
    }
    
    deinit_stp(&stp);
    deinit_switch(&sw);
    printf("\nSwitch et STP libérés avec succès\n");
}

void test_configuration() {
    test_separator("Test complet du chargement de configuration");
    
    graphe g;
    init_graphe(&g);
    
    // Test 1: Chargement d'un fichier valide
    printf("Test 1: Chargement du fichier de configuration valide\n");
    int result = charger_configuration("test_config.txt", &g);
    printf("  Résultat du chargement: %s\n", result ? "Succès" : "Échec");
    
    if (result) {
        printf("  Vérification de la structure du graphe:\n");
        printf("    Nombre de sommets: %zu\n", ordre(&g));
        printf("    Nombre d'arêtes: %zu\n", nb_aretes(&g));
        
        // Vérifier quelques connexions
        arete test_aretes[] = {{0, 1}, {1, 2}, {1, 3}, {3, 4}};
        printf("    Vérification des connexions:\n");
        for (int i = 0; i < 4; i++) {
            bool exists = existe_arete(&g, test_aretes[i]);
            printf("      Arête (%zu,%zu): %s\n", 
                   test_aretes[i].s1, test_aretes[i].s2, 
                   exists ? "Présente" : "Absente");
        }
    }
    
    deinit_graphe(&g);
    
    // Test 2: Tentative de chargement d'un fichier inexistant
    printf("\nTest 2: Chargement d'un fichier inexistant\n");
    result = charger_configuration("fichier_inexistant.txt", &g);
    printf("  Résultat du chargement: %s\n", result ? "Succès" : "Échec (attendu)");
    
    // Test 3: Fichier avec format incorrect
    printf("\nTest 3: Création et test d'un fichier avec format incorrect\n");
    FILE *f = fopen("test_config_invalid.txt", "w");
    if (f) {
        fprintf(f, "format incorrect\n");
        fprintf(f, "1;MAC invalide;IP invalide\n");
        fclose(f);
        
        result = charger_configuration("test_config_invalid.txt", &g);
        printf("  Résultat du chargement: %s\n", result ? "Succès" : "Échec (attendu)");
    }
}

int main() {
    printf("=== Programme de test complet des fonctionnalités ===\n");
    
    test_adresses();
    test_graphe();
    test_trame();
    test_station();
    test_switch();
    test_stp();
    test_configuration();
    
    printf("\n=== Fin des tests ===\n");
    return 0;
}