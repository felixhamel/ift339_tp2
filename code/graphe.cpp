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
