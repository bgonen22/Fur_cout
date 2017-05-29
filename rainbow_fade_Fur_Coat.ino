#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PINL 7
#define PINR 9
#define PINM 8

#define PinInt 3 //interrupt pin - attach the button here

// How the leds destributed in each side of the vest
#define NUMPIXELS       17
#define NUMPIXELS2      21
#define NUMPIXELS3      21

#define NUMPIXELS_M     27
#define POWER 20 // the base power of the leds

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
volatile int interrupt_flag=0;
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
bool state = 0;
/////////////////////////////////////////////////////////////////
void loop() {
/////////////////////////////////////////////////////////////////
//  Serial.print("interrupt_flag ");
//  Serial.println(interrupt_flag);
  if (interrupt_flag) {    
    state = !state;
    cli();
    interrupt_flag = 0;
    sei();
  }
  
//  Serial.print("state ");
//  Serial.println(state);
  if (state == 0) {
    startGradiant(1,0,20,200); // gradiant without powered led startGradiant(int color_jump, int power_jump,int delayval, int num_of_cycles)
  } else {
    startWings(60,500,17); //startWings(int delay_time, int time_left, int num_of_colors)
  }
}
/////////////////////////////////////////////////////////////////
void OnInterupt() {
/////////////////////////////////////////////////////////////////
  cli();
  interrupt_flag = 1;
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
    if (interrupt_flag) {    
      return;
    }  
    setWingLine(getColor(j,POWER,num_of_colors),NUMPIXELS,0);
    setWingLine(getColor(j+1,POWER,num_of_colors),NUMPIXELS2,NUMPIXELS);
    setWingLine(getColor(j+2,POWER,num_of_colors),NUMPIXELS3,NUMPIXELS+NUMPIXELS2);
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
// num_of_cycel - number of cycles that the function will run
/////////////////////////////////////////////////////////////////
void startGradiant(int color_jump, int power_jump,int delayval, int num_of_cycles) {
/////////////////////////////////////////////////////////////////
  int color_i =0;
  int power_i = 0;
  int color_add =color_jump;
  int power_add =power_jump;
  for (int a =1 ; a<2*max_pix*num_of_cycles;a++) { // number of cycles it will run 
    if (interrupt_flag) {    
      return;
    }
//    Serial.println(power_i);
    //    Serial.println(max_pix);
    color_i = color_i +color_add;
    power_i = power_i + power_add;
    if (color_i >= max_pix) {
      //  color_add = -color_jump;
    } else if (color_i <= 0) {
      //  color_add = color_jump;
    }
    if (power_i > max_pix) {
      power_add = -power_jump;
    } else if (power_i <= 0) {
      power_add = power_jump;
    }
    gradiant(color_i,power_i,0,0);
    int delay_val;
    if (a%max_pix >= max_pix-5) {
      delay_val = 1.5*delayval;
    } else {
      delay_val = delayval;
    }
    delay(delay_val); // Delay for a period of time (in milliseconds).
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
//   line  - 0 - all the lines ; 1 - line 1; 2 - line 2; 3- line 3 
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
      addPixel(i, getColor(i+color_i,power_tmp,max_pix),strip);
    }
 
    // SECOND LINE
    int index= NUMPIXELS+NUMPIXELS2-1-i;
    if (index>=NUMPIXELS && line == 2 || line == 0) {
    addPixel(index, getColor(i+color_i,power_tmp,max_pix),strip);
    }
 

    // THIRD LINE   
    if (line == 3 || line == 0) {
      index= NUMPIXELS+NUMPIXELS2+i;
      addPixel(index, getColor(i+color_i,power_tmp,max_pix),strip);
    } 
  }
  showPixels();   

}

/////////////////////////////////////////////////////////////////
uint32_t getColor(int i, int power, int max_pix) {
/////////////////////////////////////////////////////////////////
  i = i%max_pix;
  if (i<max_pix/3) {
       int p = i*3*power/max_pix;
//         Serial.print(p);         
//         Serial.println(power);                               
       return pixels.Color(power-p,p,0);
  } else if (i <2*max_pix/3) {
      // int p = i*3*power/(2*max_pix);
       int p = (i-max_pix/3)*3*power/max_pix;
       //Serial.println(power);
       //Serial.println(p);
       return pixels.Color(0,power-p,p);
  } else {
       //int p = i*power/max_pix;
       int p = (i-2*max_pix/3)*3*power/max_pix;
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
