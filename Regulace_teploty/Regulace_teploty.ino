/*************************************************** 
Program pro rizeni topneho elementu na zahrivani nanodratu

VSTUP: informace z MAX31865 = prevodnik odporu PT100/1000 na teplotu (PINY: 10, 11, 12, 13)

VYSTUP: Spinani topeni

 ****************************************************/

#include <Adafruit_MAX31865.h>

// Pouziti softwarove SPI komunikace na pinech: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);


// Velikost referencniho rezistoru. Pouziti 430.0 ohm pro PT100 a 4300.0 pro PT1000
#define RREF                                        4300.0
// 'Nominalni' 0-stupnu-Celsia odporu senzoru
// 100.0 pro PT100, 1000.0 pro PT1000
#define RNOMINAL                                    1000.0

//************************* NASTAVENI PINU A PRACE S TEPLOTOU **************************
#define ZADANA_TEPLOTA_PIN                          A0
#define TOPENI_PIN                                  7
#define TEPLOTNI_KONSTANTA                          0.2932
#define HYSTEREZNI_KONSTANTA_TEPLOTY                5
//**************************************************************************************


//Funkce pro cteni chyby se senzorem, vypisuje do serioveho monitoru
//Bude se volat jen v pripade testovani
void zkontroluj_chybu_v_senzoru(){
  
  // Pokud neco nefunguje precti chybu
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
  Serial.println();
}

//Zapni nebo vypni topeni podle toho jaka je aktualni teplota
void ovladej_topeni(uint16_t aktualni_teplota, uint16_t zadana_teplota){
  
  //Mohou nastat 3 pripady
  //Topeni se muze vypnout, protoze prectena teplota se pohybuje v rozmezi +-5 stupnu od teploty
  if((zadana_teplota + HYSTEREZNI_KONSTANTA_TEPLOTY) > aktualni_teplota > (zadana_teplota - HYSTEREZNI_KONSTANTA_TEPLOTY)) {
    digitalWrite(TOPENI_PIN, 0);
  }
  //Topeni zapnout
  else{
    digitalWrite(TOPENI_PIN, 1);
  }
  Serial.print("Pozadovana teplota: "); Serial.println(zadana_teplota);
  Serial.print("Aktualni teplota: "); Serial.println(aktualni_teplota);
  
}

//Definice jako globalni promenne
uint16_t ZADANA_TEPLOTA = 0;

//Nastaveni seriove komunikace s PC a komunikace s MAX31865
void setup() {
  Serial.begin(115200);
  Serial.println("Jitka Stefkova - diplomova prace regulace teploty");

  thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary

  //********************* NASTAVENI ZADANE TEPLOTY ****************************
  //Pin pro potenciometr, ze ktere se bude cist zadana teplota
  //Ctene hodnoty budou 0 (100 Stupnu celsia) az 1023 (400 Stupnu celsia) 
  uint16_t x = analogRead(ZADANA_TEPLOTA_PIN);

  //Ted je potreba provest prepocet na zadanou teplotu
  ZADANA_TEPLOTA = x*TEPLOTNI_KONSTANTA + 100;

  //Pokud by se neco rozbilo, je treba mit osetreno, ze se zadana_teplota nestane moc velkou
  if(ZADANA_TEPLOTA > 400){
    ZADANA_TEPLOTA = 400;
  }
  Serial.print("Pozadovana teplota na regulaci: "); Serial.println(ZADANA_TEPLOTA);

  
  //***************************************************************************
}


void loop() {
  //*****************************CTENI DAT ZE SENZORU *************************
  uint16_t rtd = thermo.readRTD();
  float ratio = rtd;
  ratio = ratio / 32768;
  
  uint16_t odpor_pt = RREF*ratio; //Vypocet odporu
  uint16_t teplota_pt = thermo.temperature(RNOMINAL, RREF); // Vypocet teploty

  //***************************** VYPIS NA SERIOVY MONITOR *********************
  //Serial.print("RTD value: "); Serial.println(rtd);
  //Serial.print("Odpor[ohm] = "); Serial.println(odpor_pt,8);
  //Serial.print("Teplota [Celsius] = "); Serial.println(teplota_pt);

  //***************************** KONTROLA CHYB V KOMUNIKACI SE SENZOREM *********************
  zkontroluj_chybu_v_senzoru();

  //Pokud se v programu dostaneme az sem (a v konzoli neni error)
  //Mame aktualni teplotu a muzeme se zaridit podle toho s topenim
  ovladej_topeni(teplota_pt, ZADANA_TEPLOTA);
  
  
  //***************************** CTENI JEDNOU ZA SEKUNDU *********************
  delay(1000);
}
