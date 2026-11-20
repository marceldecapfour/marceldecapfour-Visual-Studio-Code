#include <Arduino.h>

struct Axe {
  uint8_t rouge;
  uint8_t jaune;
  uint8_t vert;
};

Axe axeNS = {25, 26, 27};  // feux Nord et Sud (cables en parallele)
Axe axeEW = {32, 5, 4};    // feux Est et Ouest (cables en parallele)

const unsigned long DUREE_VERT     = 15000UL;
const unsigned long DUREE_JAUNE    = 10000UL;
const unsigned long DUREE_SECURITE = 5000UL;  // tout-rouge entre les deux sens

const int NB_PHASES = 6;
enum Phase { NS_VERT = 0, NS_JAUNE, TOUT_ROUGE_1, EW_VERT, EW_JAUNE, TOUT_ROUGE_2 };

const char *NOM_PHASE[NB_PHASES] = {
  "NS vert", "NS jaune", "tout rouge", "EO vert", "EO jaune", "tout rouge"
};

void setAxe(const Axe &a, bool r, bool j, bool v) {
  digitalWrite(a.rouge, r ? HIGH : LOW);
  digitalWrite(a.jaune, j ? HIGH : LOW);
  digitalWrite(a.vert,  v ? HIGH : LOW);
}

void appliquerPhase(Phase p) {
  switch (p) {
    case NS_VERT:      setAxe(axeNS, false, false, true);  setAxe(axeEW, true,  false, false); break;
    case NS_JAUNE:     setAxe(axeNS, false, true,  false); setAxe(axeEW, true,  false, false); break;
    case TOUT_ROUGE_1: setAxe(axeNS, true,  false, false); setAxe(axeEW, true,  false, false); break;
    case EW_VERT:      setAxe(axeNS, true,  false, false); setAxe(axeEW, false, false, true);  break;
    case EW_JAUNE:     setAxe(axeNS, true,  false, false); setAxe(axeEW, false, true,  false); break;
    case TOUT_ROUGE_2: setAxe(axeNS, true,  false, false); setAxe(axeEW, true,  false, false); break;
  }
}

unsigned long dureePhase(Phase p) {
  switch (p) {
    case NS_VERT:
    case EW_VERT:    return DUREE_VERT;
    case NS_JAUNE:
    case EW_JAUNE:   return DUREE_JAUNE;
    default:         return DUREE_SECURITE;
  }
}

Phase phaseActuelle = NS_VERT;
unsigned long debutPhase = 0;

void setup() {
  Serial.begin(115200);
  pinMode(axeNS.rouge, OUTPUT); pinMode(axeNS.jaune, OUTPUT); pinMode(axeNS.vert, OUTPUT);
  pinMode(axeEW.rouge, OUTPUT); pinMode(axeEW.jaune, OUTPUT); pinMode(axeEW.vert, OUTPUT);
  appliquerPhase(phaseActuelle);
  debutPhase = millis();
  Serial.printf("Demarrage - phase: %s (%lu s)\n",
                NOM_PHASE[phaseActuelle], dureePhase(phaseActuelle) / 1000UL);
}

void loop() {
  unsigned long maintenant = millis();
  if (maintenant - debutPhase >= dureePhase(phaseActuelle)) {
    phaseActuelle = static_cast<Phase>((phaseActuelle + 1) % NB_PHASES);
    appliquerPhase(phaseActuelle);
    debutPhase = maintenant;
    Serial.printf("t=%lus  phase: %s (%lu s)\n", maintenant / 1000UL,
                  NOM_PHASE[phaseActuelle], dureePhase(phaseActuelle) / 1000UL);
  }
}
