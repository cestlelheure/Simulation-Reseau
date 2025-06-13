/**
 * Programme principal de simulation réseau
 * 
 * Ce fichier contient le programme principal qui simule un réseau
 * commuté avec le protocole STP. Il permet de :
 * - Créer une topologie de test
 * - Initialiser les switches et stations
 * - Exécuter le protocole STP
 * - Simuler l'envoi de trames
 * - Afficher l'état du réseau
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "switch.h"
#include "stp.h"
#include "graphe.h"
#include "adresse.h"
#include "trame.h"
#include "station.h"
#include "configuration.h"

/**
* Affiche la configuration réseau à partir d'un format txt
*/
void test_configuration() {

    graphe g;
    init_graphe(&g);
    int result = charger_configuration("test_config.txt", &g);
    printf("  Résultat du chargement: %s\n", result ? "Succès" : "Échec");
    deinit_graphe(&g);
}

/**
 * Structure pour simuler des stations connectées aux switches
 * 
 * Cette structure étend la structure station_t avec des informations
 * sur la connexion physique au réseau :
 * - nom : identifiant lisible de la station
 * - switch_id : identifiant du switch connecté
 * - port_connecte : numéro du port sur le switch
 */
typedef struct {
    station_t station;
    char nom[32];
    int switch_id;
    int port_connecte;
} station_connectee_t;

/**
 * Crée une topologie de test en forme de triangle
 * 
 * Cette fonction crée une topologie simple avec 3 switches
 * connectés en triangle, utilisée pour tester le protocole STP.
 * 
 * Retourne un pointeur vers le graphe créé, NULL en cas d'erreur
 */
graphe* creer_topologie_triangle() {
    graphe *g = malloc(sizeof(graphe));
    if (!g) return NULL;
    
    init_graphe(g);
    
    // Ajouter 3 sommets (switches)
    ajouter_sommet(g);  // Switch 0
    ajouter_sommet(g);  // Switch 1
    ajouter_sommet(g);  // Switch 2
    
    // Créer un triangle (chaque switch connecté aux deux autres)
    arete a1 = {0, 1};
    arete a2 = {1, 2};
    arete a3 = {2, 0};
    
    ajouter_arete(g, a1);  // SW0 <-> SW1
    ajouter_arete(g, a2);  // SW1 <-> SW2
    ajouter_arete(g, a3);  // SW2 <-> SW0
    
    return g;
}

/**
 * Initialise les switches de test
 * 
 * Cette fonction initialise 3 switches avec des priorités
 * différentes pour tester la sélection de la racine STP :
 * - Switch 0 : priorité haute (100)
 * - Switch 1 : priorité moyenne (200)
 * - Switch 2 : priorité basse (300)
 */
void init_switches_test(switch_t switches[], int nb_switches) {
    // Switch 0 - Priorité haute (sera probablement root)
    switches[0] = creer_switch(creer_mac(0x00, 0x01, 0x02, 0x03, 0x04, 0x05), 3, 100);
    
    // Switch 1 - Priorité moyenne
    switches[1] = creer_switch(creer_mac(0x00, 0x01, 0x02, 0x03, 0x04, 0x06), 3, 200);
    
    // Switch 2 - Priorité basse
    switches[2] = creer_switch(creer_mac(0x00, 0x01, 0x02, 0x03, 0x04, 0x07), 3, 300);
    
    // Activer tous les ports avec coût 10
    for (int i = 0; i < nb_switches; i++) {
        for (int p = 0; p < switches[i].nb_ports; p++) {
            activer_port(&switches[i], p);
            switches[i].ports[p].cost = 10;
        }
    }
}

/**
 * Initialise les stations de test
 * 
 * Cette fonction initialise 3 stations connectées à des
 * switches différents pour tester la communication :
 * - Station A : connectée au switch 0
 * - Station B : connectée au switch 1
 * - Station C : connectée au switch 2
 */
void init_stations_test(station_connectee_t stations[], int nb_stations) {
    // Station A connectée au switch 0, port 0
    stations[0].station = creer_station(
        creer_mac(0x10, 0x10, 0x10, 0x10, 0x10, 0x10),
        creer_ip(192, 168, 1, 10)
    );
    strcpy(stations[0].nom, "Station_A");
    stations[0].switch_id = 0;
    stations[0].port_connecte = 0;
    
    // Station B connectée au switch 1, port 0
    stations[1].station = creer_station(
        creer_mac(0x20, 0x20, 0x20, 0x20, 0x20, 0x20),
        creer_ip(192, 168, 1, 20)
    );
    strcpy(stations[1].nom, "Station_B");
    stations[1].switch_id = 1;
    stations[1].port_connecte = 0;
    
    // Station C connectée au switch 2, port 0
    stations[2].station = creer_station(
        creer_mac(0x30, 0x30, 0x30, 0x30, 0x30, 0x30),
        creer_ip(192, 168, 1, 30)
    );
    strcpy(stations[2].nom, "Station_C");
    stations[2].switch_id = 2;
    stations[2].port_connecte = 0;
}

/**
 * Affiche l'état du réseau après l'exécution de STP
 * 
 * Cette fonction affiche de manière formatée :
 * - Les informations de chaque switch
 * - L'état et le rôle de chaque port
 * - La table de commutation de chaque switch
 */
void afficher_etat_reseau(switch_t switches[], int nb_switches, graphe *g) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ÉTAT DU RÉSEAU APRÈS STP                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    for (int i = 0; i < nb_switches; i++) {
        printf("\n┌─ Switch %d ", i);
        char mac_str[18];
        MAC_to_string(switches[i].mac, mac_str);
        printf("(%s) - Priorité: %d\n", mac_str, switches[i].priorite);
        
        printf("├─ Ports:\n");
        for (int p = 0; p < switches[i].nb_ports; p++) {
            printf("│  Port %d: %-12s - %-12s - Coût: %d\n", p,
                   port_role_to_string(get_port_role(&switches[i], p)),
                   port_state_to_string(get_port_state(&switches[i], p)),
                   switches[i].ports[p].cost);
        }
        
        printf("└─ Table de commutation:\n");
        if (switches[i].table.taille > 0) {
            for (int t = 0; t < switches[i].table.taille; t++) {
                char mac_entry[18];
                MAC_to_string(switches[i].table.entrees[t].mac, mac_entry);
                printf("   %s -> Port %d\n", mac_entry, switches[i].table.entrees[t].port);
            }
        } else {
            printf("   (vide)\n");
        }
    }
}

/**
 * Simule l'envoi d'une trame dans le réseau
 * 
 * Cette fonction simule le traitement d'une trame par un switch :
 * 1. Création de la trame
 * 2. Vérification de l'état du port d'entrée
 * 3. Apprentissage de l'adresse source
 * 4. Recherche de l'adresse destination
 * 5. Décision de forwarding (unicast, broadcast ou flood)
 */
void simuler_envoi_trame(switch_t switches[], int nb_switches, graphe *g,
                        MAC src_mac, MAC dst_mac, const char* message,
                        int switch_entree, int port_entree) {
    
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ SIMULATION D'ENVOI DE TRAME                                 │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    // Créer la trame
    trame t;
    uint8_t *data = (uint8_t*)message;
    if (!init_trame(&t, src_mac, dst_mac, TYPE_IPV4, data, strlen(message))) {
        printf("Erreur lors de la création de la trame\n");
        return;
    }
    
    // Calculer et assigner le FCS
    t.fcs = calculer_fcs(&t);
    
    printf("Trame créée:\n");
    afficher_trame(&t);
    printf("\n");
    
    // Traitement par le switch d'entrée
    printf("═══ Traitement par Switch %d ═══\n", switch_entree);
    
    // Vérifier l'état du port d'entrée
    if (get_port_state(&switches[switch_entree], port_entree) != PORT_FORWARDING) {
        printf("Port %d du switch %d n'est pas en état FORWARDING\n", 
               port_entree, switch_entree);
        printf("État actuel: %s\n", 
               port_state_to_string(get_port_state(&switches[switch_entree], port_entree)));
        deinit_trame(&t);
        return;
    }
    
    // Apprentissage de l'adresse source
    printf("Apprentissage: ");
    char src_str[18];
    MAC_to_string(src_mac, src_str);
    printf("%s -> Port %d\n", src_str, port_entree);
    ajouter_entree_table(&switches[switch_entree].table, src_mac, port_entree);
    
    // Recherche de l'adresse de destination
    int port_destination = chercher_port_mac(&switches[switch_entree].table, dst_mac);
    
    char dst_str[18];
    MAC_to_string(dst_mac, dst_str);
    
    if (est_broadcast(dst_mac)) {
        printf("Adresse broadcast détectée\n");
        printf("Flood sur tous les ports actifs sauf port d'entrée %d:\n", port_entree);
        
        for (int p = 0; p < switches[switch_entree].nb_ports; p++) {
            if (p != port_entree && 
                get_port_state(&switches[switch_entree], p) == PORT_FORWARDING &&
                port_est_actif(&switches[switch_entree], p)) {
                printf("   Port %d: %s\n", p, 
                       port_role_to_string(get_port_role(&switches[switch_entree], p)));
            } else if (p != port_entree) {
                printf("   Port %d: %s (%s)\n", p,
                       port_role_to_string(get_port_role(&switches[switch_entree], p)),
                       port_state_to_string(get_port_state(&switches[switch_entree], p)));
            }
        }
    } else if (port_destination == -1) {
        printf("Adresse destination %s inconnue\n", dst_str);
        printf("Flood sur tous les ports actifs sauf port d'entrée %d:\n", port_entree);
        
        for (int p = 0; p < switches[switch_entree].nb_ports; p++) {
            if (p != port_entree && 
                get_port_state(&switches[switch_entree], p) == PORT_FORWARDING &&
                port_est_actif(&switches[switch_entree], p)) {
                printf("   Port %d: %s\n", p,
                       port_role_to_string(get_port_role(&switches[switch_entree], p)));
            } else if (p != port_entree) {
                printf("   Port %d: %s (%s)\n", p,
                       port_role_to_string(get_port_role(&switches[switch_entree], p)),
                       port_state_to_string(get_port_state(&switches[switch_entree], p)));
            }
        }
    } else if (port_destination == port_entree) {
        printf("Port de destination = port d'entrée (%d), trame ignorée\n", port_entree);
    } else {
        printf("Destination trouvée: %s -> Port %d\n", dst_str, port_destination);
        if (get_port_state(&switches[switch_entree], port_destination) == PORT_FORWARDING &&
            port_est_actif(&switches[switch_entree], port_destination)) {
            printf("Envoi direct sur port %d (%s)\n", port_destination,
                   port_role_to_string(get_port_role(&switches[switch_entree], port_destination)));
        } else {
            printf(" Port %d non disponible (%s, %s)\n", port_destination,
                   port_role_to_string(get_port_role(&switches[switch_entree], port_destination)),
                   port_state_to_string(get_port_state(&switches[switch_entree], port_destination)));
        }
    }
    
    deinit_trame(&t);
}

/**
 * Fonction principale du programme
 * 
 * Cette fonction :
 * 1. Teste la configuration
 * 2. Crée une topologie de test
 * 3. Initialise les switches et stations
 * 4. Exécute le protocole STP
 * 5. Affiche l'état du réseau
 * 6. Simule l'envoi de trames 
 * 
 * Retourne 0 en cas de succès, 1 en cas d'erreur
 */
int main() {
    test_configuration();

    printf("\n\n\n");

    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              SIMULATION DE RÉSEAU AVEC STP                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    // Créer la topologie
    graphe *g = creer_topologie_triangle();
    if (!g) {
        printf("Erreur lors de la création du graphe\n");
        return 1;
    }
    
    // Initialiser les switches
    const int NB_SWITCHES = 3;
    switch_t switches[NB_SWITCHES];
    init_switches_test(switches, NB_SWITCHES);
    
    // Initialiser les structures STP
    switch_stp_t stp_switches[NB_SWITCHES];
    for (int i = 0; i < NB_SWITCHES; i++) {
        init_stp(&stp_switches[i], &switches[i]);
    }
    
    printf("Topologie créée: Triangle de 3 switches\n");
    printf("Calcul du Spanning Tree Protocol...\n\n");
    
    // Calculer le STP
    calculer_stp_simple(stp_switches, NB_SWITCHES, g);
    
    // Afficher l'état du réseau
    afficher_etat_reseau(switches, NB_SWITCHES, g);
    
    // Créer des stations de test
    const int NB_STATIONS = 3;
    station_connectee_t stations[NB_STATIONS];
    init_stations_test(stations, NB_STATIONS);
    
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    STATIONS CONNECTÉES                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    for (int i = 0; i < NB_STATIONS; i++) {
        char mac_str[18];
        MAC_to_string(stations[i].station.mac, mac_str);
        printf("%s (%s) -> Switch %d, Port %d\n", 
               stations[i].nom, mac_str, stations[i].switch_id, stations[i].port_connecte);
    }
    
    // Tests de communication
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    TESTS DE COMMUNICATION                    ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    // Test 1: Station A envoie à Station B
    printf("\n🧪 TEST 1: Station_A -> Station_B\n");
    simuler_envoi_trame(switches, NB_SWITCHES, g,
                       stations[0].station.mac, stations[1].station.mac, "Coucou Station B!",
                       stations[0].switch_id, stations[0].port_connecte);
    
    // Test 2: Broadcast depuis Station C
    printf("\n🧪 TEST 2: Station_C -> Broadcast\n");
    MAC broadcast = creer_mac(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    simuler_envoi_trame(switches, NB_SWITCHES, g,
                       stations[2].station.mac, broadcast, "Broadcast !",
                       stations[2].switch_id, stations[2].port_connecte);
    
    // Afficher l'état final des tables
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                  TABLES APRÈS TESTS                         ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    afficher_etat_reseau(switches, NB_SWITCHES, g);
    
    // Nettoyage
    for (int i = 0; i < NB_SWITCHES; i++) {
        deinit_stp(&stp_switches[i]);
        deinit_switch(&switches[i]);
    }
    deinit_graphe(g);
    free(g);
    
    printf("\nSimulation terminée avec succès!\n");
    return 0;
}