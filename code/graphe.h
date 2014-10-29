#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdint.h> // Compatibilité avec uint32_t

using namespace std;

class graphe {
private:
  struct noeud {                   // Description de toutes les composantes d'un noeud
	  // Statique
    uint32_t partieVariable;
	  float	latitude;
	  float longitude;
	  uint32_t futur[4]; // Autres trucs pour un usage futur

    // Variable
    
  };
  map<uint32_t, noeud> lesNoeuds; // Les noeuds deja lus
  uint32_t nbNOEUDS;              // Le nombre de noeuds
  ifstream DATA;                  // Le flot d'entrée
  uint32_t DEBUT;                 // Debut de la partie fixe
  string nom;				              // Nom du graphe
  uint8_t architecture;           // Architecture du fichier (Little or Big endian)

  void lire_noeud(uint32_t noeud);
  void lire(uint16_t& noeud);           // Fonction utilitaire de lecture binaire
  void lire(uint32_t& noeud);           // qui dépendent de l'architecture
  void lire(float& a);
  graphe(const graphe& graphe)=delete;            // Copieur
  graphe& operator=(const graphe& graphe)=delete; // affectateur désésactivés

public:
  graphe(string cheminVersFichier);                   // Constructeur
  ~graphe();                        // Destructeur
  uint32_t size()const;             // Nombre de noeuds dans le graphe
  void afficher_noeud(uint32_t noeud);    // Afficher toutes les informations sur un noeud
};
