#include <Adafruit_TFTLCD.h> 
#include <Adafruit_GFX.h>    
#include <TouchScreen.h>
#include <Wire.h>

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define TS_MINX 137
#define TS_MINY 106
#define TS_MAXX 952
#define TS_MAXY 922

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364);

int x = 7;

void setup() {

  Serial.begin(9600);

  //screen is master so no need for an address
  Wire.begin();
  
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.drawRect(0,0,319,240,MAGENTA);
  
  tft.fillRect(0,0, 319, 40, RED);
  tft.drawRect(0,0, 319, 40,BLACK);
  tft.setCursor(158, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("ON");
  
  
}

bool powerPress = false;
bool nextPress = false;
bool prevPress = false;

bool buttonPress = false;

String stations[3] = {"87.8", "89.5", "95.4"};
int currentStation = 0;

void loop() {
  
  TSPoint p = ts.getPoint();
  
  if (p.z > ts.pressureThreshhold) 
  {
    
   p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
   p.y = map(p.y, TS_MAXY, TS_MINY, 0, 480);

   Serial.print("\nX = "); Serial.print(p.x);
   Serial.print("\tY = "); Serial.print(p.y);

   //next press
   if(p.x>16 && p.x<60 && p.y>20 && p.y<225)
   {
      buttonPress = true;
      
      currentStation++;

      if(currentStation>2)
        currentStation = 0;

      x=2;
      Wire.beginTransmission(9); // transmit to device #9
      Wire.write(x);             // sends x 
      Wire.endTransmission();    // stop transmitting
    
   }


   //prev press
   else if(p.x>13 && p.x<60 && p.y>288 && p.y<492)
   {
      buttonPress = true;
      
      currentStation--;

      if(currentStation<0)
        currentStation = 2;

      x=1;
      Wire.beginTransmission(9); // transmit to device #9
      Wire.write(x);             // sends x 
      Wire.endTransmission();    // stop transmitting
   }
   //power button press
   if(p.x>280 && p.x<327 && p.y>0 && p.y<480)
   {
      buttonPress = true;
      
      powerPress = !powerPress;

      x=0;
      Wire.beginTransmission(9); // transmit to device #9
      Wire.write(x);             // sends x 
      Wire.endTransmission();    // stop transmitting
      Serial.println("Done Transmitting");
   }

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if(powerPress && buttonPress)
  {
      buttonPress = false;
      
      tft.fillScreen(BLACK);
      tft.drawRect(0,0,319,240,CYAN);
  
      tft.fillRect(0,0, 319, 40, RED);
      tft.drawRect(0,0, 319, 40,BLACK);
      tft.setCursor(158, 10);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.print("OFF");

      tft.fillRect(180,200, 139, 40, BLUE);
      tft.drawRect(180,200, 139, 40,BLACK);
      tft.setCursor(225, 210);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.print("NEXT");

      tft.fillRect(0,200, 139, 40, BLUE);
      tft.drawRect(0,200, 139, 40,BLACK);
      tft.setCursor(52, 210);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.print("PREV");


      tft.setCursor(120, 100);
      tft.setTextColor(WHITE);
      tft.setTextSize(4);
      tft.print(stations[currentStation]);

      
    }
    else if(!powerPress && buttonPress)
    {
      buttonPress = false;
      
      tft.fillScreen(BLACK);
      tft.drawRect(0,0,319,240,MAGENTA);
      
      tft.fillRect(0,0, 319, 40, RED);
      tft.drawRect(0,0, 319, 40,BLACK);
      tft.setCursor(158, 10);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.print("ON");
    }
  }
}
