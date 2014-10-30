#include "graphe.h"

using namespace std;

/**
 * Main function.
 * @return [description]
 */
int main()
{
  cout << "Veuillez entrer le nom du fichier a ouvrir : ";

  string nomFichier;
  cin >> nomFichier;

  // Création du graphe
  graphe dijkstra(nomFichier);

  while(true) {
    uint16_t premierNoeud = 0;
    uint16_t secondNoeud = 0;

    cout << "Entrez le numéro du premier noeud : ";
    cin >> premierNoeud;
    cout << "Entrez le numéro du deuxième noeud : ";
    cin >> secondNoeud;

    dijkstra.trouver_chemin_optimal(premierNoeud, secondNoeud);
  }
}
