#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

RTC_DS1307 RTC;

#define TIME_SIZE_TEXT 3

unsigned char timeHeures = 0;
unsigned char timeMinutes = 0;

bool etatHeure = true;
unsigned long tempsPourEtat = millis();
bool firstTimeNom = true;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  display.setTextWrap(false);


  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning())
  {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}
 
 
void loop()
{
  if(etatHeure)
  {
    DateTime now = RTC.now(); 
    display.stopscroll();
    affichHeure(TIME_SIZE_TEXT, now.hour(), now.minute());
    
    /*if (timeMinutes == 59)
    {
      timeMinutes = 0;
      if (timeHeures == 23)
        timeHeures = 0;
      else
        timeHeures += 1;
    }
    else
      timeMinutes += 1;*/
  }
  else
  {
    if(firstTimeNom)
    {
      affichNom("Elodie");
      firstTimeNom = false;
    }
  }

  if(millis() - tempsPourEtat > 1000 + 4000 * !etatHeure)
  {
    tempsPourEtat = millis();
    etatHeure = !etatHeure;
    firstTimeNom = true;
  }
  else
  {
    delay(100);
  }
}

void affichHeure(uint8_t textSize, unsigned char heures, unsigned char minutes)
{
  String timeText = "";
  if (heures < 10)
  {
    timeText += " ";
  }
  timeText += String(heures);
  timeText += "h";
  if (minutes < 10)
  {
    timeText += "0";
  }
  timeText += String(minutes);
 
  
  display.clearDisplay();

  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor((display.width() - 5 * textSize * 6 + textSize) / 2, (display.height() - textSize * 7) / 2);
  display.println(timeText);
  
  display.display();
}

void affichNom(String nom)
{
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor((display.width() - 5 * 2 * 6) / 2,(display.height() - 2 * nom.length()) / 2);
  display.print(nom+"  ");
  display.display();
 
  display.startscrollleft(0x00, 0x0F);
}

