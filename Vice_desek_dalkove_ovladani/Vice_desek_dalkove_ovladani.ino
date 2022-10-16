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
int PRIORITA_VSTUP_DESKA_1 = 5;
int VYSTUP_DESKA_1_NA_PRIORITU = 6;
int VYSTUP_DESKA_1_NA_TLACITKO = 7;



class vnitrniStavDeska{
  int VSTUP_DESKA = 0; 
  int PRIORITA_VSTUP_DESKA = 0;
  int VYSTUP_DESKA_NA_PRIORITU = 0;
  int VYSTUP_DESKA_NA_TLACITKO = 0;
  int stav_vstup_povoleni = 0;
  int stav_vstup_deska = 0;
  int stav_priorita_vstup_deska = 0;
  bool prvni_zavolani_kontroly_stavu = true;
  bool prvni_zavolani_kontrola_priority = true;
  bool blocker = false;
  bool stav_zmenen = false;


  
  unsigned long referencni_cas = 0;
  unsigned long referencni_cas_priorita = 0;
  unsigned long referencni_cas_bez_gnd= 0;

  private:
    void reset_promennych(){
      prvni_zavolani_kontroly_stavu = true;
      prvni_zavolani_kontrola_priority = true;
      blocker = false;
      stav_vstup_povoleni = digitalRead(VSTUP_POVOLENI);
      stav_vstup_deska = digitalRead(VSTUP_DESKA);
      stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
      Serial.println(stav_vstup_deska );
      Serial.println(stav_priorita_vstup_deska );
      digitalWrite(VYSTUP_POVOLENI, HIGH);
      digitalWrite(VYSTUP_DESKA_NA_TLACITKO, HIGH);
      digitalWrite(VYSTUP_DESKA_NA_PRIORITU, stav_priorita_vstup_deska);
    }
 
  public:
    void first_setup(int vstup_deska_pin, int priorita_vstup_deska_pin, int vystup_deska_na_prioritu_pin, int vystup_deska_na_tlacitko_pin){
      pinMode(vstup_deska_pin, INPUT_PULLUP);  
      pinMode(priorita_vstup_deska_pin, INPUT_PULLUP);  
      pinMode(vystup_deska_na_prioritu_pin, OUTPUT);
      pinMode(vystup_deska_na_tlacitko_pin, OUTPUT);
      
      VSTUP_DESKA = vstup_deska_pin;
      PRIORITA_VSTUP_DESKA = priorita_vstup_deska_pin;
      VYSTUP_DESKA_NA_PRIORITU = vystup_deska_na_prioritu_pin;
      VYSTUP_DESKA_NA_TLACITKO = vystup_deska_na_tlacitko_pin;

      stav_vstup_povoleni = digitalRead(VSTUP_POVOLENI);
      stav_vstup_deska = digitalRead(VSTUP_DESKA);
      stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
      
      prvni_zavolani_kontroly_stavu = true;
      prvni_zavolani_kontrola_priority = true;
      blocker = false;
      digitalWrite(VYSTUP_POVOLENI, HIGH);
      digitalWrite(VYSTUP_DESKA_NA_TLACITKO, HIGH);
      digitalWrite(VYSTUP_DESKA_NA_PRIORITU, stav_priorita_vstup_deska);
      Serial.println("Prvni setup");
    }
    
    void start_kontrola_stavu(int cas_ted){
      // first read all inputs
      stav_vstup_povoleni = digitalRead(VSTUP_POVOLENI);
      stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
      
      // Pokud prijde povoleni a je na GND a zajima me to jenom jednou
      if(stav_vstup_povoleni == 0 and blocker == false){
        Serial.println("Prisla GND na povoleni, blocker false");
        if (prvni_zavolani_kontroly_stavu == true){
          referencni_cas = cas_ted;
          prvni_zavolani_kontroly_stavu = false;
        }
        // Po 5 sekundach zavolej funkci, ktera zkontroluje vsechny stavy ze jsou ok, jinak vysli impuls na povoleni, aby se vyplo
        
        if ((cas_ted - referencni_cas) >= DOBA_CEKANI_NA_POVOLENI){
          Serial.println("Uběhlo 5 sekund a nebyla zmena na pinech");
          Serial.println("Uběhlo 5 sekund a nebyla zmena na pinech");
          Serial.println("Uběhlo 5 sekund a nebyla zmena na pinech");
          
          if (cas_ted - referencni_cas < DOBA_CEKANI_NA_POVOLENI+200){
            digitalWrite(VYSTUP_POVOLENI, LOW);
            Serial.println("Zapisuju LOW na povoleni"); 
            
          }
          else{
            digitalWrite(VYSTUP_POVOLENI, HIGH);
            reset_promennych();
            Serial.println("Zapisuju HIGH na povoleni");  
          }
          
          }
        }

        if (stav_vstup_deska != digitalRead(VSTUP_DESKA)){
        // mame 5 sekund na to, aby nastala zmena
        zkontroluj_stav_od_povoleni(cas_ted);
        }

      //pokud gnd neni a stejne zmackneme ze chceme menit stav
      if(stav_vstup_povoleni == 1){
        if (stav_vstup_deska != digitalRead(VSTUP_DESKA) and blocker == false){
          Serial.println("Prisel pozadavek menit stav desky bez POVOLENI");
          stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
          referencni_cas_bez_gnd = cas_ted;
          blocker = true;
        }
        else if(stav_priorita_vstup_deska != stav_vstup_deska and blocker == false){
          stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
          referencni_cas_bez_gnd = cas_ted;
          Serial.println("Neshoduji se");
          blocker = true;
        }
        if(blocker == true){
          Serial.println(cas_ted - referencni_cas_bez_gnd);
          if (cas_ted - referencni_cas_bez_gnd <= 300){
            digitalWrite(VYSTUP_DESKA_NA_TLACITKO, 0);
            digitalWrite(VYSTUP_DESKA_NA_PRIORITU, stav_priorita_vstup_deska);
          }
          else{
            Serial.println("Chci resetovat stav desky");
            digitalWrite(VYSTUP_DESKA_NA_TLACITKO, 1);
            reset_promennych();
          }
          
        }

      }
      
    }
    
    void zkontroluj_stav_od_povoleni(int cas_ted){
        // pokud prijde zadost o zmenu stavu a je povolena zmena
        // Tohle nastane pokazde pri prvni iteraci
        if (prvni_zavolani_kontroly_stavu == true){
          stav_vstup_deska = digitalRead(VSTUP_DESKA);
          // Digital write permanentni
          digitalWrite(VYSTUP_DESKA_NA_PRIORITU, stav_vstup_deska);
          prvni_zavolani_kontroly_stavu = false;
        }
        else if (prvni_zavolani_kontroly_stavu == false){
          // po uplynuti jedne sekundy se podivej na prioritu
          Serial.println("Cekame na prioritni vstup 1 sekundu");

          if (cas_ted - referencni_cas >= DOBA_CEKANI_NA_PRIORITU){
            zkontroluj_stav_od_priority(cas_ted);
          }
        }
    }
    
    bool zkontroluj_stav_od_priority(int cas_ted){
      Serial.println("Prioritou");
      stav_priorita_vstup_deska = digitalRead(PRIORITA_VSTUP_DESKA);
          // Jestli nesedi vystup s prioritou
          if(stav_vstup_deska != stav_priorita_vstup_deska){
            // Vytvor impuls 0.1s dlouhy
            Serial.println("Nesedi vystup s prioritou");
            // TODO: check tohle
            if(prvni_zavolani_kontrola_priority == true){
                digitalWrite(VYSTUP_DESKA_NA_TLACITKO, stav_priorita_vstup_deska);
                prvni_zavolani_kontrola_priority = false;
                referencni_cas_priorita = cas_ted;
            }
            else if(cas_ted - referencni_cas_priorita >= 100){
              digitalWrite(VYSTUP_DESKA_NA_TLACITKO, !stav_priorita_vstup_deska);
            }
          }
          else{
            Serial.println("Sedi vystup s prioritou");
            // Muze dojit k resetu promennych, vsechno bylo spravne nastaveno
            Serial.println(stav_priorita_vstup_deska);
            stav_vstup_deska = digitalRead(VSTUP_DESKA);
            digitalWrite(VYSTUP_DESKA_NA_PRIORITU, stav_vstup_deska);
            
          }
    }
   
};

vnitrniStavDeska *pole_desek[10];


void setup() {
  Serial.begin(9600);
  // Nastaveni vstupu pro POVOLENI -----------------------------------------------
  pinMode(VSTUP_POVOLENI, INPUT_PULLUP);
  pinMode(VYSTUP_POVOLENI, OUTPUT);
  
  // Tady jsou vsechny definice desek
  pole_desek[0] = new vnitrniStavDeska();
  pole_desek[0] -> first_setup(VSTUP_DESKA_1, PRIORITA_VSTUP_DESKA_1, VYSTUP_DESKA_1_NA_PRIORITU, VYSTUP_DESKA_1_NA_TLACITKO);

}

void loop() {
  unsigned long cas_ted = millis();
  for(int i = 0; i < POCET_DESEK;i++){
    pole_desek[i] -> start_kontrola_stavu(cas_ted);
  }
}
