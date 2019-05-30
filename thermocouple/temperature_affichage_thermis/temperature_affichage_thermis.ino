#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 8
#define DIO 9

#define THERM_PIN A0
#define PHOTO_PIN A1

#define CELSIUS_TO_KELVIN 273.15
#define TENSION_ALIM  5
#define R             9880
#define Rr            1200

#define CONSTANT_A    1.31945e-3
#define CONSTANT_B    1.96097e-4
#define CONSTANT_C    2.91306e-7

#define NB_MEM_TEMP        4
#define NB_MEM_PHOT        10

TM1637Display display(CLK, DIO);

const uint8_t DEGRES[] = { SEG_A | SEG_B | SEG_F | SEG_G };
const uint8_t LETTRE_C[] = { display.encodeDigit(12) };
const uint8_t MOINS[] = { SEG_G };
const uint8_t RIEN[] = { 0 };

const uint8_t BITMASK_DOTS = 0x10;

float mem_temp[NB_MEM_TEMP] = { 0 };
int mem_phot[NB_MEM_PHOT] = { 0 };
int i_temp = 0;
int i_phot = 0;


void setup()
{
  Serial.begin(9600);
  display.setBrightness(0);

  resetAffichage();

  delay(1000);

  afficheTemperature(0);

  pinMode(THERM_PIN ,INPUT);
}

void loop()
{
  int mesureTemp = analogRead(THERM_PIN);
  int res = mesureToResistance(mesureTemp);
  mem_temp[i_temp] = resistanceToTemperature(res);
  i_temp = (i_temp + 1) % NB_MEM_TEMP;

  mem_phot[i_phot] = analogRead(PHOTO_PIN);
  i_phot = (i_phot + 1) % NB_MEM_PHOT;

  /*Serial.print("\nMesure : ");
  Serial.println(mesure);
  Serial.print("Res : ");
  Serial.println(res);
  Serial.print("Temp : ");
  Serial.println(temp);*/
  
  delay(800);
  
  afficheTemperature(moyenne(mem_temp, NB_MEM_TEMP));

  display.setBrightness(map(moyenne(mem_phot, NB_MEM_PHOT), 0, 1023, 0, 7));
}

float moyenne(float tableau[], int nbElts)
{
  float S = 0;
  for(int j = 0; j < nbElts; j++)
  {
    S += tableau[j];
  }
  return S / nbElts;
}

int moyenne(int tableau[], int nbElts)
{
  int S = 0;
  for(int j = 0; j < nbElts; j++)
  {
    S += tableau[j];
  }
  return S / nbElts;
}

void resetAffichage()
{
  uint8_t data[] = { 0, 0, 0, 0 };
  display.setSegments(data);
}

void afficheTemperature(float temp)
{
  if(temp < 0)
  {
    display.setSegments(MOINS, 1, 0); // -
  }
  else
  {
    display.setSegments(RIEN, 1, 0); // ' '
  }
  int tempArrondie = abs(arrondi(temp));
  display.showNumberDecEx(tempArrondie, BITMASK_DOTS * (tempArrondie > 100), false, 2, 1);
  
  display.setSegments(DEGRES, 1, 3); // °
  
  //display.setSegments(LETTRE_C, 1, 3); // C
}

int mesureToResistance(int mesure)
{
  return (int) ((TENSION_ALIM / (mesure * 0.0048875) - 1) * R - Rr);
}

float resistanceToTemperature(int resistance)
{
  float lnR = log(resistance);
  return 1/(CONSTANT_A + CONSTANT_B * lnR + CONSTANT_C * lnR * lnR * lnR) - CELSIUS_TO_KELVIN;
}

int arrondi(float nombre_float)
{
  int nombre = int(nombre_float);
  if(nombre_float - nombre  >= 0.5)
    nombre++;
  return nombre;
}
