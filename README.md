
# TeleinfoDecoder

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Un décodeur générique du flux Téléinfo.**

## Présentation
Cette bibliothèque est un décodeur prêt-à-l'emploi du flux de *"Sorties de télé-information client des appareils de comptage électroniques utilisés par ERDF"*,
généralement appelé *Téléinfo*.

Le décodeur prend en entrée les octets du flux Téléinfo. Lorsque la trame est complète, il met à disposition un objet de type *Teleinfo* qui contient toutes
les caractéristiques fournies par le compteur électrique.

**Portabilité.** Le décodeur est développé en C++ (pour la structure objet) et C (pour les types et manipulations de données) standards. Il est donc portable, notamment pour les environnements *Arduino* ou *Raspberry Pi* où il trouvera tout son intérêt : ne pas réécrire (et débugger) une énième fois ce décodage.
Le décodeur ne lit ni n'écrit d'entrées/sorties, de pins, de port série, de GPIO, etc. Pour ça, c'est à vous de jouer !

**Robustesse.** Le décodeur est basé sur le [Design Pattern État (State)](https://fr.wikipedia.org/wiki/%C3%89tat_%28patron_de_conception%29), ce qui le rend structurellement très robuste. 
Toute donnée non attendue le ramène à son état initial en attente du début d'une nouvelle trame Téléinfo. 
Il est donc tolérant aux trames erronées, interruptions de trames, trames prises en cours...
De plus, la bibliothèque comporte des tests unitaires [CPPUnit](https://sourceforge.net/projects/cppunit/) qui assurent sa **stabilité** dans le temps et permettent d'éprouver sa robustesse en reproduisant des cas critiques (interruption, erreurs de trames, etc.).   

**Empreinte mémoire.** Pour une intégration en système embarqué, le décodeur gère sa mémoire *en bon père de famille* : une fois le décodeur initialisé (```new TeleinfoDecoder()```), son empreinte mémoire reste constante, aucune allocaton/désallocation dynamique n'est effectuée donc aucun risque de fragmentation de la mémoire.
*Attention.* l'objet de type *Teleinfo* retourné par le décodeur (voir plus bas) ne doit pas être désalloué (```free(...)```), il est réutilisé pour les décodages de trames suivantes.  

## Usage
### Initialisation
Le décodeur est initialisé par la création d'une instance de *TeleinfoDecoder* :

```C
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder();
```

### Décodage
Le décodage du flux Téléinfo se fait en injectant un à un les octets lus du flux série :

```C
Teleinfo* teleinfo = teleinfoDecoder->decode(character);
```

La méthode ```decode(int character)``` a 2 résultats possibles :

* **NULL** : aucune donnée de Téléinfo n'est disponible pour le moment; la trame en cours n'est pas terminée
* **un objet de signature *Teleinfo***: la trame Téléinfo est terminée, son contenu est disponible dans l'objet *Teleinfo* mis à disposition (voir ci-dessous)

### Consultation
Les informations de Téléinfo sont consultables sur l'objet de signature *Teleinfo* renvoyé par `decode(int character)`.

Les méthodes simplifiées de consultation sont :

Méthode | Unité | Type | Description
------- | ----- | ---- | -----------
`teleinfo->getAdco()` | | `char*` | Donne le numéro de série du compteur
`teleinfo->getTotalIndex()` | Wh | `unsigned long` | Donne la consommation totale du compteur quelque soit l'option tarifaire (base, heures pleines/creuses, EJP, etc.). Il s'agit de la somme des valeurs *BASE*, *HCHC*, *HCHP*, *EJPHN*, *EJPHPM*, *BBRHCJB*, *BBRHPJB*, *BBRHCJW*, *BBRHPJW*, *BBRHCJR* et *BBRHPJR*, à laquelle est soustrait [un *offset* optionnel](#offset-de-lindex-total). A diviser par 1000 pour obtenir des kWh plus usuels.
`teleinfo->getInstPower()` | W | `int` | Donne la puissance instantanée. Celle-ci correspond à la valeur de *PAPP* (puissance apparente). Cette dernière n'étant pas toujours présente, la puissance est alors calculée avec 230 (V) * IINST (intensité instantanée). Si IINST n'est pas disponible non plus, retourne 0.

Pour la consultation des autres informations de Téléinfo, voir [Usage avancé] (#usage-avancé).

### Constantes

La bibliothèque met à disposition une constante utilitaire `TELEINFO_BAUD_RATE` qui correspond au débit du flux Téléinfo (1200 baud).
Celle-ci n'est pas utilisée dans le décodeur mais peut être utilisée pour paramétrer un port série de lecture du flux.

### Divers
Afin d'alléger le code d'intégration le TeleinfoDecoder applique 2 filtres : 
* il ignore les caractères de valeur -1 (qui peut être renvoyé par une lecture de port série sans octet disponible, par exemple)
* les caractères du flux Téléinfo sont codés sur 7 bits + 1 bit de parité, ce dernier est ignoré par un ET logique 7Fh   

Bref, du code en moins à écrire avant d'appeler `decode(int character)`.

# Exemples 
## En environnement Arduino
### Intégration
Pour utiliser la bibliothèque TeleinfoDecoder dans votre projet Arduino, il vous faut :

1. Créer un projet dans l'IDE Arduino et le sauvegarder
2. Copier les fichiers *src/TeleinfoDecoder.h* et *src/TeleinfoDecoder.cpp* dans le répertoire du projet 
3. Ré-ouvrir le projet dans l'IDE Arduino, les fichiers *TeleinfoDecoder.h* et *TeleinfoDecoder.cpp* sont ouverts dans de nouveaux onglets, le décodeur peut être utilisé depuis le fichier *.ino* 

### Code source

Le code source suivant :

* lit le flux Téléinfo sur le pin 10 de l'Arduino
* décode le flux à l'aide de cette bibliothèque
* envoie vers le Moniteur série de l'IDE Arduino (en 115200 baud) quelques informations lues (n° du compteur, index total, puissance instantanée) 

Voir les commentaires dans le code pour tout comprendre :  

```C
#include <SoftwareSerial.h>
#include "TeleinfoDecoder.h"

SoftwareSerial* teleinfoSerial;
TeleinfoDecoder* teleinfoDecoder;

void setup() {
  pinMode(10, INPUT);
  teleinfoSerial = new SoftwareSerial(10, 255, true); // Entrée du signal Téléinfo sur le pin 10 de l'Arduino 
  teleinfoDecoder = new TeleinfoDecoder();
  Serial.begin(115200); // Sortie sur le Moniteur série de l'IDE Arduino en 115200 baud
}

void loop() {
  // Décodage du flux
  Serial.println("Lecture du flux Téléinfo"); 
  teleinfoSerial->begin(TELEINFO_BAUD_RATE); // Init de la lecture du flux Téléinfo. Utilisation de la constante TELEINFO_BAUD_RATE contient le débit du flux Téléinfo (1200 baud)
  
  Teleinfo* teleinfo = NULL;  // C'est dans cette variable que nous récupérerons les informations du compteur transmises par la Téléinfo
  while(teleinfo == NULL) {
    int character = teleinfoSerial->read();
    teleinfo = teleinfoDecoder->decode(character);
  }
  
  teleinfoSerial->end();
  // Fin de décodage du flux
  
  // Traitement des informations récupérées dans l'objet Teleinfo 
  // Ici : transmission au Moniteur série de l'IDE Arduino, mais vous pouvez mettre tout ce que vous voulez en faire (par exemple, ESP8266, RfxCom, afficheur, etc.)
  Serial.print("Compteur No : "); Serial.println(teleinfo->getAdco());
  Serial.print("Consommation totale (Wh) : "); Serial.println(teleinfo->getTotalIndex());
  Serial.print("Puissance instantanée (W) : "); Serial.println(teleinfo->getInstPower());
  
  // Pause de 30 secondes avant de recommencer
  Serial.println("Pause de 30 secondes..."); 
  delay(30000);
}
```

## Usage avancé
### Consultation des informations Téléinfo
Méthode | Etiquette Téléinfo | Description | Unité | Type
------- | ------------------ | ----------- | ----- | ----
`teleinfo->getAdco()` | ADCO | Donne l'*Adresse du compteur* | | `char*`
`teleinfo->getOptarif()` | OPTARIF | Donne l'*Option tarifaire choisie* | | `char*`
`teleinfo->getIsousc()` | ISOUSC | Donne l'*Intensité souscrite* | A | `int`
`teleinfo->getBase()` | BASE | Option BASE : donne l'*Index option base* | Wh | `unsigned long`
`teleinfo->getHchc()` | HCHC | Heures Creuses : donne l'*Index option heures creuses* | Wh | `unsigned long`
`teleinfo->getHchc()` | HCHP | Heures Creuses : donne l'*Index option heures pleines* | Wh | `unsigned long`
`teleinfo->getEjphn()` | EJPHN | Option EJP : donne l'*Index heures normales* | Wh | `unsigned long`
`teleinfo->getEjphpm()` | EJPHPM | Option EJP : donne l'*Index heures de pointe mobile* | Wh | `unsigned long`
`teleinfo->getBbrhcjb()` | BBRHCJB | Option TEMPO : donne l'*Index heures creuses jours bleus* | Wh | `unsigned long`
`teleinfo->getBbrhpjb()` | BBRHPJB | Option TEMPO : donne l'*Index heures pleines jours bleus* | Wh | `unsigned long`
`teleinfo->getBbrhcjw()` | BBRHCJW | Option TEMPO : donne l'*Index heures creuses jours blancs* | Wh | `unsigned long`
`teleinfo->getBbrhpjw()` | BBRHPJW | Option TEMPO : donne l'*Index heures pleines jours blancs* | Wh | `unsigned long`
`teleinfo->getBbrhcjr()` | BBRHCJR | Option TEMPO : donne l'*Index heures creuses jours rouges* | Wh | `unsigned long`
`teleinfo->getBbrhpjr()` | BBRHPJR | Option TEMPO : donne l'*Index heures pleines jours rouges* | Wh | `unsigned long`
`teleinfo->getPejp()` | PEJP | Donne le *Préavis heures EJP* | min | `int`
`teleinfo->getPtec()` | PTEC | Donne la *Période tarifaire en cours* |  | `char*`
`teleinfo->getDemain()` | DEMAIN | Donne la *Couleur du lendemain* |  | `char*`
`teleinfo->getIinst()` | IINST | Donne l'*Intensité instantanée* | A | `int`
`teleinfo->getAdps()` | ADPS | Donne l'*Avertissement de dépassement de puissance souscrite* | A | `int`
`teleinfo->getImax()` | IMAX | Donne l'*Intensité maximale appelée* | A | `int`
`teleinfo->getPapp()` | PAPP | Donne la *Puissance apparente* | W | `int`
`teleinfo->getHhphc()` | HHPHC | donne l'*Horaire heure creuse heure pleine* |  | `char`
`teleinfo->getMotdetat()` | MOTDETAT | donne le *Mot d'état du compteur* |  | `char*`

### Facilités de consultation de l'adresse du compteur
Méthode | Description
------- | -----------
`teleinfo->getAdco()` | Donne l'*Adresse du compteur* telle que transmise par le protocole Teleinfo, donc complété par des zéros à gauche 
`teleinfo->getAdcoChecksum8()` | Calcule un checksum modulo 256 de l*'Adresse du compteur*. Peut constituer une adresse sur 8 bits du compteur. Par exemple, cette valeur peut servir d'identifiant de sonde dans un protocole de transmission radio de la consommation électrique.       
`teleinfo->getAdcoAsLong()` | Donne l'*Adresse du compteur* sous la forme d'un entier long positif. Elimine les zéros non signifcatifs.    

### Offset de l'index total
Le décodeur permet d'appliquer un *offset* à l'index total. L'*offset* est pris en compte dans `teleinfo->getTotalIndex()` mais pas dans les méthodes de consultation des groupes Téléinfo comme `teleinfo->getBase()`, `teleinfo->getHchc()`, etc..
L'*offset* est de type `unsigned long`, il est défini à la création du décodeur. Exemple avec un *offset* de 10000Wh :
```C
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder(10000);
```
Suite à cette initialisation, pour un compteur avec un index à 550000 Wh, `teleinfo->getTotalIndex()` retournera une valeur de 540000Wh (550000Wh - 10000Wh d'*offset*).

L'*offset* appliqué peut être consulté par `teleinfo->getTotalOffset()`.


#### Offset par défaut
Par défaut, le décodeur n'applique pas d'*offset*, les 3 initialisations suivantes sont équivalentes :
```
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder();
```
ou
```
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder(TELEINFO_TOTAL_OFFSET_NONE);
```
ou
```
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder(0);
```

#### Offset automatique
Un cas particulier d'offset, est l'*offset automatique* (constante `TELEINFO_TOTAL_OFFSET_AUTO`), qui peut être défini à la création du décodeur par :
```C
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder(TELEINFO_TOTAL_OFFSET_AUTO);
```

L'*offset* prend alors la valeur de l'index obtenu dans la première trame Téléinfo. L'*index total* commence donc à zéro à la création du décodeur.

Exemple :
1. Création du décodeur
```C
TeleinfoDecoder* teleinfoDecoder = new TeleinfoDecoder(TELEINFO_TOTAL_OFFSET_AUTO);
```
2. Décodage `teleinfo = decode(character)` jusque fin de la première trame. Le compteur a un index de 550000Wh.
    - `teleinfo->getTotalIndex()` retourne 0Wh
    - `teleinfo->getTotalOffset()` retourne 550000Wh
3. Décodage `teleinfo = decode(character)` jusque fin d'une trame suivante. Le compteur a un index de 553000Wh.
    - `teleinfo->getTotalIndex()` retourne 3000Wh
    - `teleinfo->getTotalOffset()` retourne 550000Wh


`getTotalIndex()` commence donc à 0Wh
`getTotalOffset()` prend la valeur de l'index après la première trame reçue

#### Exemple d'utilisation de l'offset
L'*offset* permet de créer une sonde Téléinfo qui commence son comptage à 0Wh, en stockant ensuite l'*offset* dans une mémoire du type EEPROM :
1. Première utilisation :
	- pas d'*offset* stocké en EEPROM : création du *TeleinfoDecoder* avec un *offset* automatique `TELEINFO_TOTAL_OFFSET_AUTO`
	- après réception de la première trame, consultation de l'*offset* `teleinfo->getTotalOffset()` et stockage en EEPROM
	- le comptage avec `teleinfo->getTotalIndex()` commence à 0Wh
2. Utilisations suivantes de la sonde (après avoir été débranchée par exemple) :
	- *offset* trouvé en EEPROM : création du *TeleinfoDecoder* avec un *offset* automatique `TELEINFO_TOTAL_OFFSET_AUTO`
	- le comptage avec `teleinfo->getTotalIndex()` continue où il en était

L'*offset* peut donc être utile dans le cas où le protocole de transmission de la sonde ne supporte pas de très grandes valeurs d'index. Cce qui est le cas du protcole *RfxPower/RfxMeter*, par exemple.

## Code
### Arborescence

```
.
├── bin         les exécutables générés par la compilaton
├── build       les fichiers intermédiaires lors de la compilation       
├── lib         les bibliothèques du décodeur générées par la compilaton
├── src         le code source du décodeur (fichiers .h et .cpp)
├── test        le code source des tests unitaires du décodeur 
├── LICENSE
├── Makefile           
└── README.md                          
```

### Construction
#### Compilation
Pour compiler le décodeur :

```
make all
```

La bibliothèque du décodeur *libteleinfodecoder.a* est disponible dans le répertoire *lib*.
#### Tests unitaires
Pour lancer les tests unitaires :

```
make test-all
```

Cette commande compile le décodeur et génère un executable runtests(.exe) qui est lui-même lancé.
 
