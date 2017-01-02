/**
 * Test unitaire du décodeur Téléinfo
 * @author LK
 */

#include "TeleinfoDecoder.h"
#include <stdlib.h>
#include <string.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

class TeleinfoDecoderTest : public CppUnit::TestFixture {

public:
	/**
	 * Test avec tous les groupes valorisés (même si c'est un cas irréel)
	 */
	void testTrameComplete() {
		TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder;
		CPPUNIT_ASSERT(injectStartText(teleinfoDecoder) == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "ADCO", "026489026467") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "OPTARIF", "BASE") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "ISOUSC", "30") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BASE", "006789543") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "HCHC", "000654398") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "HCHP", "009755123") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "EJPHN", "000003365") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "EJPHPM", "003556600") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BBRHCJB", "002836660") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BBRHPJB", "001117777") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BBRHCJW", "900222022") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BBRHPJW", "568800001") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BBRHCJR", "009222010") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "BBRHPJR", "000001112") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "PEJP", "60") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "PTEC", "HCJB") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "DEMAIN", "BLAN") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "ADPS", "020") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "IINST", "004") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "IMAX", "030") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "PAPP", "00970") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "HHPHC", "D") == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "MOTDETAT", "000000") == NULL);

		Teleinfo* teleinfo = injectEndText(teleinfoDecoder);
		CPPUNIT_ASSERT(teleinfo != NULL);
		CPPUNIT_ASSERT(strcmp(teleinfo->getAdco(), "026489026467") == 0);
		CPPUNIT_ASSERT(strcmp(teleinfo->getOptarif(), "BASE") == 0);
		CPPUNIT_ASSERT(teleinfo->getIsousc() == 30);
		CPPUNIT_ASSERT(teleinfo->getBase() == 6789543);
		CPPUNIT_ASSERT(teleinfo->getHchc() == 654398);
		CPPUNIT_ASSERT(teleinfo->getHchp() == 9755123);
		CPPUNIT_ASSERT(teleinfo->getEjphn() == 3365);
		CPPUNIT_ASSERT(teleinfo->getEjphpm() == 3556600);
		CPPUNIT_ASSERT(teleinfo->getBbrhcjb() == 2836660);
		CPPUNIT_ASSERT(teleinfo->getBbrhpjb() == 1117777);
		CPPUNIT_ASSERT(teleinfo->getBbrhcjw() == 900222022);
		CPPUNIT_ASSERT(teleinfo->getBbrhpjw() == 568800001);
		CPPUNIT_ASSERT(teleinfo->getBbrhcjr() == 9222010);
		CPPUNIT_ASSERT(teleinfo->getBbrhpjr() == 1112);
		CPPUNIT_ASSERT(teleinfo->getPejp() == 60);
		CPPUNIT_ASSERT(strcmp(teleinfo->getPtec(), "HCJB") == 0);
		CPPUNIT_ASSERT(strcmp(teleinfo->getDemain(), "BLAN") == 0);
		CPPUNIT_ASSERT(teleinfo->getAdps() == 20);
		CPPUNIT_ASSERT(teleinfo->getIinst() == 4);
		CPPUNIT_ASSERT(teleinfo->getImax() == 30);
		CPPUNIT_ASSERT(teleinfo->getPapp() == 970);
		CPPUNIT_ASSERT(teleinfo->getHhphc() == 'D');
		CPPUNIT_ASSERT(strcmp(teleinfo->getMotdetat(), "000000") == 0);
	}

	/**
	 * Test avec une toute patite trame (adresse du compteur seul, même si c'est irréel)
	 */
	void testTrameMinimaliste() {
		TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder;
		CPPUNIT_ASSERT(injectStartText(teleinfoDecoder) == NULL);
		CPPUNIT_ASSERT(injectGroupe(teleinfoDecoder, "ADCO", "026489026467") == NULL);
		Teleinfo* teleinfo = injectEndText(teleinfoDecoder);
		CPPUNIT_ASSERT(teleinfo != NULL);
		CPPUNIT_ASSERT(strcmp(teleinfo->getAdco(), "026489026467") == 0);
	}

	/**
	 * Test du calcul particulier de getInstPower()
	 */
	void testGetInstPower() {
		TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder;

		// Pas de puissance tranmise
		injectStartText(teleinfoDecoder);
		injectGroupe(teleinfoDecoder, "ADCO", "026489026467");
		Teleinfo* teleinfo = injectEndText(teleinfoDecoder);
		CPPUNIT_ASSERT(teleinfo->getInstPower() == 0);

		// Intensité transmise dans IINST mais pas de PAPP
		injectStartText(teleinfoDecoder);
		injectGroupe(teleinfoDecoder, "ADCO", "026489026467");
		injectGroupe(teleinfoDecoder, "IINST", "17");
		teleinfo = injectEndText(teleinfoDecoder);
		CPPUNIT_ASSERT(teleinfo != NULL);
		CPPUNIT_ASSERT(teleinfo->getInstPower() == 3910);

		// Intensité transmise dans IINST et puissance apparent dans PAPP
		injectStartText(teleinfoDecoder);
		injectGroupe(teleinfoDecoder, "ADCO", "026489026467");
		injectGroupe(teleinfoDecoder, "IINST", "17");
		injectGroupe(teleinfoDecoder, "PAPP", "4000");
		teleinfo = injectEndText(teleinfoDecoder);
		CPPUNIT_ASSERT(teleinfo->getInstPower() == 4000);

	}

private:
	/**
	 * Injecte un caractère "Start TeXt" STX (002 h) qui indique le début de la trame
	 */
	Teleinfo* injectStartText(TeleinfoDecoder* teleinfoDecoder) {
		return injectCharacter(teleinfoDecoder, 0x02);
	}

	/**
	 * Injecte un caractère "End TeXt" ETX (003 h) indique la fin de la trame
	 */
	Teleinfo* injectEndText(TeleinfoDecoder* teleinfoDecoder) {
		return injectCharacter(teleinfoDecoder, 0x03);
	}

	/**
	 * Injecte un caractère ASCII "End Of Transmission" EOT (004 h) est généré avant interruption,
	 */
	Teleinfo* injectEndOfTransmission(TeleinfoDecoder* teleinfoDecoder) {
		return injectCharacter(teleinfoDecoder, 0x04);
	}

	/**
	 * Injecte un caractère "Line Feed" LF (00A h) indiquant le début du groupe
	 */
	Teleinfo* injectLineFeed(TeleinfoDecoder* teleinfoDecoder) {
		return injectCharacter(teleinfoDecoder, 0x0A);
	}

	/**
	 * Injecte un caractère "SPace" SP (020 h) séparateur du champ étiquette et du champ donnée
	 */
	Teleinfo* injectSpace(TeleinfoDecoder* teleinfoDecoder) {
		return injectCharacter(teleinfoDecoder, 0x20);
	}

	/**
	 * Injecte un caractère "Carriage Return" CR (00D h) indiquant la fin du groupe d'information.
	 */
	Teleinfo* injectCarriageReturn(TeleinfoDecoder* teleinfoDecoder) {
		return injectCharacter(teleinfoDecoder, 0x0D);
	}

	/**
	 * Injecte un groupe étiquette/donnée
	 */
	Teleinfo* injectGroupe(TeleinfoDecoder* teleinfoDecoder, string etiquette, string donnee) {
		injectLineFeed(teleinfoDecoder);
		injectText(teleinfoDecoder, etiquette);
		injectSpace(teleinfoDecoder);
		injectText(teleinfoDecoder, donnee);
		injectSpace(teleinfoDecoder);
		int checksum = computeChecksum(etiquette, donnee);
		injectCharacter(teleinfoDecoder, checksum);
		return injectCarriageReturn(teleinfoDecoder);
	}

	/**
	 * Injecte une chaîne de caractères quelconques
	 */
	Teleinfo* injectText(TeleinfoDecoder* teleinfoDecoder, string text) {
		Teleinfo* teleinfo = NULL;
		for(unsigned int i = 0; i < text.length(); i++) {
		    char character = text[i];
		    teleinfo = teleinfoDecoder->decode(character);
		}
		return teleinfo;
	}

	/**
	 * Injecte un caractère quelconque
	 */
	Teleinfo* injectCharacter(TeleinfoDecoder* teleinfoDecoder, int character) {
		return teleinfoDecoder->decode(character);
	}

	/**
	 * Calcul le chacksum d'un groupe étiquette/donnée
	 *
	 * La "checksum" est calculée sur l'ensemble des caractères allant du début du champ étiquette
	 * à la fin du champ donnée, caractère SP inclus. On fait tout d'abord la somme des codes ASCII
	 * de tous ces caractères. Pour éviter d'introduire des fonctions ASCII (00 à 1F en hexadécimal),
	 * on ne conserve que les six bits de poids faible du résultat obtenu (cette opération se traduit
	 * par un ET logique entre la somme précédemment calculée et 03Fh).
	 * Enfin, on ajoute 20 en hexadécimal.
	 */
	int computeChecksum(string etiquette, string donnee) {
		int checksum = 0;
	string text = etiquette + " " + donnee;
		for(unsigned int i = 0; i < text.length(); i++) {
			checksum += text[i];
		}
		checksum = (checksum & 0x3F) + 0x20;
		return checksum;
	}

	CPPUNIT_TEST_SUITE(TeleinfoDecoderTest);
	CPPUNIT_TEST(testTrameComplete);
	CPPUNIT_TEST(testTrameMinimaliste);
	CPPUNIT_TEST(testGetInstPower);
	CPPUNIT_TEST_SUITE_END();

};
CPPUNIT_TEST_SUITE_REGISTRATION(TeleinfoDecoderTest);
