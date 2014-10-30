#include "graphe.h"

#include <set>

graphe::graphe(string cheminVersFichier)
{
	DATA.open(cheminVersFichier.c_str(), ios::in|ios::binary);
	if(!DATA.is_open()) {
		cout << "Le fichier n'existe pas." << endl;
	} else {
		DATA >> nom;
		DATA.ignore(1);
		DATA >> nbNOEUDS;
		DATA.ignore(1);
		DATA >> architecture;
		DATA.ignore(1);

		DEBUT = DATA.tellg();

		cout << "Nom: " << nom <<endl;
		cout << "NbNoeuds: " << nbNOEUDS << endl;
		cout << "Architecture fichier: ";

		if(architecture == 0) {
			cout << "BigEndian";
		} else if (architecture == 1) {
			cout << "LittleEndian";
		}

		cout << endl << "Architecture machine: ";
		if(this->architectureMachine() == __LITTLE_ENDIAN) {
			cout << "LittleEndian";
		} else {
			cout << "BigEndian";
		}

		cout << endl;
		cout << "----------------------------------------" << endl;
	}
}

graphe::~graphe()
{
	if (DATA.is_open()) {
		DATA.close();
	}
}

void graphe::lire_noeud(uint32_t noeud)
{
	if(noeud < nbNOEUDS) {
		// Si le noeud n'a jamais été lu, alors il va l'être !
		if(lesNoeuds[noeud].partieVariable == 0) {
			DATA.clear();
			DATA.seekg(DEBUT + (28 * noeud), ios::beg);

			// Lecture des données statiques du noeud
			this->lire(lesNoeuds[noeud].partieVariable);
			this->lire(lesNoeuds[noeud].latitude);
			this->lire(lesNoeuds[noeud].longitude);

			for(int i = 0; i < 4; ++i) {
				this->lire(lesNoeuds[noeud].futur[i]);
			}

			// Lecture des données variable du noeud
			DATA.clear();
			DATA.seekg(lesNoeuds[noeud].partieVariable);

			this->lire(lesNoeuds[noeud].nbArcs);
			for(int i = 0; i < lesNoeuds[noeud].nbArcs; ++i) {
				uint32_t numero;
				float poids;
				this->lire(numero);
				this->lire(poids);
				lesNoeuds[noeud].liens[numero] = poids;
		  }
	  }
	}
}

void graphe::lire(uint16_t& noeud)
{
	DATA.read(reinterpret_cast<char*>(&noeud), 2);

	// Si l'architecture diffère du fichier, on swap les bits.
	int architectureMachine = this->architectureMachine();
	if((architecture == 1 && architectureMachine != __LITTLE_ENDIAN) ||
		(architecture == 0 && architectureMachine != __BIG_ENDIAN)) {
			// http://stackoverflow.com/a/2182184
			noeud = (noeud >> 8) | (noeud << 8);
	}
}

void graphe::lire(uint32_t& noeud)
{
	DATA.read(reinterpret_cast<char*>(&noeud), 4);

	// Si l'architecture diffère du fichier, on swap les bits.
	int architectureMachine = this->architectureMachine();
	if((architecture == 1 && architectureMachine != __LITTLE_ENDIAN) ||
		(architecture == 0 && architectureMachine != __BIG_ENDIAN)) {
			// http://stackoverflow.com/a/13001420
			noeud = (noeud >> 24) | ((noeud << 8) & 0x00FF0000) | ((noeud >> 8) & 0x0000FF00) | (noeud << 24);
	}
}

void graphe::lire(float& a)
{
	DATA.read(reinterpret_cast<char*>(&a), 4);

	// Si l'architecture diffère du fichier, on swap les bits.
	int architectureMachine = this->architectureMachine();
	if((architecture == 1 && architectureMachine != __LITTLE_ENDIAN) ||
		(architecture == 0 && architectureMachine != __BIG_ENDIAN)) {
			char *floatToConvert = ( char* ) & a; // http://stackoverflow.com/a/2782742
			swap(floatToConvert[0], floatToConvert[3]);
			swap(floatToConvert[1], floatToConvert[2]);
	}
}

const uint32_t graphe::size() const
{
	return this->nbNOEUDS;
}

void graphe::afficher_noeud(uint32_t noeud)
{
	this->lire_noeud(noeud);

	auto leNoeud = lesNoeuds[noeud];

	cout << "+--------------------------------------------------------------------+" << endl;
	cout << " Noeud #" << noeud << endl;
	cout << " - PartieVariable: " << leNoeud.partieVariable << endl;
	cout << " - Latitude: " << leNoeud.latitude << endl;
	cout << " - Longitude: " << leNoeud.longitude << endl;
	for(int i = 0; i < 4; ++i) {
		cout << " -> Future[" << i << "]: " << leNoeud.futur[i] << endl;
	}
	cout << " - Nombre d'arcs: " << leNoeud.nbArcs << endl;
	for(map<uint32_t, float>::iterator it = leNoeud.liens.begin(); it != leNoeud.liens.end(); ++it) {
		cout << " -> Arc vers le noeud #" << it->first << " avec un poids de " << it->second << endl;
	}
	cout << "+--------------------------------------------------------------------+" << endl;
}

const int graphe::architectureMachine() const
{
	short int word = 0x0001;
	char *byte = (char *) &word;
	return(byte[0] ? __LITTLE_ENDIAN : __BIG_ENDIAN);
}

void graphe::trouver_chemin_optimal(uint32_t premierNoeud, uint32_t secondNoeud)
{
	map<uint32_t, uint32_t> predecesseurs;			// Noeud, prédécesseur du noeud
	map<uint32_t, float> total;								// Noeud, poids
	multimap<float, uint32_t> totalInverse;		// Poids, Noeuds
	set<uint32_t> noeudObserve;								// Les noeuds déjà observés

  uint32_t noeudCourant = premierNoeud;
	total[premierNoeud] = 0;
	totalInverse.insert(pair<float, uint32_t>(0, premierNoeud));

  int compteur = 0;
	while(noeudCourant != secondNoeud) {

			// Aller chercher le noeud avec le poids le plus petit en partant
			while(noeudObserve.find(noeudCourant) != noeudObserve.end()) {
				cout << "###### NC: " << noeudCourant << endl;
				noeudCourant = totalInverse.begin()->second;
				totalInverse.erase(totalInverse.begin());
			}

			if(noeudCourant == secondNoeud) {
				cout << "Chemin trouvé !" << endl;
				break;
			}

			totalInverse.erase(totalInverse.begin());
			noeudObserve.insert(noeudCourant);
			this->lire_noeud(noeudCourant);
			noeud noeud = lesNoeuds[noeudCourant];
			cout << "Noeud Courant : " << noeudCourant << " | Prédécesseur: " << predecesseurs[noeudCourant] << endl;

			for(map<uint32_t, float>::iterator it = noeud.liens.begin(); it != noeud.liens.end(); ++it) {

				cout << "Noeud: " << it->first << " | Poids: " << (it->second + total[noeudCourant]) << endl;
				// On conserve seulement le chemin avec le plus petit poids
				if(total[it->first] > (it->second + total[noeudCourant]) || total[it->first] == 0) {
					cout << " -> Mise à jour prédécesseur # Noeud: " << it->first << " | Ancien: " << predecesseurs[noeudCourant] << " | Nouveau: " << noeudCourant << endl;
					total[it->first] = it->second + total[noeudCourant];
					totalInverse.insert(pair<float, uint32_t>(it->second + total[noeudCourant], it->first));
					predecesseurs[it->first] = noeudCourant;
				}

			}

			cout << "TotalInverse Noeud: " << totalInverse.begin()->second << " | Poids: " << totalInverse.begin()->first << endl;

			/*for(multimap<float, uint32_t>::iterator it = totalInverse.begin(); it != totalInverse.end(); ++it)
			{
				cout << "TotalInverse Noeud: " << it->second << " | Poids: " << it->first << endl;
			}*/

			cout << "# ------------------------------------ #" << endl;
			compteur++;

	}

	cout << "Compteur !: " << compteur << endl;

  cout << "Affichage des noeuds" << endl;
	for(map<uint32_t, uint32_t>::iterator it = predecesseurs.begin(); it != predecesseurs.end(); ++it) {
		cout << "Noeud: " << it->first << " <- " << it->second << endl;
	}

}
