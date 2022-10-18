#include <arduino-timer.h>


//Pocet desek
int POCET_DESEK = 1;

// Udava se v milisekundach
int DOBA_CEKANI_NA_POVOLENI = 5000; 
int DOBA_CEKANI_NA_PRIORITU = 1000; 


// Globalni vstupy / vystupy
int VSTUP_POVOLENI = 2; //Zaciname az s pinem 2, 0a1 jsou na komunikaci s USB
int VYSTUP_POVOLENI = 3; 


// Ukazka pro 1 desku
int VSTUP_DESKA_1 = 4;
int VYSTUP_DESKA_1_1 = 5;
int VYSTUP_DESKA_1_2 = 6;
int VYSTUP_DESKA_1_3 = 6;


template<typename vnitrniStavDeska>
// Definice funkce, kterou zavolat, kdyz nastane interrupt
int nastalaZmenaNaVstupu(vnitrniStavDeska *p){
  p->priselInterrupt();
  return 0;
}

template<typename vnitrniStavDeska>
int prisloPovoleniInterrupt(vnitrniStavDeska *ukazatel_na_vsechny_desky){
  for(int i = 0; i < POCET_DESEK;i++){
    ukazatel_na_vsechny_desky[i]-> prisloPovoleni();
  }
  return 0;
}

Timer<10> timer;

void impulzNaPin(int pin, int hodnotaPulzu){
  digitalWrite(pin, hodnotaPulzu);
}


class vnitrniStavDeska{
  int VSTUP_DESKA = 0; 
  int VYSTUP_1 = 0;
  int VYSTUP_2 = 0;
  int VYSTUP_3 = 0;




  public:
    // Ukazatel sam na sebe
    vnitrniStavDeska *p;

    // aktivniStav definuje aktualni stav naseho systemu (dle obrazku 0 az 6)
    int aktivniStav = 0;
    
    // Definice vsech funkci potrebnych k simulovani chovani celeho systemu
    void prisloPovoleni(){
        
        switch (aktivniStav){
        // Zakladni stav nic nedelej
        case 0:
          // Zacni pocitat 5 sekund
          aktivniStav = 4;
          break;
  
      }
    }
   

    void priselInterruptNaVstup(){
        
        switch (aktivniStav){
        // Zakladni stav nic nedelej
        case 0:
          aktivniStav = 1;
          break;
  
        // Zapis na vystup 3
        case 2:
          aktivniStav = 3;
          break;
          
        // Cekame na zmenu na interrupt
        case 4:
          aktivniStav = 5;
          break;
      }
    }

    
    void zkontrolujStav(){

     switch (aktivniStav){
      // Zakladni stav nic nedelej
      case 0:
        break;

      // Zapis na vystup 3
      case 1:
        zapisNaVystupTlacitkaGndImpulz();
        aktivniStav = 2;
        break;
        
      // Cekame na zmenu na interrupt
      case 2:
        break;

      // Cekame na zmenu na interrupt
      case 3:
        aktivniStav = 0;
        break;

      // Prijde povoleni interruptem
      case 4:
        break;
        
      // Cekame na zmenu na interrupt
      case 5:
        // Zapsat data na vystup
        aktivniStav = 6;
        break;
        
      // Cekame na zmenu na interrupt
      case 6:
        //Vyslat impulz na povoleni GND
        aktivniStav = 0;
        break;
     
     
     }
      
    }

  void zapisNaVystupTlacitkaGndImpulz(){
    timer.in(10, impulzNaPin, VYSTUP_3);
    timer.in(250, impulzNaPin, VYSTUP_3);
  }
   
    
    void first_setup(int vstup_deska_pin, int vystup_1 , int vystup_2, int vystup_3){
      pinMode(vstup_deska_pin, INPUT_PULLUP);  
      pinMode(vystup_deska_na_prioritu_pin, OUTPUT);
      pinMode(vystup_deska_na_tlacitko_pin, OUTPUT);
      attachInterrupt(digitalPinToInterrupt(VSTUP_DESKA_1), nastalaZmenaNaVstupu(p), CHANGE);
      VSTUP_DESKA_PIN = vstup_deska_pin
      VYSTUP_1 = vystup_1
      VYSTUP_2 = vystup_2
      VYSTUP_3 = vystup_3
    }
    

   
};





// Tohle se musi taky zmenit cislo uvnitr [] musi odpovidat poctu desek
vnitrniStavDeska *pole_desek[1];


void setup() {
  Serial.begin(9600);
  
  // Nastaveni vstupu pro POVOLENI -----------------------------------------------
  pinMode(VSTUP_POVOLENI, INPUT_PULLUP);
  pinMode(VYSTUP_POVOLENI, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(VSTUP_POVOLENI), prisloPovoleniInterrupt(pole_desek), CHANGE);
  
  // Tady jsou vsechny definice desek
  pole_desek[0] = new vnitrniStavDeska();
  pole_desek[0] -> first_setup(VSTUP_DESKA_1, VYSTUP_DESKA_1_1, VYSTUP_DESKA_1_2, VYSTUP_DESKA_1_3);

}

void loop() {
  for(int i = 0; i < POCET_DESEK;i++){
    pole_desek[i] -> timer.tick();
    pole_desek[i]-> zkontrolujStav();
  }
}
