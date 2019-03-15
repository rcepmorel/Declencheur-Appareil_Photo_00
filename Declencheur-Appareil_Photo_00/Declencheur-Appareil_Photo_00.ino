/*DÉCLENCHEUR D'APPAREIL PHOTO ET DE FLASHS
* 
* Auteur : Richard Morel
*     2018-11-09
* 
* Modification
*     2018-11-17
*     - Mesure du voltage d'entrée
*     - Ajout du clignotement de la DEL Bleu
*       Relié sur la broche D33
*     
*/

const int DelBleue = 33; // DEL bleue reliée au GPIO33

// Références pour la conversion Analog/Digital
// https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/v2.1.1/api-reference/peripherals/adc.html
// https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/adc.html
#include <driver/adc.h>   
//ADC1_CHANNEL_7 D35 35

  // La tension pleine échelle par défaut du CAD (ADC) est 1,1 V
  // Pour lire des tensions plus élevées (jusqu’à la tension maximale de la broche, généralement 3,3 V), 
  // vous devez régler une atténuation du signal> 0 dB pour le canal du CAD quand VDD_A est 3.3V:
  // Une atténuation de 0 dB (ADC_ATTEN_0db) donne une tension à pleine échelle de 1,1 V
  // Une atténuation de 2,5 dB (ADC_ATTEN_2_5db) donne une tension à pleine échelle de 1,5 V
  // Une atténuation de 6 dB (ADC_ATTEN_6db) donne une tension à pleine échelle de 2,2 V
  // Une atténuation de 11 dB (ADC_ATTEN_11db) donne une tension à pleine échelle de 3,9 V (voir la remarque ci-dessous)
  // La tension à pleine échelle est la tension correspondant à une lecture maximale
  // Remarque : à 11 dB d'atténuation, la tension maximale est limitée par VDD_A, et non par la tension à pleine échelle.
  // En fonction de la largeur de bit configurée par ADC, cette valeur est: 4095 pour 12 bits, 2047 pour 11 bits, 1023 pour 10 bits, 511 pour 9 bits
  // 

  //En raison des caractéristiques du CAD, les résultats les plus précis sont obtenus dans les plages de tension approximatives suivantes:
  // - 0dB    d'atténuation (ADC_ATTEN_DB_0) entre 100 and 950mV
  // - 2.5dB  d'atténuation (ADC_ATTEN_DB_2_5) entre 100 and 1250mV
  // - 6dB    d'atténuation (ADC_ATTEN_DB_6) entre 150 to 1750mV
  // - 11dB   d'atténuation (ADC_ATTEN_DB_11) entre 150 to 2450mV
  // - Pour une précision maximale, utilisez les API d’étalonnage ADC et mesurez les tensions dans les plages recommandées.
 
float VoltMesure = 0;
float vpeDvValmx_0DB = 1.1/4095; // 12 bits de résolution
float vvpeDvValmx_2_5DB = 1.5/4095;
float vpeDvValmx_6DB = 2.0/4095;
float vpeDvValmx_11DB = 3.9/4095;

float calibrationDiviseurTSN = 7.99/8.70;      // valeur réelle diviser par VoltMesure lorsque calibrationDiviseurTSN égal à 1
float calibrationDiviseurTsnTrtPolyn = 7.99/8.07; // valeur réelle diviser par VoltMesure lorsque calibrationDiviseurTsn égal à 1


// ----------------------------------------------------------------------------- 
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// ----------------------------------------------------------------------------- 
void setup() {
  Serial.begin(9600);
  
  pinMode(DelBleue, OUTPUT);
  
  // ***** Configuration pour faire des mesures sur D35 (voltage de la source d'alimentation) ******
  adc1_config_width(ADC_WIDTH_BIT_12); // Définie la résolution (0 à 4095)
  

  adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11); //Le voltage maximum au GPIO est de 3.3V

  Serial.println("PRÊT");
}

// ----------------------------------------------------------------------------- 
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP 
// ----------------------------------------------------------------------------- 
void loop() {

  // Méthode de la règle de trois
  int DigitalValueVoltAlimCtrl = analogRead(35);
  float VoltAlimCtrlReduit = DigitalValueVoltAlimCtrl * vpeDvValmx_11DB;
  VoltMesure = VoltAlimCtrlReduit*122/22; // Diviseur de tension 100K, 22K
  VoltMesure = VoltMesure*calibrationDiviseurTSN; // correction dû aux valeurs imprécises du diviseur de tension

  Serial.print(DigitalValueVoltAlimCtrl);Serial.print(" DigitalValueVoltAlimCtrl ");Serial.print(VoltAlimCtrlReduit);Serial.print(" VoltAlimCtrlReduit  ");
  Serial.print(VoltMesure);Serial.println(" VoltMesure"); 

  // Méthode de G6EJD  (applicable si l'atténuation est 11dB et la résolution est 12 bits)
  float reading = analogRead(35);
  float VoltAlimCtrlReduitTrtPolyn = -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
  float VoltMesureTrtPolyn = VoltAlimCtrlReduitTrtPolyn*122/22; // Diviseur de tension 100K, 22K
  VoltMesureTrtPolyn = VoltMesureTrtPolyn*calibrationDiviseurTsnTrtPolyn; // correction dû aux valeurs imprécises du diviseur de tension
  
  Serial.print(reading);Serial.print(" DigitalValueVoltAlimCtrl ");Serial.print(VoltAlimCtrlReduitTrtPolyn);Serial.print(" VoltAlimCtrlReduit  ");
  Serial.print(VoltMesureTrtPolyn);Serial.println(" VoltMesure"); 
  Serial.println(" "); 

  digitalWrite(DelBleue, HIGH);   // Allume le DEL (voltage HAUT)
  delay(1000);                    // Attend X millisecondes
  digitalWrite(DelBleue, LOW);    // Éteint le DEL (voltage BAS)
  delay(1000);                    // Attend X millisecondes
  
}

