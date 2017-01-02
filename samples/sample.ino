
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
  teleinfoSerial->begin(TELEINFO_BAUD_RATE); // Init de la lecture du flux Téléinfo. La constante TELEINFO_BAUD_RATE contient le débit du flux Téléinfo (1200 baud)
  
  Teleinfo* teleinfo = NULL;  // C'est dans cette variable que nous récupérerons les informations du compteur transmises par la Téléinfo
  while(teleinfo == NULL) {
    int character = teleinfoSerial->read();
    teleinfo = teleinfoDecoder->decode(character);
  }
  
  teleinfoSerial->end();
  // Fin de décodage du flux
  
  // Traitement des informations récupérées dans l'objet Teleinfo (par exemple, ESP8266, RfxCom, afficheur, etc.)
  // Ici : transmission au Moniteur série de l'IDE Arduino
  Serial.print("Compteur No : "); Serial.println(teleinfo->getAdco());
  Serial.print("Consommation totale (Wh) : "); Serial.println(teleinfo->getTotalIndex());
  Serial.print("Puissance instantanée (W) : "); Serial.println(teleinfo->getInstPower());
  
  // Pause de 30 secondes avant de recommencer
  Serial.println("Pause de 30 secondes..."); 
  delay(30000);
}
