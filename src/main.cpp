// Inclusion des bibliothèques
#include<Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MKRWAN.h>

// Définition de la pression atmosphérique
#define SEALEVELPRESSURE_HPA (1013.25)

// Declaration de la Fonction 
void CapterDonnees(); 

// Création d'objets
Adafruit_BME280 bme; // objet pour le capteur BME280
LoRaModem modem; // objet pour le module LoRaWAN

String appEui = "0000000000000000";  // Déclaration de l'identifiant de l'application LoRaWAN 
String appKey = "127EE95DB178826CF22AD2C9E41C5D84";  // Déclaration de la clé d'application LoRaWAN
int Connected , Response;  // Déclaration des variables pour la connexion LoRaWAN

float Temperature, Humidity, Pression; // Déclaration des variables pour les données de capteur BME280
int AmbientSensorValeur = 0; // Déclaration de la variable pour les données de capteur de lumière
unsigned status; // Déclaration de la variable de statut de communication I2C
unsigned long TempsAvant, TempsActuel; // Déclaration des variables pour la gestion du temps
const int DelayTime = 2000; // Déclaration du temps de retard entre chaque capture de données
const int AmbientPin = A1; // Déclaration de la broche du capteur de lumière

void setup() {

    Serial.begin(9600);
    Serial.println(F("BME280 test"));

    // default settings
    status = bme.begin(0x76); // Debut de communication I2c avec le bme sur l'adresse 0x76

    // Si le bme ne fonctionne pas 
    if (!status) {
          Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");  
    }
    Serial.println();

    // Initialisation du module LoRaWAN avec la région US915 - Canada 
    if (!modem.begin(US915)) {
    Serial.println("Failed to start module");
    while (1) {}
    };

    Connected = modem.joinOTAA(appEui, appKey); // Connexion au réseau LoRaWAN avec OTAA

    //S'il se connecter pas, Attendre jusqu'a que la connexion s'etablie 
    while (!Connected) {
    Connected = modem.joinOTAA(appEui, appKey);
    }
    Serial.println("Connected!.."); 

    delay(5000);
    
    // Configuration du port pour les données LoRaWAN et debut des packets 
    modem.setPort(3);
    modem.beginPacket();
}


void loop() { 
   
   // Apppel la fonction chaque une seconde 
   if (millis() - TempsAvant > DelayTime)
   {
      CapterDonnees(); // Appel la fonction printValue() pour capteur les donnees  
      TempsAvant = millis();
   }

    // Envoi des données mesurées via le module LoRaWAN
    modem.print("Temperature :");
    modem.print(String(Temperature));
    modem.print(", LUX :");
    modem.print(String(AmbientSensorValeur));

    Response = modem.endPacket(true); // Fin de l'envoi du packet et reception de la reponse  

    if (Response > 0) {
    Serial.println("Message sent correctly!");
    } else {
    Serial.println("Error sending message :(");
    }

    modem.poll();
    delay(10000);
}

void CapterDonnees() {

    Temperature = bme.readTemperature();  // Lecture de la température depuis le capteur BME280
    AmbientSensorValeur = analogRead(AmbientPin); // Lecture de la valeur du capteur de lum
   
    // Affichage des donnees capturer sur le moniteur serie 
    Serial.print("Temperature = ");
    Serial.print(Temperature);
    Serial.println(" °C");

    Serial.print("Light = ");
    Serial.print(AmbientSensorValeur);
    Serial.println("LUX");
}

