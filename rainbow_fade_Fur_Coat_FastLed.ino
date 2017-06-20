#include "FastLED.h"

#define PINL 13
#define PINR 12
#define PINM 11

// How the leds destributed in each side of the vest
#define NUMPIXELS1       17
#define NUMPIXELS2      21
#define NUMPIXELS3      21

#define NUMPIXELS_M     27

#define PinInt 3 //interrupt pin - attach the button here

#define NUMOFPIXELSIDE  NUMPIXELS1+NUMPIXELS2+NUMPIXELS3
// Define the array of leds
CRGB ledsOutlineL[NUMOFPIXELSIDE];
CRGB ledsOutlineR[NUMOFPIXELSIDE];
CRGB ledsOutlineM[NUMPIXELS_M];

volatile int state = 0;
/////////////////////////////////////////////////////////////////
void setup() { 
/////////////////////////////////////////////////////////////////  
  Serial.begin(9600);
  Serial.println("resetting");
  LEDS.addLeds<NEOPIXEL,PINL>(ledsOutlineL,NUMOFPIXELSIDE);
  LEDS.addLeds<NEOPIXEL,PINR>(ledsOutlineR,NUMOFPIXELSIDE);
  LEDS.addLeds<NEOPIXEL,PINM>(ledsOutlineM,NUMPIXELS_M);
  LEDS.setBrightness(54);
  pinMode(PinInt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinInt), OnInterupt, FALLING); 
}

/////////////////////////////////////////////////////////////////
void outlineWave() {  
/////////////////////////////////////////////////////////////////  
  unsigned long now = millis();
  outlineWaveStrip(now,0,NUMOFPIXELSIDE,ledsOutlineL);
  outlineWaveStrip(now,0,NUMOFPIXELSIDE,ledsOutlineR);
  outlineWaveStrip(now,0,NUMPIXELS_M,ledsOutlineM);
  
}
/////////////////////////////////////////////////////////////////
void outlineWaveStrip(unsigned long now,int start_pixel, int numOfPixels,CRGB * ledsOutline) {
/////////////////////////////////////////////////////////////////  
  uint8_t offset = 0;
  for(int i = start_pixel; i < start_pixel+numOfPixels; i++) {
    // note about math: 1 cycle of a wave is in the range 0-255
    // so millis / divider will take 256*divider milliseconds for a complete cycle
    //uint8_t wave = quadwave8(millis()/13+i*11);
    offset += scale8(quadwave8(add8(now/117, i)), 37);
    offset += scale8(quadwave8(add8(now/29, i)), 23);
    uint8_t wave = quadwave8(offset);

    // Set the i'th led to red 
    ledsOutline[i] = CHSV(add8(i,now/27), 255, qsub8(wave,20));
  }
}
/////////////////////////////////////////////////////////////////
void loop() { 
/////////////////////////////////////////////////////////////////
  //Serial.println(state);
  if (state == 0) {
  //  Serial.println("startSnake");
    startSnake();  
  } else if (state == 1) {
//    Serial.println("startUpWave");
    startUpWave();
  } else if (state == 2) {
    //Serial.println("startUpWaveAsync");
    startUpWave();
  }
  FastLED.show();
  FastLED.delay(5);

}

/////////////////////////////////////////////////////////////////
void startSnake() { 
/////////////////////////////////////////////////////////////////
  outlineWave();  
}

/////////////////////////////////////////////////////////////////
void startUpWave() { 
/////////////////////////////////////////////////////////////////
  unsigned long now = millis();
  outlineWaveStrip(now,0,NUMPIXELS1,ledsOutlineL);
  outlineWaveStrip(now,NUMPIXELS1,NUMPIXELS2,ledsOutlineL);
  outlineWaveStrip(now,NUMPIXELS1+NUMPIXELS2,NUMPIXELS3,ledsOutlineL);
  
  outlineWaveStrip(now,0,NUMPIXELS1,ledsOutlineR);
  outlineWaveStrip(now,NUMPIXELS1,NUMPIXELS2,ledsOutlineR);
  outlineWaveStrip(now,NUMPIXELS1+NUMPIXELS2,NUMPIXELS3,ledsOutlineR);
    
  outlineWaveStrip(now,0,NUMPIXELS_M,ledsOutlineM);  
}

/////////////////////////////////////////////////////////////////
void startUpWaveAsync() { 
/////////////////////////////////////////////////////////////////
  outlineWaveStrip(millis(),0,NUMPIXELS1,ledsOutlineL);
  outlineWaveStrip(millis(),NUMPIXELS1,NUMPIXELS2,ledsOutlineL);
  outlineWaveStrip(millis(),NUMPIXELS1+NUMPIXELS2,NUMPIXELS3,ledsOutlineL);
  
  outlineWaveStrip(millis(),0,NUMPIXELS1,ledsOutlineR);
  outlineWaveStrip(millis(),NUMPIXELS1,NUMPIXELS2,ledsOutlineR);
  outlineWaveStrip(millis(),NUMPIXELS1+NUMPIXELS2,NUMPIXELS3,ledsOutlineR);
    
  outlineWaveStrip(millis(),0,NUMPIXELS_M,ledsOutlineM);  
}

/////////////////////////////////////////////////////////////////
void OnInterupt() {
/////////////////////////////////////////////////////////////////
  cli();
  state = (state+1)%3;  
  sei();  
}

