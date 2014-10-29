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
		if(lesNoeuds[noeud].partieVariable == 0) {
			streampos position = DEBUT + (28 * noeud);

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
	//DATA >> a;
	DATA.read(reinterpret_cast<char*>(&a), sizeof(float));
}

uint32_t graphe::size() const
{
	return this->nbNOEUDS;
}

void graphe::afficher_noeud(uint32_t noeud)
{
	this->lire_noeud(noeud);

	cout << "+-----------------------------------------+" << endl;
	cout << " Noeud #" << noeud << endl;
	cout << " - PartieVariable: " << lesNoeuds[noeud].partieVariable << endl;
	cout << " - Latitude: " << lesNoeuds[noeud].latitude << endl;
	cout << " - Longitude: " << lesNoeuds[noeud].longitude << endl;
	for(int i = 0; i < 4; ++i) {
		cout << " -> Future[" << i << "]: " << lesNoeuds[noeud].futur[i] << endl;
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
