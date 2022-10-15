#include <arduino-timer.h>


//Pocet desek
int POCET_DESEK = 1;


// Globalni vstupy / vystupy
int VSTUP_POVOLENI = 2; //Zaciname az s pinem 2, 0a1 jsou na komunikaci s USB
int VYSTUP_POVOLENI = 3; 


// Ukazka pro 1 desku
int VSTUP_DESKA_1 = 4;
int PRIORITA_VSTUP_DESKA_1 = 5;
int VYSTUP_DESKA_1_NA_PRIORITU = 6;
int VYSTUP_DESKA_1_NA_TLACITKO = 7;


void 


// TODO: vyresit inicializaci se vstupama a vystupama ruznyma
class vnitrniStavDeska{
  int VSTUP_DESKA = 0; 
  int PRIORITA_VSTUP_DESKA = 0;
  int VYSTUP_DESKA_NA_PRIORITU = 0;
  int VYSTUP_DESKA_NA_TLACITKO = 0;
  int stav_vstup_povoleni = 0;
  int stav_vstup_deska = 0;
  int stav_priorita_vstup_deska = 0;
  
  
  public:
    void first_setup(int vstup_deska_pin, int priorita_vstup_deska_pin, int vystup_deska_na_prioritu_pin, int vystup_deska_na_tlacitko_pin){
      VSTUP_DESKA = vstup_deska_pin;
      PRIORITA_VSTUP_DESKA = priorita_vstup_deska_pin;
      VYSTUP_DESKA_NA_PRIORITU = vystup_deska_na_prioritu_pin;
      VYSTUP_DESKA_NA_TLACITKO = vystup_deska_na_tlacitko_pin;

      stav_vstup_povoleni = digitalRead(VSTUP_POVOLENI);
      stav_vstup_deska = digitalRead(VSTUP_DESKA);
      stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
      
    }
    void start_kontrola_stavu(){
      // first read all inputs
      stav_vstup_povoleni = digitalRead(VSTUP_POVOLENI);
      
      // Pokud prijde povoleni GND
      if(stav_vstup_povoleni == 0){
        // Zacni merit 5s, do te doby musi nastat tady zmena
               
        // pokud prijde zadost o zmenu stavu a je povolena
        if (stav_vstup_deska != digitalRead(VSTUP_DESKA)){
          stav_vstup_deska = digitalRead(VSTUP_DESKA);
          // Digital write permanentni
          digitalWrite(VYSTUP_DESKA_NA_PRIORITU, stav_vstup_deska);
          // TODO: Fix somehow this delay
          delay(1);
          stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
          
          // Zkontroluj jestli se něco pokazilo
          if(stav_vstup_deska != stav_priorita_vstup_deska){
          // TODO: digitalni impuls 0.1 s
            digitalWrite(VYSTUP_DESKA_NA_TLACITKO, stav_priorita_vstup_deska);
          }  
        }
      }
      //TODO: fix this
    }

  
};

vnitrniStavDeska *pole_desek[10];


void setup() {
  Serial.begin(9600);
  // Nastaveni vstupu pro POVOLENI -----------------------------------------------
  pinMode(VSTUP_POVOLENI, INPUT);
  pinMode(VYSTUP_POVOLENI, OUTPUT);
  
  // Nastaveni pinu pro DESKU 1 --------------------------------------------------
  pinMode(VSTUP_DESKA_1, INPUT);  
  pinMode(PRIORITA_VSTUP_DESKA_1, INPUT);  
  pinMode(VYSTUP_DESKA_1_NA_PRIORITU, OUTPUT);
  pinMode(VYSTUP_DESKA_1_NA_TLACITKO, OUTPUT);

  // Tady jsou vsechny definice desek
  pole_desek[0] = new vnitrniStavDeska();
  pole_desek[0] -> first_setup(VSTUP_DESKA_1, PRIORITA_VSTUP_DESKA_1, VYSTUP_DESKA_1_NA_PRIORITU, VYSTUP_DESKA_1_NA_TLACITKO);

}

void loop() {
  for(int i = 0; i < POCET_DESEK;i++){
    pole_desek[i] -> start_kontrola_stavu();
  }






  

}
