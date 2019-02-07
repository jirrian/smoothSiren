#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { THEATER_CHASE, COLOR_WIPE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel {
  public:

    // Member Variables:
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position

    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern

    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
      OnComplete = callback;
    }
    // Update the pattern
    void Update()
    {
      if ((millis() - lastUpdate) > Interval) // time to update
      {
        lastUpdate = millis();
        switch (ActivePattern)
        {
          case THEATER_CHASE:
            TheaterChaseUpdate();
            break;
          case COLOR_WIPE:
            ColorWipeUpdate();
            break;
        }
      }
    }
    // Increment the Index and reset at the end
    void Increment() {
      if (Direction == FORWARD)
      {
        Index++;
        if (Index >= TotalSteps)
        {
          Index = 0;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
      else // Direction == REVERSE
      {
        --Index;
        if (Index <= 0)
        {
          Index = TotalSteps - 1;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
    }
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = THEATER_CHASE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        if ((i + Index) % 3 == 0)
        {
          setPixelColor(i, Color1);
        }
        else
        {
          setPixelColor(i, Color2);
        }
      }
      show();
      Increment();
    }
    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD) {
      ActivePattern = COLOR_WIPE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color;
      Index = 0;
      Direction = dir;
    }

    // Update the Color Wipe Pattern
    void ColorWipeUpdate() {
      setPixelColor(Index, Color1);
      
      
      if(Index < 4){
         setPixelColor(16+(Index-4), Color(0,0,0));
         setPixelColor(16+(Index-3), Color(0,0,0));
         setPixelColor(16+(Index-2), Color(0,0,0));
         setPixelColor(16+(Index-1), Color(0,0,0));
      }
      else{
         setPixelColor(Index-4, Color(0,0,0));
         setPixelColor(Index-3, Color(30,0,0));
         setPixelColor(Index-2, Color(60,0,0));
         setPixelColor(Index-1, Color(90,0,0));
         
      }
      show();
      Increment();
    }
};

#ifdef __AVR__
#include <avr/power.h>
#endif

#define ctsPin 11
#define PIN 5

int sensorReading;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
void done();

NeoPatterns Strip(24, 5, NEO_GRB, NULL);

void setup() {
  Serial.begin(9600);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  Strip.begin();
  Strip.setBrightness(255);

  pinMode(ctsPin, INPUT);

  Strip.TheaterChase(Strip.Color(127, 0, 0), Strip.Color(0, 0, 0), 50); // Red

}

void loop() {
  Strip.Update();
  
  sensorReading = digitalRead(ctsPin);
  Serial.println(sensorReading);
  if (sensorReading == 1) {
    // Some example procedures showing how to display to the pixels:
    Strip.ActivePattern = COLOR_WIPE;
    Strip.Interval = 100;

  } else {
    Strip.ActivePattern = THEATER_CHASE;
    Strip.Interval = 50;
  }

  //  sensorReading = digitalRead(ctsPin);
  // 
  // checkSensor();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//  WheelPos = 255 - WheelPos;
//  if (WheelPos < 85) {
//    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  }
//  if (WheelPos < 170) {
//    WheelPos -= 85;
//    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//  WheelPos -= 170;
//  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//}
