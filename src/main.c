/**
 * Programme principal de simulation réseau
 * 
 * Ce fichier contient le programme principal qui simule un réseau
 * commuté avec le protocole STP. Il permet de :
 * - Charger une topologie depuis un fichier de configuration
 * - Initialiser les switches et stations
 * - Exécuter le protocole STP
 * - Simuler l'envoi de trames
 * - Afficher l'état du réseau
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "switch.h"
#include "stp.h"
#include "graphe.h"
#include "adresse.h"
#include "trame.h"
#include "station.h"
#include "configuration.h"

/**
 * Affiche l'état détaillé du réseau après l'exécution de STP
 */
void afficher_etat_reseau(configuration_reseau_t *config) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ÉTAT DU RÉSEAU APRÈS STP                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    // Afficher les switches
    for (int i = 0; i < config->nb_switches; i++) {
        printf("\n┌─ Switch %d ", i);
        char mac_str[18];
        MAC_to_string(config->switches[i].mac, mac_str);
        printf("(%s) - Priorité: %d\n", mac_str, config->switches[i].priorite);
        
        printf("├─ Ports:\n");
        for (int p = 0; p < config->switches[i].nb_ports; p++) {
            printf("│  Port %d: %-12s - %-12s - Coût: %d",
                   p,
                   port_role_to_string(get_port_role(&config->switches[i], p)),
                   port_state_to_string(get_port_state(&config->switches[i], p)),
                   config->switches[i].ports[p].cost);
            
            if (port_est_actif(&config->switches[i], p)) {
                printf(" [ACTIF]");
            } else {
                printf(" [INACTIF]");
            }
            printf("\n");
        }
        
        printf("└─ Table de commutation:\n");
        if (config->switches[i].table.taille > 0) {
            for (int t = 0; t < config->switches[i].table.taille; t++) {
                char mac_entry[18];
                MAC_to_string(config->switches[i].table.entrees[t].mac, mac_entry);
                printf("   %s -> Port %d\n", mac_entry, config->switches[i].table.entrees[t].port);
            }
        } else {
            printf("   (vide)\n");
        }
    }
    
    // Afficher les stations
    printf("\n┌─ STATIONS CONNECTÉES:\n");
    for (int i = 0; i < config->nb_stations; i++) {
        char mac_str[18];
        MAC_to_string(config->stations[i].mac, mac_str);
        printf("│ Station %d: %s (IP: %d.%d.%d.%d)\n", 
               i, mac_str,
               config->stations[i].ip.octet[0], config->stations[i].ip.octet[1],
               config->stations[i].ip.octet[2], config->stations[i].ip.octet[3]);
    }
    printf("└─\n");
}

/**
 * Affiche la topologie du réseau
 */
void afficher_topologie(configuration_reseau_t *config) {
    printf("\n═══ TOPOLOGIE DU RÉSEAU ═══\n");
    printf("Nombre de nœuds dans le graphe: %zu\n", config->g->ordre);
    printf("Nombre d'arêtes: %zu\n", nb_aretes(config->g));
    
    printf("\nConnexions:\n");
    for (size_t i = 0; i < config->g->ordre; i++) {
        printf("Nœud %zu connecté à: ", i);
        bool first = true;
        
        // Allouer un tableau pour stocker les sommets adjacents
        sommet *sommets_adj = malloc(config->g->ordre * sizeof(sommet));
        if (sommets_adj == NULL) {
            printf("Erreur d'allocation mémoire\n");
            continue;
        }
        
        // Obtenir les sommets adjacents
        size_t nb_adj = sommets_adjacents(config->g, i, sommets_adj);
        
        // Afficher les sommets adjacents
        for (size_t j = 0; j < nb_adj; j++) {
            if (!first) printf(", ");
            printf("%zu", sommets_adj[j]);
            first = false;
        }
        
        if (first) printf("aucun");
        printf("\n");
        
        // Libérer la mémoire
        free(sommets_adj);
    }
}

/**
 * Simule l'envoi d'une trame dans le réseau
 */
void simuler_envoi_trame(configuration_reseau_t *config,
                        MAC src_mac, MAC dst_mac, const char* message,
                        int switch_entree, int port_entree) {
    
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ SIMULATION D'ENVOI DE TRAME                                 │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    // Vérifier la validité des paramètres
    if (switch_entree < 0 || switch_entree >= config->nb_switches) {
        printf("Switch d'entrée invalide: %d (max: %d)\n", switch_entree, config->nb_switches - 1);
        return;
    }
    
    if (port_entree < 0 || port_entree >= config->switches[switch_entree].nb_ports) {
        printf("Port d'entrée invalide: %d (max: %d)\n", 
               port_entree, config->switches[switch_entree].nb_ports - 1);
        return;
    }
    
    // Créer la trame
    trame t;
    uint8_t *data = (uint8_t*)message;
    if (!init_trame(&t, src_mac, dst_mac, TYPE_IPV4, data, strlen(message))) {
        printf("Erreur lors de la création de la trame\n");
        return;
    }
    
    t.fcs = calculer_fcs(&t);
    
    printf("Trame créée:\n");
    afficher_trame(&t);
    printf("\n");
    
    // Traitement par le switch d'entrée
    printf("Traitement par Switch %d (Port %d)\n", switch_entree, port_entree);
    
    // Vérifier l'état du port d'entrée
    if (get_port_state(&config->switches[switch_entree], port_entree) != PORT_FORWARDING) {
        printf("Port %d du switch %d n'est pas en état FORWARDING\n", 
               port_entree, switch_entree);
        printf("   État actuel: %s\n", 
               port_state_to_string(get_port_state(&config->switches[switch_entree], port_entree)));
        deinit_trame(&t);
        return;
    }
    
    // Apprentissage de l'adresse source
    printf("Apprentissage: ");
    char src_str[18];
    MAC_to_string(src_mac, src_str);
    printf("%s -> Port %d\n", src_str, port_entree);
    ajouter_entree_table(&config->switches[switch_entree].table, src_mac, port_entree);
    
    // Recherche de l'adresse de destination
    int port_destination = chercher_port_mac(&config->switches[switch_entree].table, dst_mac);
    
    char dst_str[18];
    MAC_to_string(dst_mac, dst_str);
    
    if (est_broadcast(dst_mac)) {
        printf("Adresse broadcast détectée\n");
        printf("Flood sur tous les ports actifs (sauf port d'entrée %d):\n", port_entree);
        
        int ports_utilises = 0;
        for (int p = 0; p < config->switches[switch_entree].nb_ports; p++) {
            if (p != port_entree) {
                if (get_port_state(&config->switches[switch_entree], p) == PORT_FORWARDING &&
                    port_est_actif(&config->switches[switch_entree], p)) {
                    printf("   Port %d: %s\n", p, 
                           port_role_to_string(get_port_role(&config->switches[switch_entree], p)));
                    ports_utilises++;
                } else {
                    printf("   Port %d: %s (%s)\n", p,
                           port_role_to_string(get_port_role(&config->switches[switch_entree], p)),
                           port_state_to_string(get_port_state(&config->switches[switch_entree], p)));
                }
            }
        }
        printf("   Total ports utilisés: %d\n", ports_utilises);
        
    } else if (port_destination == -1) {
        printf("Adresse destination %s inconnue\n", dst_str);
        printf("Flood sur tous les ports actifs (sauf port d'entrée %d):\n", port_entree);
        
        int ports_utilises = 0;
        for (int p = 0; p < config->switches[switch_entree].nb_ports; p++) {
            if (p != port_entree) {
                if (get_port_state(&config->switches[switch_entree], p) == PORT_FORWARDING &&
                    port_est_actif(&config->switches[switch_entree], p)) {
                    printf("   Port %d: %s\n", p,
                           port_role_to_string(get_port_role(&config->switches[switch_entree], p)));
                    ports_utilises++;
                } else {
                    printf("   Port %d: %s (%s)\n", p,
                           port_role_to_string(get_port_role(&config->switches[switch_entree], p)),
                           port_state_to_string(get_port_state(&config->switches[switch_entree], p)));
                }
            }
        }
        printf("   Total ports utilisés: %d\n", ports_utilises);
        
    } else if (port_destination == port_entree) {
        printf("Port de destination = port d'entrée (%d), trame ignorée\n", port_entree);
        
    } else {
        printf("Destination trouvée: %s -> Port %d\n", dst_str, port_destination);
        if (get_port_state(&config->switches[switch_entree], port_destination) == PORT_FORWARDING &&
            port_est_actif(&config->switches[switch_entree], port_destination)) {
            printf("Envoi direct sur port %d (%s)\n", port_destination,
                   port_role_to_string(get_port_role(&config->switches[switch_entree], port_destination)));
        } else {
            printf("Port %d non disponible (%s, %s)\n", port_destination,
                   port_role_to_string(get_port_role(&config->switches[switch_entree], port_destination)),
                   port_state_to_string(get_port_state(&config->switches[switch_entree], port_destination)));
        }
    }
    
    deinit_trame(&t);
}

/**
 * Effectue des tests de communication automatiques
 */
void tests_communication_automatiques(configuration_reseau_t *config) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                 TESTS DE COMMUNICATION AUTO                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    if (config->nb_switches == 0) {
        printf("⚠️  Aucun switch disponible pour les tests\n");
        return;
    }
    
    if (config->nb_stations == 0) {
        printf("⚠️  Aucune station disponible pour les tests\n");
        return;
    }
    
    // Test 1: Broadcast depuis la première station
    printf("\n🧪 TEST 1: Broadcast depuis Station 0\n");
    MAC broadcast = creer_mac(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    simuler_envoi_trame(config,
                       config->stations[0].mac, broadcast, 
                       "Test broadcast depuis Station 0",
                       0, 0);  // Supposons connexion sur switch 0, port 0
    
    // Test 2: Communication unicast si plusieurs stations
    if (config->nb_stations >= 2) {
        printf("\n🧪 TEST 2: Unicast Station 0 -> Station 1\n");
        simuler_envoi_trame(config,
                           config->stations[0].mac, config->stations[1].mac,
                           "Message privé de Station 0 vers Station 1",
                           0, 0);
        
        printf("\n🧪 TEST 3: Unicast Station 1 -> Station 0 (retour)\n");
        simuler_envoi_trame(config,
                           config->stations[1].mac, config->stations[0].mac,
                           "Réponse de Station 1 vers Station 0",
                           0, 1);  // Supposons Station 1 sur port 1
    }
    
    // Test 3: Trame vers adresse inconnue
    printf("\n🧪 TEST 4: Trame vers adresse MAC inconnue\n");
    MAC unknown = creer_mac(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF);
    simuler_envoi_trame(config,
                       config->stations[0].mac, unknown,
                       "Message vers adresse inconnue",
                       0, 0);
}

/**
 * Fonction principale du programme
 */
int main(int argc, char *argv[]) {
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              SIMULATION DE RÉSEAU AVEC STP                     ║\n");
    printf("║                    Version Complète                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    // Déterminer le fichier de configuration à utiliser
    const char *fichier_config = "test_config.txt";
    if (argc > 1) {
        fichier_config = argv[1];
        printf("Utilisation du fichier de configuration: %s\n", fichier_config);
    } else {
        printf("Utilisation du fichier par défaut: %s\n", fichier_config);
        printf("   (Utilisez: %s <fichier_config> pour spécifier un autre fichier)\n\n", argv[0]);
    }
    
    // Initialiser la structure de configuration
    configuration_reseau_t config = {0};
    
    // Charger la configuration complète depuis le fichier
    printf("Chargement de la configuration réseau...\n");
    if (!charger_configuration_complete(fichier_config, &config)) {
        printf("Erreur lors du chargement de la configuration depuis %s\n", fichier_config);
        printf("   Vérifiez que le fichier existe et respecte le bon format.\n");
        return 1;
    }
    
    printf("Configuration chargée avec succès !\n\n");
    
    // Afficher un résumé de la configuration
    printf("═══ RÉSUMÉ DE LA CONFIGURATION ═══\n");
    printf("Switches: %d\n", config.nb_switches);
    printf("Stations: %d\n", config.nb_stations);
    printf("Liens: %zu\n", nb_aretes(config.g));
    printf("Nœuds dans le graphe: %zu\n", config.g->ordre);
    
    // Afficher les équipements détaillés
    printf("\n═══ SWITCHES CONFIGURÉS ═══\n");
    for (int i = 0; i < config.nb_switches; i++) {
        char mac_str[18];
        MAC_to_string(config.switches[i].mac, mac_str);
        printf("Switch %d: %s\n", i, mac_str);
        printf("   - Priorité: %d\n", config.switches[i].priorite);
        printf("   - Ports: %d\n", config.switches[i].nb_ports);
        
        // Afficher l'état initial des ports
        for (int p = 0; p < config.switches[i].nb_ports; p++) {
            printf("     Port %d: %s (%s)\n", p,
                   port_est_actif(&config.switches[i], p) ? "ACTIF" : "INACTIF",
                   port_state_to_string(get_port_state(&config.switches[i], p)));
        }
    }
    
    printf("\n═══ STATIONS CONFIGURÉES ═══\n");
    for (int i = 0; i < config.nb_stations; i++) {
        char mac_str[18];
        MAC_to_string(config.stations[i].mac, mac_str);
        printf("Station %d: %s\n", i, mac_str);
        printf("   - IP: %d.%d.%d.%d\n",
               config.stations[i].ip.octet[0], config.stations[i].ip.octet[1],
               config.stations[i].ip.octet[2], config.stations[i].ip.octet[3]);
    }
    
    // Afficher la topologie
    afficher_topologie(&config);
    
    // Vérification de cohérence
    if (config.nb_switches == 0) {
        printf("Attention: Aucun switch configuré, simulation limitée.\n");
    }
    
    if (config.nb_stations == 0) {
        printf("Attention: Aucune station configurée, pas de tests de communication possibles.\n");
    }
    
    // Initialiser les structures STP si on a des switches
    switch_stp_t *stp_switches = NULL;
    if (config.nb_switches > 0) {
        stp_switches = malloc(config.nb_switches * sizeof(switch_stp_t));
        if (!stp_switches) {
            printf("Erreur d'allocation mémoire pour STP\n");
            liberer_configuration(&config);
            return 1;
        }
        
        printf("\nInitialisation des structures STP...\n");
        for (int i = 0; i < config.nb_switches; i++) {
            init_stp(&stp_switches[i], &config.switches[i]);
            char mac_str[18];
            MAC_to_string(config.switches[i].mac, mac_str);
            printf("   Switch %d (%s) initialisé\n", i, mac_str);
        }
        
        printf("\nCalcul du Spanning Tree Protocol...\n");
        
        // Calculer le STP
        calculer_stp_simple(stp_switches, config.nb_switches, config.g);
        
        printf("STP calculé avec succès!\n");
    } else {
        printf("\nPas de switches: STP non applicable\n");
    }
    
    // Afficher l'état du réseau après STP
    afficher_etat_reseau(&config);
    
    // Effectuer les tests de communication
    if (config.nb_switches > 0 && config.nb_stations > 0) {
        tests_communication_automatiques(&config);
        
        // Afficher l'état final des tables après les tests
        printf("\n╔══════════════════════════════════════════════════════════════╗\n");
        printf("║               ÉTAT FINAL APRÈS TESTS                        ║\n");
        printf("╚══════════════════════════════════════════════════════════════╝\n");
        afficher_etat_reseau(&config);
    }
    
    // Nettoyage de la mémoire
    printf("\nNettoyage de la mémoire...\n");
    if (stp_switches) {
        for (int i = 0; i < config.nb_switches; i++) {
            deinit_stp(&stp_switches[i]);
        }
        free(stp_switches);
    }
    
    liberer_configuration(&config);
    
    printf("Simulation terminée avec succès!\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                        FIN DU PROGRAMME                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}