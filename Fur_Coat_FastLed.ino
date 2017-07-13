#include "FastLED.h"

#define PINL 7
#define PINR 9
#define PINM 8

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

int max_pix;
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
  max_pix = max(NUMPIXELS1,NUMPIXELS2);
  max_pix = max(max_pix,NUMPIXELS3);
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
  Serial.println(state);
  if (state == 0) {
    Serial.println("startSnake");
    startSnake();  
  } else if (state == 1) {
    Serial.println("startUpWave");
    startUpWave(0);
  } else if (state == 2) {
    Serial.println("startGradiant");
    //startUpWave(5);
    startGradiant(1,50,1);
  }
  FastLED.show();
  FastLED.delay(5);

}
/////////////////////////////////////////////////////////////////
void startGradiant(int color_jump, int delayval, bool one_direction) {
/////////////////////////////////////////////////////////////////
  int color_i =0;
  //int power_i = 0;
  int color_add =color_jump;
  //int power_add =power_jump;
  while (1) {  
    if (state != 2) {    
      return;
    }

    //    Serial.println(max_pix);
    color_i = (color_i +color_add) %255;  
    if (!one_direction && color_i >= max_pix) {
        color_add = -color_jump;
    } else if (!one_direction && color_i <= 0) {
        color_add = color_jump;
    }   
    gradiant(color_i);
    FastLED.show();
    FastLED.delay(delayval); // Delay for a period of time (in milliseconds).


    //Serial.println(color_i);
  }
}

/////////////////////////////////////////////////////////////////
// gradiant:
//   color_i - color changing from the index base color (for color movement)
//   strip - 0 - both strips; 1 - strip 1; 2 - strip 2
//   line  - 0 - all the lines ; 1 - line 1; 2 - line 2; 3- line 3 (ecery strip has 3 lines: up-down-up
/////////////////////////////////////////////////////////////////
void gradiant(int color_i) { 
/////////////////////////////////////////////////////////////////  
  for(int i=0;i<max_pix;i++){  
    // FIRST LINE
    if (i<NUMPIXELS1) {
      setLed(ledsOutlineL,i,i+color_i);
      setLed(ledsOutlineM,i,i+color_i);
      setLed(ledsOutlineR,i,i+color_i);
    }
 
    // SECOND LINE
    int index= NUMPIXELS1+NUMPIXELS2-1-i;
    if (index>=NUMPIXELS1 && index <= NUMPIXELS1+NUMPIXELS2) {
      setLed(ledsOutlineL,index,i+color_i);
      setLed(ledsOutlineM,index,i+color_i);
      setLed(ledsOutlineR,index,i+color_i);    
    }
 

    // THIRD LINE       
     index= NUMPIXELS1+NUMPIXELS2+i;
     setLed(ledsOutlineL,index,i+color_i);
     setLed(ledsOutlineM,index,i+color_i);
     setLed(ledsOutlineR,index,i+color_i);       
  } 
}
/////////////////////////////////////////////////////////////////
void setLed(CRGB * ledsOutline, int i,int color_num) { 
/////////////////////////////////////////////////////////////////
  color_num = (11*color_num)%255;
  //Serial.print("color_num ");
  //Serial.println(color_num);
  //Serial.print("i ");
  //Serial.println(i);  
  ledsOutline[i] = CHSV(color_num, 255, 255);
}
/////////////////////////////////////////////////////////////////
void startSnake() { 
/////////////////////////////////////////////////////////////////
  outlineWave();  
}

/////////////////////////////////////////////////////////////////
void startUpWave(int add_delay) { 
/////////////////////////////////////////////////////////////////
  unsigned long now = millis();
  outlineWaveStrip(now,0,NUMPIXELS1,ledsOutlineL);  
  now += add_delay;
  outlineWaveStrip(now,NUMPIXELS1,NUMPIXELS2,ledsOutlineL);
  now += add_delay;
  outlineWaveStrip(now,NUMPIXELS1+NUMPIXELS2,NUMPIXELS3,ledsOutlineL);
  
  now += add_delay;
  outlineWaveStrip(now,0,NUMPIXELS1,ledsOutlineR);
  now += add_delay;
  outlineWaveStrip(now,NUMPIXELS1,NUMPIXELS2,ledsOutlineR);
  now += add_delay;
  outlineWaveStrip(now,NUMPIXELS1+NUMPIXELS2,NUMPIXELS3,ledsOutlineR);

  now += add_delay;
  outlineWaveStrip(now,0,NUMPIXELS_M,ledsOutlineM);  
}

/////////////////////////////////////////////////////////////////
void OnInterupt() {
/////////////////////////////////////////////////////////////////
  cli();
  state = (state+1)%3;  
  sei();  
}

