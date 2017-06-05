#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// shootingStar:
#define STAR_LENGTH 5 // how many pixels the star will be
#define STAR_DELAY_TIME 10

#define PINL 7
#define PINR 9
#define PINM 8

#define PinInt 3 //interrupt pin - attach the button here

// How the leds destributed in each side of the vest
#define NUMPIXELS       17
#define NUMPIXELS2      21
#define NUMPIXELS3      21

#define NUMPIXELS_M     27
#define POWER 45 // the base power of the leds

#define NUM_OF_COLORS  12

#define UNCONNECTED_ANALOG_PIN 0// for random seed
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(59, PINL, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(59, PINR, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(NUMPIXELS_M, PINM, NEO_GRB + NEO_KHZ800);



// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
volatile bool state = 0;
int max_pix;
/////////////////////////////////////////////////////////////////
void setup() {
/////////////////////////////////////////////////////////////////  
   Serial.begin(9600);  
   pixels.begin(); // This initializes the NeoPixel library.
   pixels2.begin(); // This initializes the NeoPixel library.
   pixels3.begin(); // This initializes the NeoPixel library.
   max_pix = max(NUMPIXELS,NUMPIXELS2);
   max_pix = max(max_pix,NUMPIXELS3);
   randomSeed(analogRead(UNCONNECTED_ANALOG_PIN)); 
   pinMode(PinInt, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(PinInt), OnInterupt, FALLING); 
}

/////////////////////////////////////////////////////////////////
void loop() {
/////////////////////////////////////////////////////////////////
//  Serial.print("interrupt_flag ");
//  Serial.println(interrupt_flag);
    
//  Serial.print("state ");
//  Serial.println(state);
  if (state == 0) {
    startGradiant(1,0,100,1); // gradiant without powered led startGradiant(int color_jump, int power_jump,int delayval,bool one_direction)
  } else {
    //startWings(60,500,17); //startWings(int delay_time, int time_left, int num_of_colors)
    shootingStar();
  }
}

/////////////////////////////////////////////////////////////////
void shootingStar() {
/////////////////////////////////////////////////////////////////
  while (1) {
    if (state != 1) {
      return;    
    }
    long line = random(7);
    long color = random(NUM_OF_COLORS);
    shootStar(line,color);
  }
  
}

/////////////////////////////////////////////////////////////////
void shootStar(long line, long color) {
/////////////////////////////////////////////////////////////////
   int start_pixel,end_pixel,strip;
   switch (line) {
    // set the start and end pixels that will participate
    case 0:
      start_pixel = 0;
      end_pixel = NUMPIXELS-1;
      strip = 0;
      break;
    case 1:
      start_pixel = NUMPIXELS;
      end_pixel = NUMPIXELS+NUMPIXELS2-1;
      strip = 0;
      break;
    case 2:
      start_pixel = NUMPIXELS+NUMPIXELS2;
      end_pixel = start_pixel+NUMPIXELS3;
      strip = 0;
      break;
    case 3:
      start_pixel = 0;
      end_pixel = NUMPIXELS-1;
      strip = 1;
      break;
    case 4:
      start_pixel = NUMPIXELS;
      end_pixel = NUMPIXELS+NUMPIXELS2-1;
      strip = 1;
      break;
    case 5:
      start_pixel = NUMPIXELS+NUMPIXELS2;
      end_pixel = start_pixel+NUMPIXELS3;
      strip = 1;
      break;   
    case 6:
      start_pixel = 0;
      end_pixel = NUMPIXELS_M;
      strip = 3;
      break;   
   }
   lightStar(start_pixel,end_pixel,strip,color);
}

/////////////////////////////////////////////////////////////////
void lightStar(int start_pixel, int end_pixel, int strip, int color) {
/////////////////////////////////////////////////////////////////
  for (int i = start_pixel ; i <= end_pixel; ++i) {
    addPixel(i,getColor(color,POWER),strip);
    if (i - STAR_LENGTH >= start_pixel) {
      addPixel(i-STAR_LENGTH,0,strip);
    }
    showPixels();
    delay(STAR_DELAY_TIME);
  }
  int power = POWER;
  for (int i = end_pixel - STAR_LENGTH; i <= end_pixel; ++i) {
    addPixel(i,0,strip);
    for (int j = i+1 ; j <= end_pixel; ++j) {
      addPixel(j,getColor(color,power/2),strip);
    }
    showPixels();
    delay(STAR_DELAY_TIME);
  }
}

/////////////////////////////////////////////////////////////////
void OnInterupt() {
/////////////////////////////////////////////////////////////////
  cli();
  state = !state;  
  sei();  
}

/////////////////////////////////////////////////////////////////
void clearAll() {
/////////////////////////////////////////////////////////////////  
  for (int i=0; i<NUMPIXELS+NUMPIXELS2+NUMPIXELS3; i++) {
    addPixel(i,pixels.Color(0,0,0),0);
  }
  
}

/////////////////////////////////////////////////////////////////
void startWings(int delay_time, int time_left, int num_of_colors) {
/////////////////////////////////////////////////////////////////  
   int j=0;
   clearAll();   
   while (time_left > 0) {   
    if (state != 1) {    
      return;
    }  
    setWingLine(getColor(j,POWER),NUMPIXELS,0);
    setWingLine(getColor(j+1,POWER),NUMPIXELS2,NUMPIXELS);
    setWingLine(getColor(j+2,POWER),NUMPIXELS3,NUMPIXELS+NUMPIXELS2);
    delay(delay_time);
    time_left -= delay_time;
    showPixels();
    j++;
  }
    
}

/////////////////////////////////////////////////////////////////
void setWingLine(uint32_t color, int num_of_pixels, int start_pixel) {
/////////////////////////////////////////////////////////////////  
    //Serial.println(start_pixel);
    //Serial.println(num_of_pixels);
    for (int i = start_pixel ; i<start_pixel+num_of_pixels; i++) {
      //Serial.println(i);
      addPixel(i,color ,1); //FIRST STRIP
      addPixel(NUMPIXELS+NUMPIXELS2+NUMPIXELS3-i,color ,2); //SECOND STRIP
    }
    addPixel(NUMPIXELS_M/2+start_pixel,color ,3); //MIDDLE STRIP
    addPixel(NUMPIXELS_M/2-start_pixel,color ,3); //MIDDLE STRIP
}


/////////////////////////////////////////////////////////////////
// startGradiant
// color_jump - each iteration the color will move this amount of pixels
// power_jump - each iteration the led with more power will move this amount of pixels
// delayval - the delay between to iterations
// one_direction - go only up or up-down
/////////////////////////////////////////////////////////////////
void startGradiant(int color_jump, int power_jump,int delayval, bool one_direction) {
/////////////////////////////////////////////////////////////////
  int color_i =0;
  int power_i = 0;
  int color_add =color_jump;
  int power_add =power_jump;
  while (1) {  
    if (state != 0) {    
      return;
    }
//    Serial.println(power_i);
    //    Serial.println(max_pix);
    color_i = color_i +color_add;
    power_i = power_i + power_add;
    if (!one_direction && color_i >= max_pix) {
        color_add = -color_jump;
    } else if (!one_direction && color_i <= 0) {
        color_add = color_jump;
    }
    if (power_i > max_pix) {
      power_add = -power_jump;
    } else if (power_i <= 0) {
      power_add = power_jump;
    }
    gradiant(color_i,power_i,0,0);
    
    delay(delayval); // Delay for a period of time (in milliseconds).
//    Serial.print(power_i);
//    Serial.print(" ");
//    Serial.println(color_i);
  }
}
/////////////////////////////////////////////////////////////////
// gradiant:
//   color_i - color changing from the index base color (for color movement)
//   power_i - the led index to be whith more power
//   strip - 0 - both strips; 1 - strip 1; 2 - strip 2
//   line  - 0 - all the lines ; 1 - line 1; 2 - line 2; 3- line 3 (ecery strip has 3 lines: up-down-up
/////////////////////////////////////////////////////////////////
void gradiant(int color_i,int power_i,int strip,int line) { 
/////////////////////////////////////////////////////////////////  
 for(int i=0;i<max_pix;i++){
  //  int power = sin(PI*(float)((i%WAVE_WIDTH)+1)/(WAVE_WIDTH+1))*50;
    int power = POWER;
 //   Serial.println(i);
    int power_tmp;
    if (i==power_i+1 || i==power_i-1) {
      power_tmp=power+40;
   //   Serial.println(i);
    } else if (i==power_i) {
      power_tmp=power+80;      
    } else {
      power_tmp=power;
    }

    // FIRST LINE
    if (i<NUMPIXELS && line == 1 || line == 0) {
      addPixel(i, getColor(i+color_i,power_tmp),strip);
    }
 
    // SECOND LINE
    int index= NUMPIXELS+NUMPIXELS2-1-i;
    if (index>=NUMPIXELS && line == 2 || line == 0) {
    addPixel(index, getColor(i+color_i,power_tmp),strip);
    }
 

    // THIRD LINE   
    if (line == 3 || line == 0) {
      index= NUMPIXELS+NUMPIXELS2+i;
      addPixel(index, getColor(i+color_i,power_tmp),strip);
    } 
  }
  showPixels();   

}

/////////////////////////////////////////////////////////////////
uint32_t getColor(int i, int power) {
/////////////////////////////////////////////////////////////////
  i = i%NUM_OF_COLORS;  
  if (i<NUM_OF_COLORS/3) {
       int p = i*3*power/NUM_OF_COLORS;
//         Serial.print(p);         
//         Serial.println(power);                               
       return pixels.Color(power-p,p,0);
  } else if (i <2*NUM_OF_COLORS/3) {
      // int p = i*3*power/(2*NUM_OF_COLORS);
       int p = (i-NUM_OF_COLORS/3)*3*power/NUM_OF_COLORS;
       //Serial.println(power);
       //Serial.println(p);
       return pixels.Color(0,power-p,p);
  } else {
       //int p = i*power/NUM_OF_COLORS;
       int p = (i-2*NUM_OF_COLORS/3)*3*power/NUM_OF_COLORS;
       return pixels.Color(p,0,power-p); 
  }
}

/////////////////////////////////////////////////////////////////
void showPixels() {
/////////////////////////////////////////////////////////////////  
   pixels.show(); // This sends the updated pixel color to the hardware.
   pixels2.show(); // This sends the updated pixel color to the hardware.
   pixels3.show(); // This sends the updated pixel color to the hardware.
}

/////////////////////////////////////////////////////////////////
void addPixel(int i, uint32_t color,int strip) {
/////////////////////////////////////////////////////////////////  
   switch (strip) {
     case 0:
          pixels.setPixelColor(i, color); 
          pixels2.setPixelColor(i, color); 
          pixels3.setPixelColor(i, color); 
          break;
     case 1:
          pixels.setPixelColor(i, color); 
          break;
     case 2:
          pixels2.setPixelColor(i, color); 
          break;      
     case 3:
          pixels3.setPixelColor(i, color); 
          break;      
   }
}
