/**
 * Implémentation du décodeur Téléinfo
 *
 * @author LK
 */
#include "TeleinfoDecoder.h"

#include <stdlib.h>
#include <string.h>

/*********************************************************************************************************************************************************************
  CONSTANTES
 *********************************************************************************************************************************************************************/

/* Caractères spéciaux du protocole TeleInfo */
#define TELEINFO_CHAR_STX            0x02  // Start of Text
#define TELEINFO_CHAR_ETX            0x03  // End of Text
#define TELEINFO_CHAR_EOT            0x04  // End of Transmission
#define TELEINFO_CHAR_LF             0x0A  // Groupe Feed
#define TELEINFO_CHAR_CR             0x0D  // Carriage return
#define TELEINFO_CHAR_SPACE          0x20  // Space

/*********************************************************************************************************************************************************************
   CLASSES INTERNES
 *********************************************************************************************************************************************************************/

/**
 * Représente un groupe Téléinfo : un ocuple composé d'une étiquette et d'une donnée.
 *
 * Toutes les données du compteur sont délivrées par groupes d'information qui forment chacun un ensemble cohérent
 * avec une étiquette et une valeur associée de telle sorte qu'il soit facile de les distinguer les unes des autres.
 */
class TeleinfoGroupe {
private:
	char etiquette[64];
	unsigned int indexEtiquette;
	char donnee[64];
	unsigned int indexDonnee;
	char checksum;

public:
	/**
       Constructeur
	 */
	TeleinfoGroupe() {
		reset();
	}

	/**
     * RAZ du contenu de la ligne TeleInfo
	 */
	void reset() {
		memset(etiquette, '\0', sizeof(etiquette));
		memset(donnee, '\0', sizeof(donnee));
		indexEtiquette = 0;
		indexDonnee = 0;
		checksum = 0;
	}

	/**
	 * Ajoute un caractère à l'étiquette, le caractère est ignoré si la taille max de létiquette est atteinte
	 */
	void appendToEtiquette(char character) {
		if (indexEtiquette < (sizeof(etiquette) - 1)) { // On laisse un caractère 0x00 de fin pour marquer la fin de la chaîne
			etiquette[indexEtiquette++] = character;
		}
	}

	/**
	 * Ajoute un caractère à la donnée, le caractère est ignoré si la taille max de létiquette est atteinte
	 */
	void appendToDonnee(char character) {
		if (indexDonnee < (sizeof(donnee) - 1)) { // On laisse un caractère 0x00 de fin pour marquer la fin de la chaîne
			donnee[indexDonnee++] = character;
		}
	}

	/**
	 * Définit le checksum
	 */
	void setChecksum(char character) {
		checksum = character;
	}

	/**
	 * Vérifie le checksum
	 *
	 * La "checksum" est calculée sur l'ensemble des caractères allant du début du champ étiquette
	 * à la fin du champ donnée, caractère SP inclus. On fait tout d'abord la somme des codes ASCII
	 * de tous ces caractères. Pour éviter d'introduire des fonctions ASCII (00 à 1F en hexadécimal),
	 * on ne conserve que les six bits de poids faible du résultat obtenu (cette opération se traduit
	 * par un ET logique entre la somme précédemment calculée et 03Fh).
	 * Enfin, on ajoute 20 en hexadécimal.
	 */
	bool check() {
		unsigned int sum = 0;
		for (int i = 0; i < sizeof(etiquette); i++) {
			sum = sum + etiquette[i];
		}
		sum = sum + 0x20;
		for (int i = 0; i < sizeof(donnee); i++) {
			sum = sum + donnee[i];
		}
		sum = (sum & 0x3F) + 0x20;
		if (checksum == sum) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Donne l'étiquette
	 */
	char* getEtiquette() {
		return etiquette;
	}

	/**
	 * Donne la chaîne de donnée
	 */
	char* getDonnee() {
		return donnee;
	}
};


/**
 * L'implémentation de Teleinfo
 */
class TeleinfoImpl : public Teleinfo {
private :
	// Toutes les données du compteur
	// Voir : http://www.worldofgz.com/electronique/recuperer-la-teleinformation-erdf-sur-larduino/
	char adco[12 + 1]; // Adresse du compteur (+1 octet pour une null-terminated-string)
	char optarif[4 + 1]; // Option tarifaire choisie (+1 octet pour une null-terminated-string)
	int isousc; // Intensité souscrite (A)

	// Option BASE
	unsigned long base; // Index option base (Wh)

	// Option Heures Creuses
	unsigned long hchc; // Index option heure creuse (Wh)
	unsigned long hchp; // Index option heure pleine (Wh)

	// Option EJP
	unsigned long ejphn; // Index heures normales (Wh)
	unsigned long ejphpm; // Index heures de pointe mobile (Wh)

	// Option TEMPO
	unsigned long bbrhcjb; // Index heures creuses jours bleus (Wh)
	unsigned long bbrhpjb; // Index heures pleines jours bleus (Wh)
	unsigned long bbrhcjw; // Index heures creuses jours blancs (Wh)
	unsigned long bbrhpjw; // Index heures pleines jours blancs (Wh)
	unsigned long bbrhcjr; // Index heures creuses jours rouges (Wh)
	unsigned long bbrhpjr; // Index heures pleines jours rouges (Wh)

	// Autreslo
	int pejp; // Préavis heures EJP (min)
	char ptec[4 + 1]; // Période tarifaire en cours (+1 octet pour une null-terminated-string)
	char demain[4 + 1]; // Couleur du lendemain (+1 octet pour une null-terminated-string)
	int iinst; // Intensité instantanée (A)
	int adps; // Avertissement de dépassement de puissance souscrite (A)
	int imax; // Intensité maximale appelée (A)
	int papp; // Puissance apparent (VA)
	char hhphc; // Horaire geure creuse heure pleine
	char motdetat[6 + 1]; // Mot d'état du compteur (+1 octet pour une null-terminated-string)

public:

	// Méthodes de l'interface -------------------------------------------------------------------------------------------------

	char* getAdco() {
		return adco;
	}
	char* getOptarif() {
		return optarif;
	}
	int getIsousc() {
		return isousc;
	}
	unsigned long getBase() {
		return base;
	}
	unsigned long getHchc() {
		return hchc;
	}
	unsigned long getHchp() {
		return hchp;
	}
	unsigned long getEjphn() {
		return ejphn;
	}
	unsigned long getEjphpm() {
		return ejphpm;
	}
	unsigned long getBbrhcjb() {
		return bbrhcjb;
	}
	unsigned long getBbrhpjb() {
		return bbrhpjb;
	}
	unsigned long getBbrhcjw() {
		return bbrhcjw;
	}
	unsigned long getBbrhpjw() {
		return bbrhpjw;
	}
	unsigned long getBbrhcjr() {
		return bbrhcjr;
	}
	unsigned long getBbrhpjr() {
		return bbrhpjr;
	}
	int getPejp() {
		return pejp;
	}
	char* getPtec() {
		return ptec;
	}
	char* getDemain() {
		return demain;
	}
	int getIinst() {
		return iinst;
	}
	int getAdps() {
		return adps;
	}
	int getImax() {
		return imax;
	}
	int getPapp() {
		return papp;
	}
	char getHhphc() {
		return hhphc;
	}
	char* getMotdetat() {
		return motdetat;
	}

	// Méthodes pratiques ------------------------------------------------------------------------------------------------------

	unsigned long getTotalIndex() {
		return base + hchc + hchp + ejphn + ejphpm + bbrhcjb + bbrhpjb + bbrhcjw + bbrhpjw + bbrhcjr + bbrhpjr;
	}

	int getInstPower() {
		if(papp > 0) {
				return papp;

		} else if(iinst > 0) {
			return iinst * 230;

		} else{
			return 0;
		}
	}

	// Divers ------------------------------------------------------------------------------------------------------------------

	void reset() {
		memset(adco, '\0', sizeof(adco));
		memset(optarif, '\0', sizeof(optarif));
		isousc = 0;
		base = 0;
		hchc = 0;
		hchp = 0;
		ejphn = 0;
		ejphpm = 0;
		bbrhcjb = 0;
		bbrhpjb = 0;
		bbrhcjw = 0;
		bbrhpjw = 0;
		bbrhcjr = 0;
		bbrhpjr = 0;
		pejp = 0;
		memset(ptec, '\0', sizeof(ptec));
		memset(demain, '\0', sizeof(demain));
		iinst = 0;
		adps = 0;
		imax = 0;
		papp = 0;
		hhphc = '\0';
		memset(motdetat, '\0', sizeof(motdetat));
	}

	/**
	 *  Trasfert les données d'un groupe dans la structire totale Téléinfo
	 */
	void store(TeleinfoGroupe* teleinfoGroupe) {
		char* etiquette = teleinfoGroupe->getEtiquette();
		char* donnee = teleinfoGroupe->getDonnee();
		if (strcmp(etiquette, "ADCO") == 0) {
			strcpy(adco, donnee);

		} else if (strcmp(etiquette, "OPTARIF") == 0) {
			strcpy(optarif, donnee);

		} else if (strcmp(etiquette, "ISOUSC") == 0) {
			isousc = atoi(donnee);

		} else if (strcmp(etiquette, "BASE") == 0) {
			base = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "HCHC") == 0) {
			hchc = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "HCHP") == 0) {
			hchp = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "EJPHN") == 0) {
			ejphn = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "EJPHPM") == 0) {
			ejphpm = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "BBRHCJB") == 0) {
			bbrhcjb = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "BBRHPJB") == 0) {
			bbrhpjb = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "BBRHCJW") == 0) {
			bbrhcjw = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "BBRHPJW") == 0) {
			bbrhpjw = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "BBRHCJR") == 0) {
			bbrhcjr = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "BBRHPJR") == 0) {
			bbrhpjr = strtoul(donnee, NULL, 10);

		} else if (strcmp(etiquette, "PEJP") == 0) {
			pejp = atoi(donnee);

		} else if (strcmp(etiquette, "PTEC") == 0) {
			strcpy(ptec, donnee);

		} else if (strcmp(etiquette, "DEMAIN") == 0) {
			strcpy(demain, donnee);

		} else if (strcmp(etiquette, "IINST") == 0) {
			iinst = atoi(donnee);

		} else if (strcmp(etiquette, "ADPS") == 0) {
			adps = atoi(donnee);

		} else if (strcmp(etiquette, "IMAX") == 0) {
			imax = atoi(donnee);

		} else if (strcmp(etiquette, "PAPP") == 0) {
			papp = atoi(donnee);

		} else if (strcmp(etiquette, "HHPHC") == 0) {
			hhphc = donnee[0];

		} else if (strcmp(etiquette, "MOTDETAT") == 0) {
			strcpy(motdetat, donnee);
		}

	}

};

/*********************************************************************************************************************************************************************
   MACHINE D'ETAT DE DECODAGE DES TRAMES TELEINFO
 *********************************************************************************************************************************************************************/

/**
 * Signature d'un état.
 */
class StateInterface {
public:
	/* Chaque action donne l'état suivant ou lui même. NULL si aucun état suivant trame Téléinfo terminée */
	virtual StateInterface* stx() = 0;
	virtual StateInterface* etx() = 0;
	virtual StateInterface* eot() = 0;
	virtual StateInterface* cr() = 0;
	virtual StateInterface* lf() = 0;
	virtual StateInterface* space() = 0;
	virtual StateInterface* other(char character) = 0;
	virtual Teleinfo* getResult() = 0;
	virtual const char* getName() = 0;
};

/**
 * Donne un accès à tous les états
 */
class StateRegistry {
private:
	StateInterface* waitingStartTextState;
	StateInterface* waitingStartGroupeState;
	StateInterface* readingEtiquetteState;
	StateInterface* readingDonneeState;
	StateInterface* readingChecksumState;
	StateInterface* waitingEndGroupeState;
	StateInterface* waitingEndTextOrStartGroupeState;
	StateInterface* terminatedState;

public:
	StateRegistry(TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl);
	StateInterface* getWaitingStartTextState();
	StateInterface* getWaitingStartGroupeState();
	StateInterface* getReadingEtiquetteState();
	StateInterface* getReadingDonneeState();
	StateInterface* getReadingChecksumState();
	StateInterface* getWaitingEndGroupeState();
	StateInterface* getWaitingEndTextOrStartGroupeState();
	StateInterface* getTerminatedState();
};

/**
 * Etat par défaut
 */
class DefaultState: public StateInterface {
protected:
	StateRegistry* stateRegistry;
	TeleinfoGroupe* teleinfoGroupe;
	TeleinfoImpl* teleinfoImpl;

public:
	DefaultState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) {
		this->stateRegistry = stateRegistry;
		this->teleinfoGroupe = teleinfoGroupe;
		this->teleinfoImpl = teleinfoImpl;
	}
	StateInterface* stx() {
		return stateRegistry->getWaitingStartTextState();
	}
	StateInterface* etx() {
		return stateRegistry->getWaitingStartTextState();
	}
	StateInterface* eot() {
		return stateRegistry->getWaitingStartTextState();
	}
	StateInterface* cr() {
		return stateRegistry->getWaitingStartTextState();
	}
	StateInterface* lf() {
		return stateRegistry->getWaitingStartTextState();
	}
	StateInterface* space() {
		return stateRegistry->getWaitingStartTextState();
	}
	StateInterface* other(char character) {
		return stateRegistry->getWaitingStartTextState();
	}
	Teleinfo* getResult() {
		return NULL;
	}
};

class WaitingStartTextState: public DefaultState {
public:
	WaitingStartTextState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* stx() {
		// Vidage des données du compteur
		teleinfoImpl->reset();
		return stateRegistry->getWaitingStartGroupeState();
	}
	const char* getName() {
		return "WaitingStartTextState";
	}
};

class WaitingStartGroupeState: public DefaultState {
public:
	WaitingStartGroupeState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* lf() {
		teleinfoGroupe->reset(); // Démarrage d'une nouvelle ligne en cours de traitement
		return stateRegistry->getReadingEtiquetteState();
	}
	const char* getName() {
		return "WaitingStartGroupeState";
	}
};

class ReadingEtiquetteState: public DefaultState {
public:
	ReadingEtiquetteState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* space() {
		return stateRegistry->getReadingDonneeState();
	}
	StateInterface* other(char character) {
		teleinfoGroupe->appendToEtiquette(character);
		return this;
	}
	const char* getName() {
		return "ReadingEtiquetteState";
	}
};

class ReadingDonneeState: public DefaultState {
public:
	ReadingDonneeState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* space() {
		return stateRegistry->getReadingChecksumState();
	}
	StateInterface* other(char character) {
		teleinfoGroupe->appendToDonnee(character);
		return this;
	}
	const char* getName() {
		return "ReadingDonneeState";
	}
};

class ReadingChecksumState: public DefaultState {
public:
	ReadingChecksumState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* space() {
		teleinfoGroupe->setChecksum(TELEINFO_CHAR_SPACE);
		return stateRegistry->getWaitingEndGroupeState();
	}
	StateInterface* other(char character) {
		teleinfoGroupe->setChecksum(character);
		return stateRegistry->getWaitingEndGroupeState();
	}
	const char* getName() {
		return "ReadingChecksumState";
	}
};

class WaitingEndGroupeState: public DefaultState {
public:
	WaitingEndGroupeState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* cr() {
		if (teleinfoGroupe->check()) {
			teleinfoImpl->store(teleinfoGroupe);
			return stateRegistry->getWaitingEndTextOrStartGroupeState();
		} else {
			// checksum error
			return stateRegistry->getWaitingStartTextState();
		}
	}
	const char* getName() {
		return "WaitingEndGroupeState";
	}
};

class WaitingEndTextOrStartGroupeState: public DefaultState {
public:
	WaitingEndTextOrStartGroupeState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	StateInterface* lf() {
		return stateRegistry->getWaitingStartGroupeState()->lf(); // Début d'une nouvelle ligne, on fait suivre à WaitingStartGroupeState
	}
	StateInterface* etx() {
		return stateRegistry->getTerminatedState();
	}
	const char* getName() {
		return "WaitingEndTextOrStartGroupeState";
	}
};

class TerminatedState: public DefaultState {
public:
	TerminatedState(StateRegistry* stateRegistry, TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) : DefaultState(stateRegistry, teleinfoGroupe, teleinfoImpl) {
		// do nothing
	}
	const char* getName() {
		return "TerminatedState";
	}
	Teleinfo* getResult() {
		return teleinfoImpl;
	}
};

// *** Init de StateRegistry ***
StateRegistry::StateRegistry(TeleinfoGroupe* teleinfoGroupe, TeleinfoImpl* teleinfoImpl) {
	waitingStartTextState = new WaitingStartTextState(this, teleinfoGroupe, teleinfoImpl);
	waitingStartGroupeState = new WaitingStartGroupeState(this, teleinfoGroupe, teleinfoImpl);
	readingEtiquetteState = new ReadingEtiquetteState(this, teleinfoGroupe, teleinfoImpl);
	readingDonneeState = new ReadingDonneeState(this, teleinfoGroupe, teleinfoImpl);
	readingChecksumState = new ReadingChecksumState(this, teleinfoGroupe, teleinfoImpl);
	waitingEndGroupeState = new WaitingEndGroupeState(this, teleinfoGroupe, teleinfoImpl);
	waitingEndTextOrStartGroupeState = new WaitingEndTextOrStartGroupeState(this, teleinfoGroupe, teleinfoImpl);
	terminatedState = new TerminatedState(this, teleinfoGroupe, teleinfoImpl);
}
StateInterface* StateRegistry::getWaitingStartTextState() {
	return waitingStartTextState;
}
StateInterface* StateRegistry::getWaitingStartGroupeState() {
	return waitingStartGroupeState;
}
StateInterface* StateRegistry::getReadingEtiquetteState() {
	return readingEtiquetteState;
}
StateInterface* StateRegistry::getReadingDonneeState() {
	return readingDonneeState;
}
StateInterface* StateRegistry::getReadingChecksumState() {
	return readingChecksumState;
}
StateInterface* StateRegistry::getWaitingEndGroupeState() {
	return waitingEndGroupeState;
}
StateInterface* StateRegistry::getWaitingEndTextOrStartGroupeState() {
	return waitingEndTextOrStartGroupeState;
}
StateInterface* StateRegistry::getTerminatedState() {
	return terminatedState;
}

/*********************************************************************************************************************************************************************
  LE DECODEUR TELEINFO (PIMPL IDIOM) @see https://en.wikibooks.org/wiki/C%2B%2B_Programming/Idioms#Pointer_To_Implementation_.28pImpl.29
 *********************************************************************************************************************************************************************/
/**
 * TeleinfoDecoder::TeleinfoDecoderImpl : implémentation
 */
class TeleinfoDecoder::TeleinfoDecoderImpl {
private:
	TeleinfoGroupe* teleinfoGroupe;
	TeleinfoImpl* teleinfoImpl;
	StateRegistry* stateRegistry;
	StateInterface* currentState;

public:
	/**
	 * Init
	 */
	TeleinfoDecoderImpl() {
		teleinfoGroupe = new TeleinfoGroupe();
		teleinfoImpl = new TeleinfoImpl();
		stateRegistry = new StateRegistry(teleinfoGroupe, teleinfoImpl);
		reset();
	}


	/**
	 * Décodage d'un caractère flux Téléinfo
	 */
	Teleinfo* decode(int character) {
		if(character == -1) { // Pré-filtre : on ignore les caractères -1
			return NULL;
		}

		character = character & 0x7F; // Pré-filtre (les caractères sont stockés sur 7 bits + 1 bit de parité)

		StateInterface* nextState;
		switch (character) {
			case TELEINFO_CHAR_STX  :
				nextState = currentState->stx();
				break;

			case TELEINFO_CHAR_ETX :
				nextState = currentState->etx();
				break;

			case TELEINFO_CHAR_EOT  :
				nextState = currentState->eot();
				break;

			case TELEINFO_CHAR_LF :
				nextState = currentState->lf();
				break;

			case TELEINFO_CHAR_CR :
				nextState = currentState->cr();
				break;

			case TELEINFO_CHAR_SPACE :
				nextState = currentState->space();
				break;

			default :
				nextState = currentState->other(character);
				break;
		}

		currentState = nextState;

		Teleinfo* result = currentState->getResult();
		if(result != NULL) {
			reset();
		}
		return result;
	}

	private:
		void reset() {
			currentState = stateRegistry->getWaitingStartTextState();
		}
};

/*
   TeleinfoDecoder : redirection -> TeleinfoDecoder::TeleinfoDecoderImpl
 */
TeleinfoDecoder::TeleinfoDecoder() {
	pimpl_ = new TeleinfoDecoderImpl();
}
Teleinfo* TeleinfoDecoder::decode(int character) {
	return pimpl_->decode(character);
}

