//Pocet desek
int POCET_DESEK = 1;

// Udava se v milisekundach
int DOBA_CEKANI_NA_POVOLENI = 5000; 


// Globalni vstupy / vystupy
int VSTUP_POVOLENI = 2; //Zaciname az s pinem 2, 0 a 1 jsou na komunikaci s USB
int VYSTUP_POVOLENI = 3; 

// Ukazka pro 1 desku
int VSTUP_DESKA_1 = 4;
int VYSTUP_DESKA_1_1 = 5; // Vystup na rele 1
int VYSTUP_DESKA_1_2 = 6; // Vystup na rele 2
int VYSTUP_DESKA_1_3 = 7; // Vystup zpet na tlacitko


class vnitrniStavDeska{
  int VSTUP_DESKA = 0; 
  int VYSTUP_1 = 0; 
  int VYSTUP_2 = 0; 
  int VYSTUP_3 = 0; 

  unsigned long referencni_cas = 0;
  unsigned long cas_ted = 0;

  int interrupt_zmena_stav = -1;
  int minulyStavVstupu = -1;
  int stavPovoleni = -1;

  bool sestupna_hrana = false;
  bool vzestupna_hrana = false;

  public:
    // Ukazatel sam na sebe
    vnitrniStavDeska *p;
    
    // aktivniStav definuje aktualni stav naseho systemu (dle obrazku 0 az 6, v -1 zaciname)
    int aktivniStav = -1;
    int minulyAktivniStav = -1;
    // Definice vsech funkci potrebnych k simulovani chovani celeho systemu
    void prisloPovoleni(){
        switch (aktivniStav){
          // Prechod ze zakladniho stavu na cekani na dobu povoleni
          case 0:  
            Serial.print("Stav jde na 4");
            aktivniStav = 4;
            break;
        }
      
    }
   

    void prislaZmenaNaVstup(){
      Serial.println("Prisla zmena na vstup");
        delay(100);
        switch (aktivniStav){
        case 0:
          aktivniStav = 1;
          break;
  
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
    cas_ted = millis();
    delay(10);
    Serial.println(aktivniStav);
    // Pokud bude povoleni na GND, vime ze prisel pozadavek
    stavPovoleni = digitalRead(VSTUP_POVOLENI);
    if (stavPovoleni == 0){
      prisloPovoleni();
    }

    //Detekce zmeny na vstupu
    if(minulyStavVstupu != digitalRead(VSTUP_DESKA)){
        prislaZmenaNaVstup();
    }

     switch (aktivniStav){
      // Zakladni stav nic nedelej
      case 0:
        minulyAktivniStav = 0;
        break;

      // Zapis na vystup 3
      case 1:
        if (minulyAktivniStav != aktivniStav){
          zapisNaPin(0, VYSTUP_3, 1, 0);
        }
        zapisNaPin(150, VYSTUP_3, 2, 1);
        minulyAktivniStav = 1;
        break;
        
      // Cekame na zmenu na interrupt
      case 2:
        minulyAktivniStav = 2;
        break;

      // Cekame na zmenu na interrupt
      case 3:
        aktivniStav = 0;
        minulyAktivniStav = 3;
        break;

      // Prijde povoleni interruptem
      case 4:
        // Za 5 sekund posli impulz
        if (minulyAktivniStav != aktivniStav){
          referencni_cas = cas_ted;
        }
        zapisNaPin(DOBA_CEKANI_NA_POVOLENI, VYSTUP_POVOLENI, 6, 0);
        minulyAktivniStav = 4;
        break;
        
      // Cekame na zmenu na interrupt
      case 5:
        Serial.print(minulyAktivniStav);
        Serial.print("Stav: " + minulyStavVstupu);
        Serial.print( "Stav: " + digitalRead(VSTUP_DESKA));
        // Detekujeme vzestupnou hranu
        if (minulyStavVstupu == 0 and digitalRead(VSTUP_DESKA) == 1){
          if (minulyAktivniStav != aktivniStav){
            zapisNaPin(0, VYSTUP_1, 5, 1);
            referencni_cas = cas_ted;
          }
          vzestupna_hrana = true;
          
        }
        // Detekujeme sestupnou hranu
        else if(minulyStavVstupu == 1 and digitalRead(VSTUP_DESKA) == 0){
          if (minulyAktivniStav != aktivniStav){
            zapisNaPin(0, VYSTUP_2, 5, 1);
            referencni_cas = cas_ted;
          }
          sestupna_hrana = true;
        }
        Serial.println(sestupna_hrana);
        Serial.println(vzestupna_hrana);
        if (sestupna_hrana == true){
            zapisNaPin(500, VYSTUP_2, 6, 0);
        }
        else if (vzestupna_hrana == true){
          zapisNaPin(500, VYSTUP_1, 6, 0);
        }
        minulyAktivniStav = 5;
        break;
        
      // Cekame na zmenu na interrupt
      case 6:
        if (minulyAktivniStav == 5){
          minulyStavVstupu = digitalRead(VSTUP_DESKA);
          vzestupna_hrana = false;
          sestupna_hrana = false;
        }
        //Vyslat impulz na povoleni GND
        if (minulyAktivniStav != aktivniStav){
          zapisNaPin(0, VYSTUP_POVOLENI, 6, 0);
          referencni_cas = cas_ted;  
        }  
        zapisNaPin(100, VYSTUP_POVOLENI, 0, 1);

        minulyAktivniStav = 6;
        break;
     
     
     }
      
    }

  void zapisNaPin(int doba_cekani, int pin, int aktivni_budouci_stav, int hodnota_na_vystup){
    if(cas_ted - referencni_cas >= doba_cekani){
      digitalWrite(pin, hodnota_na_vystup);
      Serial.print("Pin:");
      Serial.print(pin);
      Serial.print(" Doba cekani: ");
      Serial.print(doba_cekani);
      Serial.print(" Aktualni stav: ");
      Serial.print(aktivniStav);
      Serial.print(" Budouci stav: ");
      Serial.print(aktivni_budouci_stav);
      // TODO: mozny fix bude pridat sem delay
      aktivniStav = aktivni_budouci_stav;
      referencni_cas = cas_ted;
    }
  }
   
    
    void first_setup(int vstup_deska_pin, int vystup_1 , int vystup_2, int vystup_3){
      //Serial.println("Prvni setup");
      pinMode(vstup_deska_pin, INPUT_PULLUP);
      pinMode(vystup_1, OUTPUT);
      pinMode(vystup_2, OUTPUT);
      pinMode(vystup_3, OUTPUT);
      
      digitalWrite(vystup_1, LOW); // Pro relatka LOW
      digitalWrite(vystup_2, LOW); // Pro relatka LOW
      digitalWrite(vystup_3, HIGH); 

      VSTUP_DESKA = vstup_deska_pin;
      VYSTUP_1 = vystup_1;
      VYSTUP_2 = vystup_2;
      VYSTUP_3 = vystup_3;

      minulyStavVstupu = digitalRead(VSTUP_DESKA);
      stavPovoleni = digitalRead(VSTUP_POVOLENI);

      aktivniStav = 0;


    }
    

   
};


// Tohle se musi taky zmenit cislo uvnitr [] musi odpovidat poctu desek
vnitrniStavDeska *pole_desek[1];


void setup() {
  Serial.begin(112500);
  // Nastaveni vstupu pro POVOLENI -----------------------------------------------
  pinMode(VSTUP_POVOLENI, INPUT_PULLUP);
  pinMode(VYSTUP_POVOLENI, OUTPUT);
  digitalWrite(VYSTUP_POVOLENI, HIGH);
  // Tady jsou vsechny definice desek
  pinMode(VSTUP_DESKA_1, INPUT_PULLUP);
  pole_desek[0] = new vnitrniStavDeska();
  pole_desek[0] -> first_setup(VSTUP_DESKA_1, VYSTUP_DESKA_1_1, VYSTUP_DESKA_1_2, VYSTUP_DESKA_1_3);


}

void loop() {
  for(int i = 0; i < POCET_DESEK;i++){
    pole_desek[i]-> zkontrolujStav();
  }
}