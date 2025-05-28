#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adresse.h"
#include "graphe.h"
#include "trame.h"
#include "station.h"
#include "switch.h"
#include "configuration.h"

// Fonctions utilitaires pour les tests
void test_separator(const char* test_name) {
    printf("\n=== Test: %s ===\n", test_name);
}

void test_adresses() {
    test_separator("Adresses MAC et IPv4");
    
    // Test des fonctions de conversion en string
    MAC mac_test = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    char str_mac[18];
    
    if (MAC_to_string(mac_test, str_mac)) {
        printf("MAC Address: %s\n", str_mac);
    } else {
        printf("Erreur lors de la conversion MAC\n");
    }
    
    // Test avec une autre adresse MAC
    MAC mac_test2 = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    if (MAC_to_string(mac_test2, str_mac)) {
        printf("MAC Broadcast: %s\n", str_mac);
    }
}

void test_graphe() {
    test_separator("Structure de graphe");
    
    graphe g;
    init_graphe(&g);
    
    printf("Graphe initialisé - Ordre: %zu, Nb arêtes: %zu\n", ordre(&g), nb_aretes(&g));
    
    // Ajout de sommets
    ajouter_sommet(&g); // Sommet 0
    ajouter_sommet(&g); // Sommet 1
    ajouter_sommet(&g); // Sommet 2
    ajouter_sommet(&g); // Sommet 3
    
    printf("Après ajout de 4 sommets - Ordre: %zu\n", ordre(&g));
    
    // Test des index de sommets
    printf("Index du sommet 2: %zu\n", index_sommet(&g, 2));
    printf("Index du sommet 5 (inexistant): %zu\n", index_sommet(&g, 5));
    
    // Ajout d'arêtes
    arete a1 = {0, 1};
    arete a2 = {1, 2};
    arete a3 = {2, 3};
    arete a4 = {0, 3};
    arete a5 = {0, 1}; // Duplicate
    
    printf("\nAjout d'arêtes:\n");
    printf("Arête (0,1): %s\n", ajouter_arete(&g, a1) ? "Ajoutée" : "Échec");
    printf("Arête (1,2): %s\n", ajouter_arete(&g, a2) ? "Ajoutée" : "Échec");
    printf("Arête (2,3): %s\n", ajouter_arete(&g, a3) ? "Ajoutée" : "Échec");
    printf("Arête (0,3): %s\n", ajouter_arete(&g, a4) ? "Ajoutée" : "Échec");
    printf("Arête (0,1) duplicate: %s\n", ajouter_arete(&g, a5) ? "Ajoutée" : "Échec");
    
    printf("Nombre d'arêtes après ajouts: %zu\n", nb_aretes(&g));
    
    // Test existence d'arêtes
    printf("\nTest d'existence d'arêtes:\n");
    printf("Arête (0,1) existe: %s\n", existe_arete(&g, a1) ? "Oui" : "Non");
    printf("Arête (1,0) existe: %s\n", existe_arete(&g, (arete){1, 0}) ? "Oui" : "Non");
    
    // Test des sommets adjacents
    printf("\nSommets adjacents au sommet 0:\n");
    sommet adjacents[10];
    size_t nb_adj = sommets_adjacents(&g, 0, adjacents);
    printf("Nombre de sommets adjacents: %zu\n", nb_adj);
    for (size_t i = 0; i < nb_adj; i++) {
        printf("  - Sommet %zu\n", adjacents[i]);
    }
    
    deinit_graphe(&g);
    printf("Graphe libéré\n");
}

void test_trame() {
    test_separator("Trames Ethernet");
    
    MAC mac_src = {{0x00, 0x1B, 0x63, 0x84, 0x45, 0xE6}};
    MAC mac_dest = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    
    // Données de test
    uint8_t data[] = "Hello World!";
    size_t data_size = strlen((char*)data);
    
    trame t;
    bool result = init_trame(&t, mac_src, mac_dest, TYPE_IPV4, data, data_size);
    
    printf("Initialisation de la trame: %s\n", result ? "Succès" : "Échec");
    
    if (result) {
        char str_mac[18];
        
        printf("Trame créée:\n");
        
        if (MAC_to_string(t.source, str_mac)) {
            printf("  Source: %s\n", str_mac);
        }
        
        if (MAC_to_string(t.destination, str_mac)) {
            printf("  Destination: %s\n", str_mac);
        }
        
        printf("  Type: 0x%04X\n", t.type);
        printf("  Taille données: %zu octets\n", t.taille_donnees);
        printf("  Données: %s\n", (char*)t.donnees);
        
        // Note: On ne libère pas les données car elles sont statiques dans ce test
        // deinit_trame(&t);
    }
}

void test_capacite_graphe() {
    test_separator("Test de capacité du graphe");
    
    graphe g;
    init_graphe(&g);
    
    // Ajout de beaucoup de sommets
    for (int i = 0; i < 10; i++) {
        ajouter_sommet(&g);
    }
    
    printf("Graphe avec %zu sommets créé\n", ordre(&g));
    
    // Ajout de nombreuses arêtes pour tester la réallocation
    int aretes_ajoutees = 0;
    for (sommet i = 0; i < 9; i++) {
        for (sommet j = i + 1; j < 10; j++) {
            arete a = {i, j};
            if (ajouter_arete(&g, a)) {
                aretes_ajoutees++;
            }
        }
    }
    
    printf("Arêtes ajoutées: %d\n", aretes_ajoutees);
    printf("Nombre d'arêtes dans le graphe: %zu\n", nb_aretes(&g));
    
    // Test de recherche d'index
    arete test_arete = {3, 7};
    size_t index = index_arete(&g, test_arete);
    if (index != UNKNOWN_INDEX) {
        printf("Arête (3,7) trouvée à l'index: %zu\n", index);
    } else {
        printf("Arête (3,7) non trouvée\n");
    }
    
    deinit_graphe(&g);
}

void test_configuration() {
    test_separator("Chargement de configuration réseau");
        
    graphe g;
    
    // Test 1: Chargement d'un fichier valide
    printf("Test 1: Chargement du fichier de configuration valide\n");
    int result = charger_configuration("test_config.txt", &g);
    printf("Résultat du chargement: %s\n", result ? "Succès" : "Échec");
    
    if (result) {
        printf("Vérification de la structure du graphe:\n");
        printf("  - Nombre de sommets: %zu\n", ordre(&g));
        printf("  - Nombre d'arêtes: %zu\n", nb_aretes(&g));
        
        // Vérifier quelques connexions
        arete test_aretes[] = {{0, 1}, {1, 2}, {1, 3}, {3, 4}};
        printf("  - Vérification des connexions:\n");
        for (int i = 0; i < 4; i++) {
            bool exists = existe_arete(&g, test_aretes[i]);
            printf("    Arête (%zu,%zu): %s\n", 
                   test_aretes[i].s1, test_aretes[i].s2, 
                   exists ? "Présente" : "Absente");
        }
        
        deinit_graphe(&g);
    }
    
    // Test 2: Tentative de chargement d'un fichier inexistant
    printf("\nTest 2: Chargement d'un fichier inexistant\n");
    result = charger_configuration("fichier_inexistant.txt", &g);
    printf("Résultat du chargement: %s\n", result ? "Succès" : "Échec (attendu)");
    
    // Test 3: Fichier avec format incorrect
    printf("\nTest 3: Création et test d'un fichier avec format incorrect\n");
    FILE *f = fopen("test_config_invalid.txt", "w");
    if (f) {
        fprintf(f, "format incorrect\n");
        fprintf(f, "1;MAC invalide;IP invalide\n");
        fclose(f);
        
        result = charger_configuration("test_config_invalid.txt", &g);
        printf("Résultat du chargement: %s\n", result ? "Succès" : "Échec (attendu)");
    }
    
    // Nettoyage des fichiers de test
    printf("\nNettoyage des fichiers de test...\n");
    remove("test_config.txt");
    remove("test_config_invalid.txt");
    printf("Fichiers de test supprimés\n");
}

int main() {
    printf("=== Programme de test des fonctions implémentées ===\n");
    
    test_adresses();
    test_graphe();
    test_trame();
    test_capacite_graphe();
    test_configuration();
    
    printf("\n=== Fin des tests ===\n");
    return 0;
}