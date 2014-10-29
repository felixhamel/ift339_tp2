#include "graphe.h"

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
		cout << "Debut: " << DEBUT << endl;
		cout << "Architecture: ";
		if(architecture == 0) {
			cout << "BigEndian";
		} else if (architecture == 1) {
			cout << "LittleEndian";
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

		streampos position = DEBUT + (28 * noeud);

		if(lesNoeuds[noeud].partieVariable == 0) {
			DATA.clear();
			DATA.seekg(position, ios::beg);

			// Lecture des données statiques du noeud
			this->lire(lesNoeuds[noeud].partieVariable);
			this->lire(lesNoeuds[noeud].latitude);
			this->lire(lesNoeuds[noeud].longitude);
			for(int i = 0; i < 4; ++i) {
				this->lire(lesNoeuds[noeud].futur[i]);
			}

		} else {
			cout << "DEBUG: Le noeud # " << noeud << " est déjà présent dans la mémoire." << endl;
		}

		// Lecture des données variable du noeud (peut être modifié durant l'exécution du programme)
		DATA.clear();
		position = lesNoeuds[noeud].partieVariable;
		DATA.seekg(position);

		cout << "Position: " << DATA.tellg() << endl;
		this->lire(lesNoeuds[noeud].nbArcs);
		cout << "Position: " << DATA.tellg() << endl;

		lesNoeuds[noeud].liens.clear(); 	// On va repopuler ce map
		for(int i = 0; i < lesNoeuds[noeud].nbArcs; ++i) {
			uint32_t numero;
			float poids;
			this->lire(numero);
			this->lire(poids);
			lesNoeuds[noeud].liens[numero] = poids;
		}

	}
}

void graphe::lire(uint16_t& noeud)
{
	DATA.read(reinterpret_cast<char*>(&noeud), 2);
}

void graphe::lire(uint32_t& noeud)
{
	DATA.read(reinterpret_cast<char*>(&noeud), 4);
}

void graphe::lire(float& a)
{
	DATA.read(reinterpret_cast<char*>(&a), sizeof(float));
}

uint32_t graphe::size() const
{
	return this->nbNOEUDS;
}

void graphe::afficher_noeud(uint32_t noeud)
{
	this->lire_noeud(noeud);

	auto leNoeud = lesNoeuds[noeud];

	cout << "+-----------------------------------------+" << endl;
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
	cout << endl;
}

/*graphe::graphe(string cheminVersFichier)
 {
 DATA.open(cheminVersFichier);

 if(!DATA.is_open()) {
 cout << "Fichier inexistant (" << cheminVersFichier << ")" << endl;
 } else {

 // Lire le nom du graphe
 while(DATA.read() != ' ') {
 }

 // Le nombre de noeuds présent dans le fichier
 DATA >> nbNOEUDS;
 }
 }*/
