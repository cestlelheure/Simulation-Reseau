# Simulation Réseau - SAE 2.3 🌐

[English version below](#english-version)

## 📡 Description

**Simulation Réseau** est un projet de simulation d'architecture réseau local développé en langage C. Le projet implémente les protocoles Ethernet et Spanning Tree Protocol (STP) pour modéliser le fonctionnement d'un réseau local avec stations et switches.

## 👥 Équipe de développement

- **Andy PHAN** ([@cestlelheure](https://github.com/cestlelheure))
- **Arthur ALEXANDRE** ([@ArthurALEXANDRE-29](https://github.com/ArthurALEXANDRE-29))
- **Adrien THIERRY**

## 🎯 Objectifs du projet

Le projet démontre notre capacité à :
- Manipuler des structures de données représentant un graphe étiqueté
- Concevoir des structures de données pour modéliser des équipements réseau (stations, switches)
- Utiliser ces structures pour modéliser une architecture de réseau local
- Comprendre et implémenter le protocole Ethernet pour la diffusion de messages
- Comprendre et implémenter le protocole STP avec l'algorithme de synchronisation par BPDU

## 🔧 Fonctionnalités

### Étape 1 : Structures de données
- **Adresses réseau** : Représentation des adresses MAC (Ethernet) et IPv4
- **Station** : Modélisation avec adresse MAC et adresse IP
- **Switch** : Modélisation avec adresse MAC, nombre de ports, priorité STP et table de commutation
- **Réseau local** : Structure basée sur un graphe étiqueté

### Étape 2 : Fichiers de configuration
- Chargement d'architectures réseau depuis des fichiers de configuration
- Format standardisé pour décrire équipements et interconnexions
- Support des poids de liens selon les débits (10Mb/s, 100Mb/s, 1Gb/s)

### Étape 3 : Trames Ethernet
- Structure de données fidèle au format réel d'une trame Ethernet
- Affichage en mode utilisateur et hexadécimal
- Simulation de la commutation de trames dans le réseau

### Étape 4 : Spanning Tree Protocol (STP)
- Implémentation complète du protocole STP
- Échange de BPDU entre switches
- Configuration automatique des ports (port racine, ports désignés, ports bloqués)
- Convergence du protocole pour éliminer les boucles

## 🛠️ Technologies utilisées

- **Langage** : C
- **Structures de données** : Graphes étiquetés (librairie M23)
- **Protocoles** : Ethernet, STP (Spanning Tree Protocol)
- **Build** : Makefile

## 📁 Structure du projet

```
.
├── README.md
├── Makefile
├── src/
│   ├── main.c
│   ├── station.c
│   ├── station.h
│   ├── switch.c
│   ├── switch.h
│   ├── ethernet.c
│   ├── ethernet.h
│   ├── stp.c
│   ├── stp.h
│   └── ...
|   └── exemple_reseau.txt
```

## 🚀 Compilation et exécution

### Prérequis
- Compilateur GCC
- Make
- Librairie de graphes (M23)

### Compilation
```bash
# Compiler le projet
make

# Nettoyer les fichiers compilés
make clean
```

### Exécution
```bash
# Lancer la simulation avec un fichier de configuration
./simulation exemple_reseau.txt
```

## 📝 Format des fichiers de configuration

Exemple de fichier de configuration :
```
4 3
2;01:45:23:a6:f7:ab;8;1024
1;54:d6:a6:82:c5:23;130.79.80.21
1;c8:69:72:5e:43:af;130.79.80.27
1;77:ac:d6:82:12:23;130.79.80.42
0;1;4
0;2;19
0;3;4
```

**Format** :
- Ligne 1 : `<nombre_equipements> <nombre_liens>`
- Lignes suivantes (switchs) : `2;<MAC>;<nb_ports>;<priorite>`
- Lignes suivantes (stations) : `1;<MAC>;<IP>`
- Lignes de liens : `<equipement1>;<equipement2>;<poids>`

**Poids des liens** :
- 10 Mb/s : poids = 100
- 100 Mb/s : poids = 19
- 1 Gb/s : poids = 4

## 📊 Protocole Spanning Tree (STP)

Le protocole STP est implémenté pour :
1. Élire un switch racine (priorité la plus basse)
2. Élire un port racine sur chaque switch non-racine
3. Désigner des ports pour chaque segment
4. Bloquer les ports redondants pour éviter les boucles

Les BPDU sont échangés entre switches jusqu'à convergence du protocole.

## 🧪 Tests

Le projet inclut plusieurs configurations de test pour valider :
- Les topologies en étoile
- Les topologies avec cycles
- Les topologies complexes avec multiples switches
- La convergence du protocole STP

## 📖 Documentation

- **Sujet complet** : Voir le document `2023 - sae.pdf`
- **Protocole Ethernet** : Format de trame avec préambule, SFD, adresses, type, données et FCS
- **Protocole STP** : Échange de BPDU et configuration automatique des ports

## 📝 Licence

Projet académique réalisé à l'Université de Strasbourg.

---

# English Version

## 📡 Description

**Network Simulation** is a local network architecture simulation project developed in C language. The project implements Ethernet and Spanning Tree Protocol (STP) to model the operation of a local network with stations and switches.

## 👥 Development Team

- **Andy PHAN** ([@cestlelheure](https://github.com/cestlelheure))
- **Arthur ALEXANDRE** ([@ArthurALEXANDRE-29](https://github.com/ArthurALEXANDRE-29))
- **Adrien THIERRY**

## 🎯 Project Objectives

The project demonstrates our ability to:
- Manipulate data structures representing a labeled graph
- Design data structures to model network equipment (stations, switches)
- Use these structures to model a local network architecture
- Understand and implement the Ethernet protocol for message broadcasting
- Understand and implement the STP protocol with BPDU synchronization algorithm

## 🔧 Features

### Step 1: Data Structures
- **Network Addresses**: Representation of MAC (Ethernet) and IPv4 addresses
- **Station**: Modeling with MAC and IP address
- **Switch**: Modeling with MAC address, number of ports, STP priority, and switching table
- **Local Network**: Structure based on a labeled graph

### Step 2: Configuration Files
- Loading network architectures from configuration files
- Standardized format to describe equipment and interconnections
- Support for link weights according to bandwidth (10Mb/s, 100Mb/s, 1Gb/s)

### Step 3: Ethernet Frames
- Data structure faithful to the actual Ethernet frame format
- Display in user mode and hexadecimal
- Simulation of frame switching in the network

### Step 4: Spanning Tree Protocol (STP)
- Complete implementation of the STP protocol
- BPDU exchange between switches
- Automatic port configuration (root port, designated ports, blocked ports)
- Protocol convergence to eliminate loops

## 🛠️ Technologies Used

- **Language**: C
- **Data Structures**: Labeled graphs (M23 library)
- **Protocols**: Ethernet, STP (Spanning Tree Protocol)
- **Build**: Makefile

## 📁 Project Structure

```
.
├── README.md
├── Makefile
├── src/
│   ├── main.c
│   ├── station.c
│   ├── station.h
│   ├── switch.c
│   ├── switch.h
│   ├── ethernet.c
│   ├── ethernet.h
│   ├── stp.c
│   ├── stp.h
│   └── ...
|   └── example_network.txt
```

## 🚀 Compilation and Execution

### Prerequisites
- GCC Compiler
- Make
- Graph library (M23)

### Compilation
```bash
# Compile the project
make

# Clean compiled files
make clean
```

### Execution
```bash
# Run the simulation with a configuration file
./simulation example_network.txt
```

## 📝 Configuration File Format

Configuration file example:
```
4 3
2;01:45:23:a6:f7:ab;8;1024
1;54:d6:a6:82:c5:23;130.79.80.21
1;c8:69:72:5e:43:af;130.79.80.27
1;77:ac:d6:82:12:23;130.79.80.42
0;1;4
0;2;19
0;3;4
```

**Format**:
- Line 1: `<equipment_count> <link_count>`
- Following lines (switches): `2;<MAC>;<port_count>;<priority>`
- Following lines (stations): `1;<MAC>;<IP>`
- Link lines: `<equipment1>;<equipment2>;<weight>`

**Link Weights**:
- 10 Mb/s: weight = 100
- 100 Mb/s: weight = 19
- 1 Gb/s: weight = 4

## 📊 Spanning Tree Protocol (STP)

The STP protocol is implemented to:
1. Elect a root switch (lowest priority)
2. Elect a root port on each non-root switch
3. Designate ports for each segment
4. Block redundant ports to avoid loops

BPDUs are exchanged between switches until protocol convergence.

## 🧪 Testing

The project includes several test configurations to validate:
- Star topologies
- Topologies with cycles
- Complex topologies with multiple switches
- STP protocol convergence

## 📖 Documentation

- **Complete Subject**: See document `2023 - sae.pdf`
- **Ethernet Protocol**: Frame format with preamble, SFD, addresses, type, data, and FCS
- **STP Protocol**: BPDU exchange and automatic port configuration


## 📝 License

Academic project carried out at the University of Strasbourg.
