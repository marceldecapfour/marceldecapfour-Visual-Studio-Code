# Feux de carrefour ESP32 (4 voies)

4 feux (Nord, Sud, Est, Ouest), 3 LED par feu (rouge, jaune, verte) = 12 LED.
Les feux face à face sont identiques et câbles en parallèle sur les mêmes GPIO
(6 GPIO au total, **une résistance par LED**).

## Temporisations (cycle de 60 s)

| Phase | Durée | Axe Nord-Sud | Axe Est-Ouest |
|---|---|---|---|
| 1 | 15 s | vert | rouge |
| 2 | 10 s | jaune | rouge |
| 3 | 5 s | rouge | rouge |
| 4 | 15 s | rouge | vert |
| 5 | 10 s | rouge | jaune |
| 6 | 5 s | rouge | rouge |

Rouge = 15 + 10 + 5 = **30 s** par axe. Vert = 15 s, jaune = 10 s.

## Câblage

| Feux | Couleur | GPIO ESP32 | Résistance |
|---|---|---|---|
| Nord + Sud | Rouge | 25 | 220 Ω |
| Nord + Sud | Jaune | 26 | 220 Ω |
| Nord + Sud | Verte | 27 | 150 Ω |
| Est + Ouest | Rouge | 32 | 220 Ω |
| Est + Ouest | Jaune | 5 | 220 Ω |
| Est + Ouest | Verte | 4 | 150 Ω |

Chaque LED : `GPIO → résistance → anode (+, patte longue)`, `cathode (–) → GND`.

```
ALIMENTATION 5 V (USB ou adaptateur 5 V >= 1 A)
   ├── 5V  ──► VIN (broche « 5V » de l'ESP32)
   └── GND ──► GND (ESP32) ──┬──► GND de TOUTES les LED (masse commune)
ESP32 DevKit
   ├── 3V3 ──► alimentation des LED via les résistances
   ├── GPIO25 ──[220Ω]──► (+) LED rouge  Nord/Sud
   ├── GPIO26 ──[220Ω]──► (+) LED jaune  Nord/Sud
   ├── GPIO27 ──[150Ω]──► (+) LED verte  Nord/Sud
   ├── GPIO32 ──[220Ω]──► (+) LED rouge  Est/Ouest
   ├── GPIO5  ──[220Ω]──► (+) LED jaune  Est/Ouest
   └── GPIO4  ──[150Ω]──► (+) LED verte  Est/Ouest
```

Broches sûres choisies : éviter GPIO 6–11 (flash interne), 34–39 (entrée seule), 12 et 15 (démarrage).

## Alimentations

1. **Test (recommandé)** : câble USB 5 V → port micro-USB de l'ESP32, LED alimentées par la broche **3V3** (max ~4 LED allumées en même temps ≈ 25 mA, OK).
2. **Autonome** : adaptateur **5 V ≥ 1 A** → broche **VIN (5V)** + **GND** (ne pas dépasser 5 V sur VIN).
3. **Feux 5 V / 12 V ou LED puissantes** : alimentation externe pour les LED + un MOSFET 2N7000 par canal :
   `+alim LED → [R] → (+) LED`, `cathode LED (–) → drain`, `source → GND (commun avec l'ESP32, obligatoire)`, `gate → [100Ω] → GPIO` + pulldown 100 kΩ sur la gate.

Si la LED verte est faible en 3V3 (Vf ≈ 3,2 V), passer sa résistance à 100 Ω ou utiliser l'option 3.

## Compilation / flash

Projet PlatformIO : `platformio.ini` + `src/main.cpp` fournis.
VS Code → extension PlatformIO → « New Project » → carte `esp32dev` → copier ces fichiers → **Upload**.
Moniteur série à 115200 bauds : chaque changement de phase s'affiche avec le temps écoulé.
