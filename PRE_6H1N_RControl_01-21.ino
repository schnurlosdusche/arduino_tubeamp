// PreAmp 6H1N - Relay Controller Arduino Nano V0.1.21

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>

#define DATA_PIN    6			// Data pin for NeoPixel LED
#define LED_TYPE    WS2812B		// Type of LED
#define COLOR_ORDER GRB			// Color mode
#define NUM_LEDS    1			// only using 1 LED in this project
#define FRAMES_PER_SECOND  120		

CRGB leds[NUM_LEDS];
int BRIGHTNESS = 96;			// change this value for the LED brightness (0-255)
int ORIG_BRIGHTNESS;
int DIM_BRIGHTNESS;

const word ANODE_RELAY_PIN = A2;	// Pins for controlling the relay boards
const word HEATER_RELAY_PIN = A3;	// ..
const word HEATER_PIN = A6;		// Pins for measuring the voltages ( each over a voltage devider )
const word ANODE_PIN = A7;		// ..

int a=0;
int i=0;

float HEATER_MIN_VALUE = 6.0;           // acc to datasheet of tube 6H1N min 6.0V
float NORMAL_HEATER_MAX_VALUE = 6.7;    // acc to datasheet of tube 6H1N max 6.6V
float INITIAL_HEATER_MAX_VALUE = 7.3;	// set to NORMAL_HEATER_MAX_VALUE if your power supply is stabilized to 6.3V.
					// My power supply outputs a voltage around 7V without any load,
					// so very often I get an overvoltage error. With load applied
					// (HEATER relay active), the voltage drops back to the desired range... 
float ANODE_MIN_VALUE = 80.0;		// lower end of anode voltage range (for this particular preamp !)  
float ANODE_MAX_VALUE = 91.5;		// upper end of anode voltage range (for this particular preamp !)
float HEATER_VALUE;			// variable for the measured voltage
float ANODE_VALUE;			// ..
float LOAD_PERCENTAGE;			// percentage value during preheating process (will be shown in display)

char ERROR_MSG_1[21] = "ERR - Heater Voltage";
char ERROR_MSG_2[21] = "ERR - Anode Voltage";

bool HEATER = false;			// false = voltage not in range | true = voltage within range
bool PREHEATING = false;		// preheating ready ? status of preheating
bool ANODE = false;			// false = voltage not in range | true = voltage within range

LiquidCrystal_I2C lcd(0x27,20,4);	// initialize the I2C bus for the 20x4 display. 
					// set the HEX address to your needs (e.g. 0x30)

void setup() 
{
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(Tungsten40W);

  ORIG_BRIGHTNESS = BRIGHTNESS;
  DIM_BRIGHTNESS = BRIGHTNESS;
	
  pinMode(ANODE_RELAY_PIN, OUTPUT);	// set mode of PINS
  pinMode(HEATER_RELAY_PIN, OUTPUT);	// ..
  pinMode(ANODE_PIN, INPUT);		// ..
  pinMode(HEATER_PIN, INPUT);		// ..

  digitalWrite(ANODE_RELAY_PIN, LOW);	// make sure, both relays are in OFF state / connection is open
  digitalWrite(HEATER_RELAY_PIN, LOW);	// ..
	
  FastLED.setBrightness(ORIG_BRIGHTNESS);
  leds[0] = CRGB::Blue;			// set LED color BLUE on start
  FastLED.show();			// ..
  
  lcd.init();				// initialize the display
  lcd.backlight();			// turn on the backlight of the display
    
  lcd.setCursor(6,0); 			// set cursor to position...
  lcd.print("PRE6H1N");			// and show something
  lcd.setCursor(3,2);			// ..
  lcd.print("Version 0.1.22");		// ..
  
  delay(5000);

  lcd.setCursor(0,2); 			// set cursor
  lcd.print("checking voltages...");

  // wait max 30 seconds (60 x 500ms) to let the voltages stabilize. 
  // If that fails, LED flashes red in a deadlock and shows message on display

  // If the voltages got stabilized within the defined range and time, set HEATER and ANODE
  // true (done in "readOutVoltages") to interrupt the "while loop" and continue with the normal operation 
  while (( i < 60 ) && ((HEATER == false) && (ANODE == false))) 
      {
        readOutVoltages();
        delay(500);
  	lcd.setCursor(0,1); 		// set cursor
        lcd.print("waiting for voltages");
        lcd.setCursor(0,2); 		// set cursor
        lcd.print("to stabilize...     ");
        i++;
      }

  // if the counter exceeds 59 (almost half a minute), guess the power supply to be faulty or
  // at least, having a bad day or problem :(
  if ( i > 59 )
      {
        while (i)			// a never ending loop (deadlock)
          {
            lcd.setCursor(0,1); // set cursor to 1st symbol of 1st line
            lcd.print("ERROR - Voltages not");
            lcd.setCursor(0,2); // set cursor to 1st symbol of 2nd line
            lcd.print("in defined range  :(");

	    // start flashing the LED red... forever and ever... 
	    // the preamp must be turned off and on to reset this state
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

  // if at least the HEATER is true (voltage in range), start the preheating process.
  // In this case, it is more important to have a correct HEATER VOLTAGE than ANODE VOLTAGE !
  if (HEATER == true)
  {
      preheating();
  }

}

// main loop. this is for the normal operation
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

  // If the preamp has an unstabilized power supply for the HEATER,
  // a higher voltage during unloaded condition might occur, that prevents the
  // process to start the preheating (as it thinks, the HEATER voltage is too 
  // high). But when load is applied by the HEATER coils, the voltage drops
  // back to normal range. For this case, we use an additional variable with
  // a slightly higher MAX VOLTAGE.
	
  if ( PREHEATING == false )
  	{ HEATER_MAX_VALUE = INITIAL_HEATER_MAX_VALUE; }
  else
  	{ HEATER_MAX_VALUE = NORMAL_HEATER_MAX_VALUE; } 	
	
  HEATER_VALUE = analogRead(HEATER_PIN); // Probe Input
  HEATER_VALUE = HEATER_VALUE * (5.0 / 1023); HEATER_VALUE = (HEATER_VALUE * 1.525);
  ANODE_VALUE = analogRead(ANODE_PIN); // Probe Input
  ANODE_VALUE = ANODE_VALUE * (5.0 / 1023); ANODE_VALUE = (ANODE_VALUE * 21.438);
 
  if (( HEATER_VALUE >= HEATER_MIN_VALUE )&&( HEATER_VALUE <= HEATER_MAX_VALUE )) { HEATER = true; }  else { HEATER = false; }
  if (( ANODE_VALUE >= ANODE_MIN_VALUE )&&( ANODE_VALUE <= ANODE_MAX_VALUE )) { ANODE = true; } else { ANODE = false; }

  lcd.setCursor(2,3);
  lcd.print("H:");
  lcd.print(HEATER_VALUE);      
  lcd.print("V   A:");
  lcd.print(ANODE_VALUE);
  lcd.print("V");
}

void preheating()
{
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(2,2);
    lcd.print("preheating tubes  ");
    
    digitalWrite(HEATER_RELAY_PIN, HIGH);       // switch on relay for heater
    ORIG_BRIGHTNESS = BRIGHTNESS;		

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

      // the factor 4.6 is needed to calculate the shown percentage of the preheating
      // process
      LOAD_PERCENTAGE = ( a * 4.6 );
      lcd.setCursor(17,2);
      lcd.print(LOAD_PERCENTAGE,0); lcd.print("%");
    
      readOutVoltages;
  }

  #define BRIGHTNESS ORIG_BRIGHTNESS		// after dimming during preheat, set original brightness

    lcd.setCursor(0,2);
    lcd.print("preheating finished ");
      delay(2000);
    lcd.setCursor(0,2);
    lcd.print("                           ");

    PREHEATING = true;
}
