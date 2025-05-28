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
 * @param nom_fichier Nom du fichier de configuration
 * @param g Pointeur vers le graphe à remplir
 * @return 1 si le chargement a réussi, 0 sinon
 */
int charger_configuration(const char *nom_fichier, graphe *g) {
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }
    
    char ligne[MAX_LIGNE];
    int nombre_equipements, nombre_liens;
    
    // Lecture de la ligne d'en-tête (nb_equipements nb_liens)
    if (fgets(ligne, MAX_LIGNE, f) == NULL || sscanf(ligne, "%d %d", &nombre_equipements, &nombre_liens) != 2) {
        fprintf(stderr, "Format de la première ligne incorrect\n");
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
        fclose(f);
        return 0;
    }
    
    int nb_switchs = 0;
    int nb_stations = 0;
    
    // Lecture des équipements
    for (int i = 0; i < nombre_equipements; i++) {
        if (fgets(ligne, MAX_LIGNE, f) == NULL) {
            fprintf(stderr, "Fin de fichier inattendue\n");
            free(switchs);
            free(stations);
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
        if (fgets(ligne, MAX_LIGNE, f) == NULL) {
            fprintf(stderr, "Fin de fichier inattendue lors de la lecture des liens\n");
            break;
        }
        
        int equipement1, equipement2, poids;
        if (sscanf(ligne, "%d;%d;%d", &equipement1, &equipement2, &poids) != 3) {
            fprintf(stderr, "Format incorrect pour le lien: %s", ligne);
            continue;
        }
        
        // Vérifier que les indices sont valides
        if (equipement1 >= nombre_equipements || equipement2 >= nombre_equipements) {
            fprintf(stderr, "Indice d'équipement invalide: %d ou %d\n", equipement1, equipement2);
            continue;
        }
        
        // Ajouter l'arête au graphe
        arete a = {equipement1, equipement2};
        if (!ajouter_arete(g, a)) {
            fprintf(stderr, "Impossible d'ajouter l'arête entre %d et %d\n", equipement1, equipement2);
        }
    }
    
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

