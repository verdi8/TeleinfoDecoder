[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# Présentation
Cette bibliothèque est un décodeur prêt-à-l'emploi du flux de *"Sorties de télé-information client des appareils de comptage électroniques utilisés par ERDF"*,
généralement appelé *Téléinfo*.

Le décodeur prend en entrée les octets du flux Téléinfo. Lorsque la trame est complète, il met à disposition un objet de type *Teleinfo* qui contient toutes
les caractéristiques fournies par le compteur électrique.   

# Portabilité
Le décodeur est développé en C++/standard (structure) avec les bibliothèques standards du C (types). Il est donc portable, notamment pour les environnements Arduino ou Raspberry Pi.
Le décodeur ne lit ni n'écrit d'entrées/sorties, de pins, de port série, de GPIO, etc. Pour ça, c'est à vous de jouer !  

# Robustesse
Le décodeur est basé sur le [Design Pattern État (State)](https://fr.wikipedia.org/wiki/%C3%89tat_%28patron_de_conception%29), ce qui le rend structurellement très robuste. 
Toute donnée non attendue le ramène à son état initial en attente du début d'une nouvelle trame Téléinfo. 
Il est donc tolérant aux trames erronées, interruptions de trames, trames prises en cours... 

# Empreinte mémoire
Le décodeur gère sa mémoire "en bon père de famille". Une fois le décodeur initialisé (new TeleinfoDecoder()), son empreinte mémoire reste constante, aucune allocaton/désallocation n'est executée par la suite.

Attention, l'objet de type Teleinfo retourné par le décodeur ne doit pas être désalloué (free), il est réutilisé pour les décodages de trames suivantes.  

# Usage
## Initailisation du décodeur

## Décodage des informations de Téléinfo

## Consultation des informations de Téléinfo

### Principales méthodes 
Méthode | Description
-|-
compteur->getAdco()|

## Constantes

## Autres
Afin d'alleger le code d'intégration, le TeleinfoDecoder applique 2 filtres : 
* il ignore les caractères de valeur -1  
* les caractères du flux Téléinfo sont codés sur 7 bits + 1 bit de parité, ce dernier est ignoré par un ET logique 7Fh   

# Exemple d'usage de la bibliothèque TeleinfoDecoder en environnement Arduino

## Intégration
Pour utiliser la bibliothèque TeleinfoDecoder dans votre projet Arduino, il vous faut :
1. Créer un projet dans l'IDE Arduino et le sauvegarder
2. Copier les fichiers TeleinfoDecoder.h et TeleinfoDecoder.cpp dans le répertoire du projet 
3. Ré-ouvrir le projet dans l'IDE Arduino, les fichiers TeleinfoDecoder.h et TeleinfoDecoder.cpp sont ouverts dans de nouveaux onglets, le décodeur peut être utilisé depuis le fichier .ino  

## Exemple de code

 



# Usage avancé

Méthode|Etiquette Téléinfo| Description| Unité|Type
-|-|-|-|-
teleinfo->getAdco()|ADCO|Donne l'*Adresse du compteur*||char*
teleinfo->getOptarif()|OPTARIF|Donne l'*Option tarifaire choisie*||char*
teleinfo->getIsousc()|ISOUSC|Donne l'*Intensité souscrite*|A|int
teleinfo->getBase()|BASE|Option BASE : donne l'*Index option base*|Wh|unsigned long
teleinfo->getHchc()|HCHC|Heures Creuses : donne l'*Index option heures creuses*|Wh|unsigned long
teleinfo->getHchc()|HCHP|Heures Creuses : donne l'*Index option heures pleines*|Wh|unsigned long
teleinfo->getEjphn()|EJPHN|Option EJP : donne l'*Index heures normales*|Wh|unsigned long
teleinfo->getEjphpm()|EJPHPM|Option EJP : donne l'*Index heures de pointe mobile*|Wh|unsigned long
teleinfo->getBbrhcjb()|BBRHCJB|Option TEMPO : donne l'*Index heures creuses jours bleus*|Wh|unsigned long
teleinfo->getBbrhpjb()|BBRHPJB|Option TEMPO : donne l'*Index heures pleines jours bleus*|Wh|unsigned long
teleinfo->getBbrhcjw()|BBRHCJW|Option TEMPO : donne l'*Index heures creuses jours blancs*|Wh|unsigned long
teleinfo->getBbrhpjw()|BBRHPJW|Option TEMPO : donne l'*Index heures pleines jours blancs*|Wh|unsigned long
teleinfo->getBbrhcjr()|BBRHCJR|Option TEMPO : donne l'*Index heures creuses jours rouges*|Wh|unsigned long
teleinfo->getBbrhpjr()|BBRHPJR|Option TEMPO : donne l'*Index heures pleines jours rouges*|Wh|unsigned long
teleinfo->getPejp()|PEJP|Donne le *Préavis heures EJP*|min|int
teleinfo->getPtec()|PTEC|Donne la *Période tarifaire en cours*||char*
teleinfo->getDemain()|DEMAIN|Donne la *Couleur du lendemain*||char*
teleinfo->getIinst()|IINST|Donne l'*Intensité instantanée*|A|int
teleinfo->getAdps()|ADPS|Donne l'*Avertissement de dépassement de puissance souscrite*|A|int
teleinfo->getImax()|IMAX|Donne l'*Intensité maximale appelée*|A|int
teleinfo->getPapp()|PAPP|Donne la *Puissance apparente*|W|int
teleinfo->getHhphc()|HHPHC|donne l'*Horaire heure creuse heure pleine*||char
teleinfo->getMotdetat()|MOTDETAT|donne le *Mot d'état du compteur*||char*
