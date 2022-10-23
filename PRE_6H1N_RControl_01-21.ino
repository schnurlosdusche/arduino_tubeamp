// PreAmp 6H1N - Relay Controller Arduino Nano V0.1.21

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>

#define DATA_PIN    6
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    1
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];
int BRIGHTNESS = 96;
int ORIG_BRIGHTNESS;
int DIM_BRIGHTNESS;

const word ANODE_RELAY_PIN = A2;
const word HEATER_RELAY_PIN = A3;
const word HEATER_PIN = A6;
const word ANODE_PIN = A7;

int a=0;
int i=0;

float HEATER_MIN_VALUE = 6.0;             // acc to datasheet 6H1N min 6.0V
float HEATER_MAX_VALUE = 6.9;             // acc to datasheet 6H1N max 6.6V (the powersupply output is without load of tubes greater than 6.6V !)
float ANODE_MIN_VALUE = 80.0;
float ANODE_MAX_VALUE = 91.5;
float HEATER_VALUE;
float ANODE_VALUE;
float LOAD_PERCENTAGE;

char ERROR_MSG_1[21] = "ERR - Heater Voltage";
char ERROR_MSG_2[21] = "ERR - Anode Voltage";

bool HEATER = false;
bool PREHEATING = false;
bool ANODE = false;
bool RELAY_HEATER = false;
bool RELAY_ANODE = false;

LiquidCrystal_I2C lcd(0x27,20,4);

void setup() 
{
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(Tungsten40W);

  ORIG_BRIGHTNESS = BRIGHTNESS;
  DIM_BRIGHTNESS = BRIGHTNESS;
  
	pinMode(ANODE_RELAY_PIN, OUTPUT);
	pinMode(HEATER_RELAY_PIN, OUTPUT);
  pinMode(ANODE_PIN, INPUT);
	pinMode(HEATER_PIN, INPUT);

  FastLED.setBrightness(ORIG_BRIGHTNESS);
  leds[0] = CRGB::Blue;
  FastLED.show();

	digitalWrite(ANODE_RELAY_PIN, LOW);
  RELAY_ANODE = false;
	digitalWrite(HEATER_RELAY_PIN, LOW);		
  RELAY_HEATER = false;

  lcd.init();
  lcd.backlight();
    
  lcd.setCursor(6,0); // set cursor to 1 symbol of 1 line
  lcd.print("PRE6H1N");
  
  //-----------------------------------------------------------------------
  lcd.setCursor(3,2);
  lcd.print("Version 0.1.21");
  // ----------------------------------------------------------------------

  delay(3000);

  lcd.setCursor(0,2); // set cursor to 1 symbol of 1 line
  lcd.print("checking voltages...");

  delay(1000);

    while (( i < 60 ) && ((HEATER == false) && (ANODE == false))) 
      {
        readOutVoltages();
        delay(500);
  	    lcd.setCursor(0,1); // set cursor to 1 symbol of 1 line
        lcd.print("waiting for voltages");
        lcd.setCursor(0,2); // set cursor to 1 symbol of 1 line
        lcd.print("to stabilize...     ");
        i++;
      }

    if ( i > 58 )
      {
        while (i)
          {
            lcd.setCursor(0,1); // set cursor to 1 symbol of 1 line
            lcd.print("ERROR - Voltages not");
            lcd.setCursor(0,2); // set cursor to 1 symbol of 1 line
            lcd.print("in defined range  :(");

            FastLED.setBrightness(128);
            leds[0] = CRGB::Red;
            FastLED.show();
            delay(150);
            FastLED.setBrightness(0);
            leds[0] = CRGB::Black;
            FastLED.show();
            delay(850);
          }
      }

  if (HEATER == true)
  {
      preheating();
  }

}

void loop() 
	{
  readOutVoltages();

	if ((ANODE == true)&&(HEATER == true))
		{ 
      if (PREHEATING == true)	 
        {
          FastLED.setBrightness(48);
          leds[0] = CRGB::Green;
          FastLED.show();
        
          digitalWrite(ANODE_RELAY_PIN, HIGH); 
          RELAY_ANODE = true;
          digitalWrite(HEATER_RELAY_PIN, HIGH);
          RELAY_HEATER = true;
          delay(500); 
        }
      else
        {
          if (HEATER == true)
          {
            preheating();
          }
            
        }
    }
     else
		{ 
        FastLED.setBrightness(BRIGHTNESS);
        leds[0] = CRGB::Red;
        FastLED.show();
        
        digitalWrite(ANODE_RELAY_PIN, LOW); 
        RELAY_ANODE = false;
        digitalWrite(HEATER_RELAY_PIN, LOW);
        RELAY_HEATER = false;
        
        delay(500); 

      lcd.setCursor(0,2); 
      lcd.print("                    ");
      lcd.setCursor(0,3); 
      lcd.print("                    ");
      
      if ( HEATER == false ) 
      {
        lcd.setCursor(0,1); 
        lcd.print(ERROR_MSG_1); 
      }
      if ( ANODE == false ) 
      { 
        lcd.setCursor(0,2); 
        lcd.print(ERROR_MSG_2);
      }
           
      while ( 1 != 0)                 // dead lock with red blinking LED
      {
        FastLED.setBrightness(255);
        leds[0] = CRGB::Red;
        FastLED.show();
        delay(150);
        FastLED.setBrightness(0);
        leds[0] = CRGB::Black;
        FastLED.show();
        delay(850);
      }
    }
	
	delay(100);
	}
  
void readOutVoltages()
{
  HEATER_VALUE = analogRead(HEATER_PIN); // Probe Input
  HEATER_VALUE = HEATER_VALUE * (5.0 / 1023); HEATER_VALUE = (HEATER_VALUE * 1.525);
  ANODE_VALUE = analogRead(ANODE_PIN); // Probe Input
  ANODE_VALUE = ANODE_VALUE * (5.0 / 1023); ANODE_VALUE = (ANODE_VALUE * 21.438);
 
  if (( HEATER_VALUE >= HEATER_MIN_VALUE )&&( HEATER_VALUE <= HEATER_MAX_VALUE )) { HEATER = true; }  else { HEATER = false; }
  if (( ANODE_VALUE >= ANODE_MIN_VALUE )&&( ANODE_VALUE <= ANODE_MAX_VALUE )) { ANODE = true; } else { ANODE = false; }

  lcd.setCursor(2,3);
  if ( RELAY_HEATER == true ) 
    { lcd.print("H:"); } else { lcd.print("h:"); }
  lcd.print(HEATER_VALUE);      
  if ( RELAY_ANODE == true ) 
    { lcd.print("V  A:"); } else { lcd.print("V  a:"); }
  lcd.print(ANODE_VALUE);
  lcd.print("V");
}

void preheating()
{
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("preheating tubes    ");
    
    digitalWrite(HEATER_RELAY_PIN, HIGH);               // switch on relay for heater
    ORIG_BRIGHTNESS = BRIGHTNESS;

    RELAY_HEATER = true;
    readOutVoltages;

  for ( a = 0; a <= 21; a ++)
  {
    while ( DIM_BRIGHTNESS > 10)
      {
        FastLED.setBrightness(DIM_BRIGHTNESS);
        leds[0] = CRGB::Orange;
        FastLED.show();
        DIM_BRIGHTNESS--;
        delay(15);
      }

    while ( DIM_BRIGHTNESS <= ORIG_BRIGHTNESS )
      {
        FastLED.setBrightness(DIM_BRIGHTNESS);
        leds[0] = CRGB::Orange;
        FastLED.show();
        DIM_BRIGHTNESS++;
        delay(15);
      }

      LOAD_PERCENTAGE = ( a * 4.6 );
      lcd.setCursor(17,2);
      lcd.print(LOAD_PERCENTAGE,0); lcd.print("%");
    
      readOutVoltages;
  }

  #define BRIGHTNESS ORIG_BRIGHTNESS

    lcd.setCursor(0,2);
    lcd.print("preheating finished ");
      delay(2000);
    lcd.setCursor(0,2);
    lcd.print("                           ");

    PREHEATING = true;
}
