#include "graphe.h"

#include <set>
#include <chrono>
#include <ctime>

graphe::graphe(string cheminVersFichier)
{
	DATA.open(cheminVersFichier.c_str(), ios::in|ios::binary);
	if(!DATA.is_open()) {
		cout << "Erreur d'ouverture du fichier, celui-ci n'existe pas." << endl;
		exit(1);
	} else {
		DATA >> nom;
		DATA.ignore(1);
		DATA >> nbNOEUDS;
		DATA.ignore(1);
		DATA >> architecture;
		DATA.ignore(1);

		DEBUT = DATA.tellg();

// Afficher une description du graphe si on est en debug
#ifdef _DEBUG
		cout << "Description du graphe : " << endl;
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
#endif

	}
}

graphe::~graphe()
{
	// Fermer le fichier a la sortie du programme.
	if (DATA.is_open()) {
		DATA.close();
	}
}

void graphe::lire_noeud(uint32_t noeud)
{
	if(noeud < nbNOEUDS) {
		// Si le noeud n'a jamais été lu, alors il va l'être !
		if(lesNoeuds[noeud].partieVariable == 0) {

			// Lecture des données statiques du noeud
			DATA.seekg(DEBUT + (28 * noeud), ios::beg);
			this->lire(lesNoeuds[noeud].partieVariable);
			this->lire(lesNoeuds[noeud].latitude);
			this->lire(lesNoeuds[noeud].longitude);
			for(int i = 0; i < 4; ++i) {
				this->lire(lesNoeuds[noeud].futur[i]);
			}

			// Lecture des données variable du noeud
			DATA.seekg(lesNoeuds[noeud].partieVariable);
			this->lire(lesNoeuds[noeud].nbArcs);
			for(int i = 0; i < lesNoeuds[noeud].nbArcs; ++i) {
				uint32_t numero;
				this->lire(numero);
				this->lire(lesNoeuds[noeud].liens[numero]);
		  }

			uint16_t nombreDeCaracteres;
			this->lire(nombreDeCaracteres);

			char* nom = new char[nombreDeCaracteres];
			DATA.read(nom, nombreDeCaracteres);
			lesNoeuds[noeud].nom = nom;

			delete nom;
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
	cout << " - Nom: " << leNoeud.nom << endl;
	for(int i = 0; i < 4; ++i) {
		cout << " -> Future[" << i << "]: " << leNoeud.futur[i] << endl;
	}
	cout << " - Nombre d'arcs: " << leNoeud.nbArcs << endl;
	for(map<uint32_t, float>::iterator it = leNoeud.liens.begin(); it != leNoeud.liens.end(); ++it) {
		cout << " -> Arc vers le noeud " << it->first << " avec un poids de " << it->second << endl;
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
	map<uint32_t, float> total;								  // Noeud, poids
	multimap<float, uint32_t> totalInverse;		  // Poids, Noeuds
	set<uint32_t> noeudsObserve;								// Les noeuds déjà observés
  uint32_t noeudCourant = premierNoeud;

	// On va ajouter 0 dans le total et le total inversé afin d'éviter de faire du
	// code custom juste pour la première boucle.
	total[premierNoeud] = 0;
	totalInverse.insert(pair<float, uint32_t>(0, premierNoeud));

	// Chrono
	chrono::time_point<chrono::system_clock> debut, fin;
	debut = chrono::system_clock::now();

	// Boucle afin de déterminer le chemin le plus optimal pour se rendre entre
	// le premier noeud et le deuxième.
	while(noeudCourant != secondNoeud) {

			// Aller chercher le noeud avec le poids le plus petit en partant
			while(noeudsObserve.find(noeudCourant) != noeudsObserve.end() && totalInverse.size() > 0) {
				noeudCourant = totalInverse.begin()->second;
				totalInverse.erase(totalInverse.begin());
			}

			// Si TotalInverse est vide et qu'on n'est plus sur le premier noeud,
			// alors on a fait le tour du graphe et on peut maintenant savoir le
			// chemin le plus efficace à l'aide du map des prédécesseurs.
			if(totalInverse.size() == 0 && noeudCourant != premierNoeud) {
				break;
			}

			// Puisqu'on a trouvé un noeud qu'on a pas encore parcouru, on l'ajoute a
			// la liste des noeuds parcourus et on le retire de TotalInverse.
			noeudsObserve.insert(noeudCourant);
			totalInverse.erase(totalInverse.begin());

			// Si on est rendu au noeud qu'on voulait trouver, on a trouvé le chemin
			// le plus optimal !
			if(noeudCourant != secondNoeud) {
				this->lire_noeud(noeudCourant);
				//cout << "Noeud Courant : " << noeudCourant << " | Prédécesseur: " << predecesseurs[noeudCourant] << endl;

				for(map<uint32_t, float>::iterator it = lesNoeuds[noeudCourant].liens.begin(); it != lesNoeuds[noeudCourant].liens.end(); ++it) {

					// On conserve seulement le chemin avec le plus petit poids
					float poidsTotal = it->second + total[noeudCourant];
					//cout << "Noeud: " << it->first << " | Poids: " << poidsTotal << endl;

					if(total[it->first] == 0 || total[it->first] > poidsTotal) {
						//cout << " -> Mise à jour prédécesseur # Noeud: " << it->first << " | Ancien: " << predecesseurs[noeudCourant] << " | Nouveau: " << noeudCourant << endl;
						total[it->first] = poidsTotal;
						totalInverse.insert(pair<float, uint32_t>(poidsTotal, it->first));
						predecesseurs[it->first] = noeudCourant;
					}
				}

				/*for(multimap<float, uint32_t>::iterator it = totalInverse.begin(); it != totalInverse.end(); ++it) {
					cout << "TotalInverse Noeud: " << it->second << " | Poids: " << it->first << endl;
				}
				cout << "# ------------------------------------ #" << endl;*/
			}

	}

	// On veut savoir le temps qu'a pris la méthode pour trouver le chemin le
	// plus optimal.
	fin = chrono::system_clock::now();
	chrono::duration<double> tempsEcoule = fin - debut;
	cout << "Temps pour trouvé le meilleur chemin = " << tempsEcoule.count() << "s." << endl;
}
