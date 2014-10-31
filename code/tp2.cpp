#include "graphe.h"

using namespace std;

/**
 * Main function.
 * @return code de sortie du programme.
 */
int main()
{
  string nomFichier;

  cout << "Veuillez entrer le nom du fichier a ouvrir : ";
  cin >> nomFichier;

  // Création du graphe
  graphe dijkstra(nomFichier);

  uint16_t premierNoeud = -1;
  uint16_t secondNoeud = -1;

  while(true) {
    do {
      cout << "Entrez le numéro du premier noeud : ";
      cin >> premierNoeud;
    } while(premierNoeud < 0 || premierNoeud >= dijkstra.size());
    do {
      cout << "Entrez le numéro du deuxième noeud : ";
      cin >> secondNoeud;
    } while(secondNoeud < 0 || secondNoeud >= dijkstra.size());
    dijkstra.trouver_chemin_optimal(premierNoeud, secondNoeud);
  }
}
