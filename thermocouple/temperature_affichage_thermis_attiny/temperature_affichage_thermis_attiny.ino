#include <TM1637Display.h>
#include <avr/pgmspace.h>
#include "table2.h"


// Module connection pins (Digital Pins)
#define TM1637_CLK_PIN 1
#define TM1637_DIO_PIN 2

#define THERM_PIN A2
#define PHOTO_PIN A3

#define UNDER_TABLE -32768
#define OVER_TABLE  32767

#define TENSION_ALIM  5
#define R             9880
#define Rr            1200

#define NB_MEM_TEMP        5
#define NB_MEM_PHOT        5


int mem_temp[NB_MEM_TEMP] = { 0 }; // 100*temp
int mem_phot[NB_MEM_PHOT] = { 0 };
byte i_temp = 0;
byte i_phot = 0;


TM1637Display display(TM1637_CLK_PIN, TM1637_DIO_PIN);
const uint8_t DEGRES[] = { SEG_A | SEG_B | SEG_F | SEG_G };
const uint8_t MOINS[] = { SEG_G };
const uint8_t RIEN[] = { 0 };
const uint8_t BITMASK_DOTS = 0b01000000;


void setup()
{
  /*
  TM1637_init();
  
  TM1637_set_brightness(5);

  TM1637_clear();

  TM1637_enable(false);
  */

  display.setBrightness(5);
  
  afficheTemperature(0, 1);

  pinMode(THERM_PIN, INPUT);
  pinMode(PHOTO_PIN, INPUT);
}

void loop()
{
  int mesure = analogRead(THERM_PIN);
  int temp = mesureToTemperature(mesure);
  mem_temp[i_temp] = temp;
  i_temp = (i_temp + 1) % NB_MEM_TEMP;

  mem_phot[i_phot] = analogRead(PHOTO_PIN);
  i_phot = (i_phot + 1) % NB_MEM_PHOT;
  
  delay(800);
  
  afficheTemperature(moyenne(mem_temp, NB_MEM_TEMP), (temp == OVER_TABLE) || (temp == UNDER_TABLE));

  display.setBrightness(moyenne(mem_phot, NB_MEM_PHOT) / 146);
}

int moyenne(int tableau[], byte nbElts)
{
  int S = 0;
  for(byte j = 0; j < nbElts; j++)
  {
    S += tableau[j];
  }
  return int(S / nbElts);
}

void afficheTemperature(int temp, byte error)
{
  if(temp < 0)
  {
    //TM1637_display_segments(0x00, 0b01000000); // -
    display.setSegments(MOINS, 1, 0);
  }
  else
  {
     //TM1637_display_segments(0x00, 0); // ' '
     display.setSegments(RIEN, 1, 0);
  }
  char tempArrondie = char(abs(arrondi(temp)) / 100);
  /*TM1637_display_digit(0x01, temp - 10 * (temp / 10));
  TM1637_display_digit(0x02, temp / 10);
  
  TM1637_display_segments(0x03, 0b011000011); // °*/


  display.showNumberDecEx(tempArrondie, BITMASK_DOTS * error, false, 2, 1);
  
  display.setSegments(DEGRES, 1, 3); // °
}

int mesureToTemperature(int mesure)
{
  if(mesure < 68)
    return UNDER_TABLE;
  else if(mesure <= 888)
  {
    return pgm_read_word_near(table + ((mesure - 68) / 4));
  }
  else
    return OVER_TABLE;
}

int arrondi(int nombre_100)
{
  int nombre = int(nombre_100 / 100) * 100;
  if(nombre_100 - nombre  >= 50)
    nombre += 100;
  return nombre;
}

