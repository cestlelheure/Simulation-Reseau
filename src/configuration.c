/**
 * Gestion de la configuration réseau
 * 
 * Ce fichier contient les fonctions permettant de charger et gérer
 * la configuration d'un réseau à partir d'un fichier texte.
 * Il gère la lecture des équipements (switches et stations) et
 * des liens entre eux.
 */

#include "configuration.h"
#include "station.h"
#include "switch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Fonction askip vraiment utile */
//sscanf permet de recup des infos avec une chaine formatée
//sprintf permet de formater une chaine dans un char *


/**
 * Charge la configuration réseau à partir d'un fichier
 * 
 * Cette fonction lit un fichier de configuration au format suivant :
 * - Première ligne : nombre d'équipements et nombre de liens
 * - Lignes suivantes : description des équipements
 *   - Format switch : 2;MAC;nb_ports;priorite
 *   - Format station : 1;MAC;IP
 * - Dernières lignes : description des liens
 *   - Format : equipement1;equipement2;poids
 * 
 * Le fichier est parsé ligne par ligne et les structures
 * correspondantes sont créées et initialisées.
 * 
 * Retourne 1 si le chargement a réussi, 0 sinon
 */
int charger_configuration(const char *nom_fichier, graphe *g) {
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }
    
    char *ligne = NULL;
    size_t taille_ligne = 0;
    size_t longueur;
    int nombre_equipements, nombre_liens;
    
    // Lecture de la ligne d'en-tête (nb_equipements nb_liens)
    if ((longueur = getline(&ligne, &taille_ligne, f)) == -1 || 
        sscanf(ligne, "%d %d", &nombre_equipements, &nombre_liens) != 2) {
        fprintf(stderr, "Format de la première ligne incorrect\n");
        free(ligne);
        fclose(f);
        return 0;
    }
    
    // Initialisation du graphe
    init_graphe(g);
    
    // Tableaux pour stocker les équipements
    switch_t *switchs = malloc(nombre_equipements * sizeof(switch_t));
    station_t *stations = malloc(nombre_equipements * sizeof(station_t));
    
    if (!switchs || !stations) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        free(switchs);
        free(stations);
        free(ligne);
        fclose(f);
        return 0;
    }
    
    int nb_switchs = 0;
    int nb_stations = 0;
    
    // Lecture des équipements
    for (int i = 0; i < nombre_equipements; i++) {
        if ((longueur = getline(&ligne, &taille_ligne, f)) == -1) {
            fprintf(stderr, "Fin de fichier inattendue\n");
            free(switchs);
            free(stations);
            free(ligne);
            fclose(f);
            return 0;
        }
        
        int type_equipement;
        char *token = strtok(ligne, ";");
        if (token == NULL || sscanf(token, "%d", &type_equipement) != 1) {
            fprintf(stderr, "Format incorrect pour le type d'équipement\n");
            continue;
        }
        
        // Ajout d'un sommet dans le graphe pour cet équipement
        ajouter_sommet(g);
        
        switch (type_equipement) {
            case 2: { // Switch
                // Format: 2;MAC;nb_ports;priorite
                MAC mac;
                int nb_ports;
                unsigned int priorite;
                
                // Lecture de l'adresse MAC
                token = strtok(NULL, ";");
                if (token == NULL) {
                    fprintf(stderr, "Format incorrect pour l'adresse MAC du switch\n");
                    continue;
                }
                // Conversion de la chaîne en adresse MAC
                sscanf(token, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                       &mac.octet[0], &mac.octet[1], &mac.octet[2], 
                       &mac.octet[3], &mac.octet[4], &mac.octet[5]);
                
                // Lecture du nombre de ports
                token = strtok(NULL, ";");
                if (token == NULL || sscanf(token, "%d", &nb_ports) != 1) {
                    fprintf(stderr, "Format incorrect pour le nombre de ports\n");
                    continue;
                }
                
                // Lecture de la priorité
                token = strtok(NULL, ";");
                if (token == NULL || sscanf(token, "%u", &priorite) != 1) {
                    fprintf(stderr, "Format incorrect pour la priorité\n");
                    continue;
                }
                
                // Initialisation du switch
                switchs[nb_switchs].mac = mac;
                switchs[nb_switchs].nb_ports = nb_ports;
                switchs[nb_switchs].priorite = priorite;
                switchs[nb_switchs].table.entrees = NULL;
                switchs[nb_switchs].table.taille = 0;
                
                nb_switchs++;
                break;
            }
            case 1: { // Station
                // Format: 1;MAC;IP
                MAC mac;
                IPv4 ip;
                
                // Lecture de l'adresse MAC
                token = strtok(NULL, ";");
                if (token == NULL) {
                    fprintf(stderr, "Format incorrect pour l'adresse MAC de la station\n");
                    continue;
                }
                // Conversion de la chaîne en adresse MAC
                sscanf(token, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                       &mac.octet[0], &mac.octet[1], &mac.octet[2], 
                       &mac.octet[3], &mac.octet[4], &mac.octet[5]);
                
                // Lecture de l'adresse IP
                token = strtok(NULL, ";");
                if (token == NULL) {
                    fprintf(stderr, "Format incorrect pour l'adresse IP\n");
                    continue;
                }
                // Conversion de la chaîne en adresse IP
                sscanf(token, "%hhu.%hhu.%hhu.%hhu", 
                       &ip.octet[0], &ip.octet[1], &ip.octet[2], &ip.octet[3]);
                
                // Initialisation de la station
                stations[nb_stations].mac = mac;
                stations[nb_stations].ip = ip;
                
                nb_stations++;
                break;
            }
            default:
                fprintf(stderr, "Type d'équipement inconnu: %d\n", type_equipement);
                break;
        }
    }
    
    // Lecture des liens
    for (int i = 0; i < nombre_liens; i++) {
        if ((longueur = getline(&ligne, &taille_ligne, f)) == -1) {
            fprintf(stderr, "Fin de fichier inattendue lors de la lecture des liens\n");
            break;
        }
        
        int equipement1, equipement2, poids;
        if (sscanf(ligne, "%d;%d;%d", &equipement1, &equipement2, &poids) != 3) {
            fprintf(stderr, "Format incorrect pour le lien: %s", ligne);
            continue;
        }
        
        // Vérifier que les indices correspondent aux sommets du graphe
        if (equipement1 >= g->ordre || equipement2 >= g->ordre || equipement1 < 0 || equipement2 < 0) {
            fprintf(stderr, "Indice d'équipement invalide pour le graphe: %d ou %d (graphe a %ld sommets)\n", 
                    equipement1, equipement2, g->ordre);
            continue;
        }
        
        // Debug pour voir ce qui se passe
        printf("Ajout arête: %d -> %d\n", equipement1, equipement2);
        
        // Ajouter l'arête au graphe
        arete a = {equipement1, equipement2};
        if (!ajouter_arete(g, a)) {
            fprintf(stderr, "Impossible d'ajouter l'arête entre %d et %d\n", equipement1, equipement2);
        }
    }
    free(ligne);
    fclose(f);
    
    // Affichage des informations de la configuration chargée
    printf("Configuration réseau chargée avec succès:\n");
    printf("\n=================== En-tête ==================\n");
    printf("- %d équipements (%d switchs, %d stations)\n", nombre_equipements, nb_switchs, nb_stations);
    printf("- %zu liens\n", nb_aretes(g));
    
    // Afficher les switchs
    printf("\n\n==================== Switchs ==================\n");
    for (int i = 0; i < nb_switchs; i++) {
        printf("Switch %d - MAC: %02X:%02X:%02X:%02X:%02X:%02X | Ports: %d | Priorité: %d\n", 
               i,
               switchs[i].mac.octet[0], switchs[i].mac.octet[1], switchs[i].mac.octet[2],
               switchs[i].mac.octet[3], switchs[i].mac.octet[4], switchs[i].mac.octet[5],
               switchs[i].nb_ports, switchs[i].priorite);
    }
    
    // Afficher les stations
    printf("\n\n==================== Stations ==================\n");
    for (int i = 0; i < nb_stations; i++) {
        printf("Station %d - MAC: %02X:%02X:%02X:%02X:%02X:%02X | IP: %d.%d.%d.%d\n", 
               i,
               stations[i].mac.octet[0], stations[i].mac.octet[1], stations[i].mac.octet[2],
               stations[i].mac.octet[3], stations[i].mac.octet[4], stations[i].mac.octet[5],
               stations[i].ip.octet[0], stations[i].ip.octet[1], 
               stations[i].ip.octet[2], stations[i].ip.octet[3]);
    }
    
    // Libérer la mémoire
    free(switchs);
    free(stations);
    
    return 1;
}

/**
 * Charge la configuration réseau depuis un fichier et crée les structures
 * 
 * Cette fonction étend charger_configuration() pour également créer
 * les structures switch_t et station_t nécessaires à la simulation
 */
int charger_configuration_complete(const char *nom_fichier, configuration_reseau_t *config) {
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }
    
    char *ligne = NULL;
    size_t taille_ligne = 0;
    size_t longueur;
    int nombre_equipements, nombre_liens;
    
    // Lecture de la ligne d'en-tête
    if ((longueur = getline(&ligne, &taille_ligne, f)) == -1 || 
        sscanf(ligne, "%d %d", &nombre_equipements, &nombre_liens) != 2) {
        fprintf(stderr, "Format de la première ligne incorrect\n");
        free(ligne);
        fclose(f);
        return 0;
    }
    
    // Initialisation du graphe
    config->g = malloc(sizeof(graphe));
    if (!config->g) {
        fclose(f);
        return 0;
    }
    init_graphe(config->g);
    
    // Allocation des tableaux
    config->switches = malloc(nombre_equipements * sizeof(switch_t));
    config->stations = malloc(nombre_equipements * sizeof(station_t));
    
    if (!config->switches || !config->stations) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        free(config->switches);
        free(config->stations);
        free(config->g);
        free(ligne);
        fclose(f);
        return 0;
    }
    
    config->nb_switches = 0;
    config->nb_stations = 0;
    
    // Lecture des équipements
    for (int i = 0; i < nombre_equipements; i++) {
        if ((longueur = getline(&ligne, &taille_ligne, f)) == -1) {
            fprintf(stderr, "Fin de fichier inattendue\n");
            free(config->switches);
            free(config->stations);
            free(config->g);
            free(ligne);
            fclose(f);
            return 0;
        }
        
        int type_equipement;
        char *token = strtok(ligne, ";");
        if (token == NULL || sscanf(token, "%d", &type_equipement) != 1) {
            fprintf(stderr, "Format incorrect pour le type d'équipement\n");
            continue;
        }
        
        // Ajout d'un sommet dans le graphe
        ajouter_sommet(config->g);
        
        switch (type_equipement) {
            case 2: { // Switch
                MAC mac;
                int nb_ports;
                unsigned int priorite;
                
                // Lecture MAC
                token = strtok(NULL, ";");
                if (token == NULL) {
                    fprintf(stderr, "Format incorrect pour l'adresse MAC du switch\n");
                    continue;
                }
                sscanf(token, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                       &mac.octet[0], &mac.octet[1], &mac.octet[2], 
                       &mac.octet[3], &mac.octet[4], &mac.octet[5]);
                
                // Lecture nombre de ports
                token = strtok(NULL, ";");
                if (token == NULL || sscanf(token, "%d", &nb_ports) != 1) {
                    fprintf(stderr, "Format incorrect pour le nombre de ports\n");
                    continue;
                }
                
                // Lecture priorité
                token = strtok(NULL, ";");
                if (token == NULL || sscanf(token, "%u", &priorite) != 1) {
                    fprintf(stderr, "Format incorrect pour la priorité\n");
                    continue;
                }
                
                // Création du switch
                config->switches[config->nb_switches] = creer_switch(mac, nb_ports, priorite);
                
                // Activer tous les ports avec coût 10
                for (int p = 0; p < nb_ports; p++) {
                    activer_port(&config->switches[config->nb_switches], p);
                    config->switches[config->nb_switches].ports[p].cost = 10;
                }
                
                config->nb_switches++;
                break;
            }
            case 1: { // Station
                MAC mac;
                IPv4 ip;
                
                // Lecture MAC
                token = strtok(NULL, ";");
                if (token == NULL) {
                    fprintf(stderr, "Format incorrect pour l'adresse MAC de la station\n");
                    continue;
                }
                sscanf(token, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                       &mac.octet[0], &mac.octet[1], &mac.octet[2], 
                       &mac.octet[3], &mac.octet[4], &mac.octet[5]);
                
                // Lecture IP
                token = strtok(NULL, ";");
                if (token == NULL) {
                    fprintf(stderr, "Format incorrect pour l'adresse IP\n");
                    continue;
                }
                sscanf(token, "%hhu.%hhu.%hhu.%hhu", 
                       &ip.octet[0], &ip.octet[1], &ip.octet[2], &ip.octet[3]);
                
                // Création de la station
                config->stations[config->nb_stations] = creer_station(mac, ip);
                config->nb_stations++;
                break;
            }
            default:
                fprintf(stderr, "Type d'équipement inconnu: %d\n", type_equipement);
                break;
        }
    }
    
    // Lecture des liens
    for (int i = 0; i < nombre_liens; i++) {
        if ((longueur = getline(&ligne, &taille_ligne, f)) == -1) {
            fprintf(stderr, "Fin de fichier inattendue lors de la lecture des liens\n");
            break;
        }
        
        int equipement1, equipement2, poids;
        if (sscanf(ligne, "%d;%d;%d", &equipement1, &equipement2, &poids) != 3) {
            fprintf(stderr, "Format incorrect pour le lien: %s", ligne);
            continue;
        }
        
        // Vérifier la validité des indices
        if (equipement1 >= config->g->ordre || equipement2 >= config->g->ordre || 
            equipement1 < 0 || equipement2 < 0) {
            fprintf(stderr, "Indice d'équipement invalide: %d ou %d\n", equipement1, equipement2);
            continue;
        }
        
        // Ajouter l'arête
        arete a = {equipement1, equipement2};
        if (!ajouter_arete(config->g, a)) {
            fprintf(stderr, "Impossible d'ajouter l'arête entre %d et %d\n", equipement1, equipement2);
        }
    }
    
    free(ligne);
    fclose(f);
    
    // Affichage de la configuration chargée
    printf("Configuration réseau chargée avec succès:\n");
    printf("- %d équipements (%d switches, %d stations)\n", 
           nombre_equipements, config->nb_switches, config->nb_stations);
    printf("- %zu liens\n", nb_aretes(config->g));
    
    return 1;
}

/**
 * Libère la mémoire allouée pour la configuration
 */
void liberer_configuration(configuration_reseau_t *config) {
    if (config->switches) {
        for (int i = 0; i < config->nb_switches; i++) {
            deinit_switch(&config->switches[i]);
        }
        free(config->switches);
    }
    if (config->stations) {
        free(config->stations);
    }
    if (config->g) {
        deinit_graphe(config->g);
        free(config->g);
    }
}