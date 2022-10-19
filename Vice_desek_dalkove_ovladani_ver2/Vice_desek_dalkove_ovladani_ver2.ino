//Pocet desek
int POCET_DESEK = 1;

// Udava se v milisekundach
int DOBA_CEKANI_NA_POVOLENI = 5000; 
int DOBA_CEKANI_NA_PRIORITU = 1000; 


// Globalni vstupy / vystupy
int VSTUP_POVOLENI = 2; //Zaciname az s pinem 2, 0 a 1 jsou na komunikaci s USB
int VYSTUP_POVOLENI = 3; 


// Ukazka pro 1 desku
int VSTUP_DESKA_1 = 4;
int VYSTUP_DESKA_1_1 = 5;
int VYSTUP_DESKA_1_2 = 6;
int VYSTUP_DESKA_1_3 = 6;


template<typename vnitrniStavDeska>
// Definice funkce, kterou zavolat, kdyz nastane interrupt
int nastalaZmenaNaVstupu(vnitrniStavDeska *p){
  p->priselInterruptNaVstup();
  return 0;
}

template<typename vnitrniStavDeska>
int prisloPovoleniInterrupt(vnitrniStavDeska *ukazatel_na_vsechny_desky){
  for(int i = 0; i < POCET_DESEK;i++){
    ukazatel_na_vsechny_desky[i]-> prisloPovoleni();
  }
  return 0;
}



class vnitrniStavDeska{
  int VSTUP_DESKA = 0; 
  int VYSTUP_1 = 0;
  int VYSTUP_2 = 0;
  int VYSTUP_3 = 0;

  unsigned long referencni_cas = 0;
  unsigned long cas_ted = 0;

  int interrupt_zmena_stav = -1;

  public:
    // Ukazatel sam na sebe
    vnitrniStavDeska *p;
    
    bool reagujNaZmenuVystupu = true;
    
    
    // aktivniStav definuje aktualni stav naseho systemu (dle obrazku 0 az 6, v -1 zaciname)
    int aktivniStav = -1;
    
    // Definice vsech funkci potrebnych k simulovani chovani celeho systemu
    void prisloPovoleni(){
        
        switch (aktivniStav){
        // Zakladni stav nic nedelej
        case 0:  
          aktivniStav = 4;
          break;
  
      }
    }
   

    void priselInterruptNaVstup(){

        
        switch (aktivniStav){
        // Zakladni stav nic nedelej
        case 0:
          // TODO: mozna fix, pokud nebude fungovat
          interrupt_zmena_stav = digitalRead(VSTUP_DESKA_1);
          aktivniStav = 1;
          break;
  
        case 2:
          // TODO: mozna fix, pokud nebude fungovat
          interrupt_zmena_stav = digitalRead(VSTUP_DESKA_1);
          aktivniStav = 3;
          break;
          
        // Cekame na zmenu na interrupt
        case 4:
          // TODO: mozna fix, pokud nebude fungovat
          interrupt_zmena_stav = digitalRead(VSTUP_DESKA_1);
          aktivniStav = 5;
          break;
      }
    }

    
    void zkontrolujStav(){
    cas_ted = millis();
     switch (aktivniStav){
      // Zakladni stav nic nedelej
      case 0:
        break;

      // Zapis na vystup 3
      case 1:
        zapisNaPin(0, VYSTUP_3, 1, 0);
        zapisNaPin(150, VYSTUP_3, 2, 1);
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
        // Za 5 sekund posli impulz
        zapisNaPin(DOBA_CEKANI_NA_POVOLENI, VYSTUP_POVOLENI, 6, 0);
        break;
        
      // Cekame na zmenu na interrupt
      case 5:
        // Zapsat data na vystup
        // Podle toho jaka zmena nastala tak zapsat na vystup
        if (interrupt_zmena_stav == 0){
          zapisNaPin(0, VYSTUP_1, 5, 1);
          zapisNaPin(500, VYSTUP_1, 6, 0);
        }
        else if(interrupt_zmena_stav == 1){
          zapisNaPin(0, VYSTUP_2, 5, 1);
          zapisNaPin(500, VYSTUP_2, 6, 0);
        }
        
        break;
        
      // Cekame na zmenu na interrupt
      case 6:
        //Vyslat impulz na povoleni GND
        zapisNaPin(0, VYSTUP_POVOLENI, 6, 0);   
        zapisNaPin(100, VYSTUP_POVOLENI, 0, 1);
        break;
     
     
     }
      
    }

  void zapisNaPin(int doba_cekani, int pin, int aktivni_budouci_stav, int hodnota_na_vystup){
    if(cas_ted - referencni_cas >= doba_cekani){
      digitalWrite(pin, hodnota_na_vystup);
      // TODO: mozny fix bude pridat sem delay
      aktivniStav = aktivni_budouci_stav;
      referencni_cas = cas_ted;
    }
  }
   
    
    void first_setup(int vstup_deska_pin, int vystup_1 , int vystup_2, int vystup_3){
      pinMode(vstup_deska_pin, INPUT_PULLUP);  
      pinMode(vystup_1, OUTPUT);
      pinMode(vystup_2, OUTPUT);
      pinMode(vystup_3, OUTPUT);
      
      attachInterrupt(digitalPinToInterrupt(VSTUP_DESKA_1), nastalaZmenaNaVstupu(p), CHANGE);
      VSTUP_DESKA = vstup_deska_pin;
      VYSTUP_1 = vystup_1;
      VYSTUP_2 = vystup_2;
      VYSTUP_3 = vystup_3;

      aktivniStav = 0;
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
    pole_desek[i]-> zkontrolujStav();
  }
}
