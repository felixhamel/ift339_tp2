#include "graphe.h"

int main()
{
	uint32_t numero;

	graphe USA("grUSA_32.ibin");

	cout << "Le graphe contient " << USA.size() << " noeuds." << endl;

	while(true){
		cin >> numero;
		if(numero < 0) {
			cout << "Erreur: Le # du noeud doit être >= 0." << endl;
		} else if(numero >= USA.size()) {
			cout << "Erreur: Le # du noeud est plus élevé que le nombre de noeud dans ce graphe." << endl;
		} else {
			USA.afficher_noeud(numero);
		}
	}

	return 0;
}
