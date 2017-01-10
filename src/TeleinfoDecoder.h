/**
 * Déclaration du décodeur Téléinfo
 * @author LK
 */

#ifndef TELEINFO_DECODER_H_
#define TELEINFO_DECODER_H_

/**
 * Constante donnant le débit de liaison série du flux Téléinfo
 */
#define TELEINFO_BAUD_RATE   1200

/**
 * Cette interface donne accès aux données du compteur qui ont été lues par le protocole Téléinfo
 */
class Teleinfo {
  public:
    /**
     * Donne l'Adresse du compteur
     */
    virtual char* getAdco()=0;    
    
    /**
     * Donne l'Option tarifaire choisie
     */
    virtual char* getOptarif()=0;

    /**
     * Donne l'Intensité souscrite (A)
     */
    virtual int getIsousc()=0;

    /**
     * Option BASE : donne l'Index option base (Wh)
     */
    virtual unsigned long getBase()=0;

    /**
     * Option Heures Creuses : donne l'Index option heure creuse (Wh)
     */
    virtual unsigned long getHchc()=0;

    /**
     * Option Heures Creuses : donne l'Index option heure pleine (Wh)
     */
    virtual unsigned long getHchp()=0;
    
    /** 
     * Option EJP : donne l'Index heures normales (Wh)
     */
    virtual unsigned long getEjphn()=0;

    /**
     * Option EJP : donne l'Index heures de pointe mobile (Wh)
     */
    virtual unsigned long getEjphpm()=0; // 

    /**
     * Option TEMPO : donne l'Index heures creuses jours bleus (Wh)
     */
    virtual unsigned long getBbrhcjb()=0; 

    /**
     * Option TEMPO : donne l' Index heures pleines jours bleus (Wh)
     */
    virtual unsigned long getBbrhpjb()=0;

    /**
     * Option TEMPO : donne l'Index heures creuses jours blancs (Wh)
     */
    virtual unsigned long getBbrhcjw()=0;  

    /**
     * Option TEMPO : donne l'Index heures pleines jours blancs (Wh)
     */
    virtual unsigned long getBbrhpjw()=0; 

    /**
     * Option TEMPO : donne l'Index heures creuses jours rouges (Wh)
     */
    virtual unsigned long getBbrhcjr()=0; 

    /**
     * Option TEMPO : donne l'Index heures pleines jours rouges (Wh)
     */
    virtual unsigned long getBbrhpjr()=0; 

    /**
     * Autres : donne le Préavis heures EJP (min)
     */
    virtual int getPejp()=0;

    /**
     * Autres : donne la Période tarifaire en cours
     */
    virtual char* getPtec()=0;

    /**
     * Autres : donne la Couleur du lendemain
     */
    virtual char* getDemain()=0;

    /**
     * Autres : donne l'Intensité instantanée (A)
     */
    virtual int getIinst()=0; 

    /**
     * Autres : donne l'Avertissement de dépassement de puissance souscrite (A)
     */
    virtual int getAdps()=0;  

    /**
     * Autres : donne l'Intensité maximale appelée (A)
     */
    virtual int getImax()=0; 

    /**
     * Autres : donne la Puissance apparente (VA)
     */
    virtual int getPapp()=0;

    /**
     * Autres : donne l'Horaire heure creuse heure pleine
     */
    virtual char getHhphc()=0;

    /**
     * Autres : donne le Mot d'état du compteur
     */
    virtual char* getMotdetat()=0; 

    // Fonctions spéciales -----------------------------------------------------------------------------------------------------------------

    /**
     * Donne l'index total (la somme de tous les index quelque soit l'option)
     */
    virtual unsigned long getTotalIndex()=0;

    /**
     * Donne la puissance instantanée (W)
     * @return la valeur de PAPP (Puissance apparente) si disponible, sinon la valeur de IINST x 230V si disponible, 0 sinon
     */
    virtual int getInstPower()=0;

    /**
     * Donne l'Adresse du compteur sous forme d'un entier positif long
     * @return l'adresse du compteur, 0 si elle ne peut être parsée (voir la fonction standard strtoul(...))
     */
    virtual unsigned long getAdcoAsLong()=0;

    /**
     * Donne un checksum sur 8 bits de l'adresse du compteur
     */
    virtual unsigned int getAdcoChecksum8()=0;

};

/**
 * Cette classe est un décodeur Téléinfo. Elle lit le flux sur un pin d'entrée donné pour construire un objet de type CompteurInterface. 
 * Le CompteurInterface donne accès aux données du compteur.
 * 
 * Implémentation par le pimpl idiom : @see https://en.wikibooks.org/wiki/C%2B%2B_Programming/Idioms#Pointer_To_Implementation_.28pImpl.29
 */
class TeleinfoDecoder {
  private:
    class TeleinfoDecoderImpl;
    TeleinfoDecoderImpl* pimpl_;

  public:
    /**
     * Création du décodeur Téléinfo.
     * 
     * @param inputPin le numéro de pin d'entrée du flux Téléinfo
     */
    TeleinfoDecoder();

    /**
     * Décode un caractère du flux Téléinfo
     * 
     * @return un objet Teleinfo qui donne accès aux données du compteur, ou NULL si aucun flux Téléinfo n'a pu Ãªtre lu
     */
    Teleinfo* decode(int character);


};

#endif  // TELEINFO_DECODER_H_



