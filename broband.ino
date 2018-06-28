#include <Adafruit_GFX.h>
#include <gfxfont.h>

// Adafruit_NeoMatrix example for single NeoPixel Shield.
// By Marc MERLIN <marc_soft@merlins.org>
// Contains code (c) Adafruit, license BSD

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "LowPower.h"
#include "digitalbutton.h"
//#include "presets.h"

// Choose your prefered pixmap
//#include "heart24.h"
#include "heart8.h"
#include "pacman.h"
#include "tetris.h"
//#include "yellowsmiley24.h"
//#include "bluesmiley24.h"
//#include "smileytongue24.h"

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define BUTTON_PIN 2
#define BUTTON_PIN2 3
#define PIN 6
#define MAX_PRESETS 2
#define EEPROM_PRESET_LOCATION 0
#define BUTTON_OFF_TIME 1100
#define FPS 8
#define LED_POWER_PIN 5
#define MIC_POWER_PIN 11
#define audioSensitivity 20

DigitalButton btn = buttonCreate(BUTTON_PIN);

DigitalButton btn2 = buttonCreate(BUTTON_PIN2);

uint8_t animationFrame = 0; //actual frame rendered in animation

    uint8_t pickPalette = 0;//palette pointer for audio junk

uint64_t lastMillis = 0;  // Start of sample window
uint64_t startAnimationMillis = 0;
uint16_t tickRate = 125;
// wake from sleep flag
volatile bool justWoke = false;
uint8_t currentPresetIndex = 0;
uint8_t menu = 0;

void setJustWoke(){
    justWoke = true;
}


void onWake(){
    
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
    ledsOn();
}

void onButtonTap(){
   // nextPreset();

   menu = 1 - menu;
    //EEPROM.write(EEPROM_PRESET_LOCATION, currentPresetIndex);
   // DEBUGF("set preset %i\n", currentPresetIndex);
}

void onButtonTap2(){
   // nextPreset();
   if(pickPalette == 3)
   {
    pickPalette = 0;
   }
   else
   {
    pickPalette += 1;
   }
   //pickPalette = 4 - pickPalette;
    //EEPROM.write(EEPROM_PRESET_LOCATION, currentPresetIndex);
   // DEBUGF("set preset %i\n", currentPresetIndex);
}


void onButtonHold(){
    ledsOff();
    //DEBUG("sleepin");
    delay(500);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), setJustWoke, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

/*
int presetCount;
Preset * currentPreset;
Preset * presets[MAX_PRESETS];*/

/*
int pushPreset(Preset * preset){
    if(presetCount >= MAX_PRESETS){
        DEBUG("Cant push another preset, already at max presets");
        return 0;
    }
    presets[presetCount-1] = preset;
    presetCount++;
    return 1;
}

void nextPreset(){
    currentPresetIndex = (currentPresetIndex + 1) % (presetCount - 1);
    currentPreset = presets[currentPresetIndex];
}
*/

//later optimize so that mic is only on when audio thing is
void ledsOn(){
    digitalWrite(LED_POWER_PIN, HIGH);
    digitalWrite(MIC_POWER_PIN, HIGH);
}
void ledsOff(){
    digitalWrite(LED_POWER_PIN, LOW);
    digitalWrite(MIC_POWER_PIN, LOW);
}





// Max is 255, 32 is a conservative value to not overload
// a USB power supply (500mA) for 12x12 pixels.
#define BRIGHTNESS 32

// Define matrix width and height.
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 pixels)
//   NEO_GRB     Pixels are wired for GRB bitstream (v2 pixels)
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA v1 pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, PIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// This could also be defined as matrix->color(255,0,0) but those defines
// are meant to work for adafruit_gfx backends that are lacking color()
#define LED_BLACK		0

#define LED_RED_VERYLOW 	(3 <<  11)
#define LED_RED_LOW 		(7 <<  11)
#define LED_RED_MEDIUM 		(15 << 11)
#define LED_RED_HIGH 		(31 << 11)

#define LED_GREEN_VERYLOW	(1 <<  5)   
#define LED_GREEN_LOW 		(15 << 5)  
#define LED_GREEN_MEDIUM 	(31 << 5)  
#define LED_GREEN_HIGH 		(63 << 5)  

#define LED_BLUE_VERYLOW	3
#define LED_BLUE_LOW 		7
#define LED_BLUE_MEDIUM 	15
#define LED_BLUE_HIGH 		31

#define LED_ORANGE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW		(LED_RED_LOW     + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW	(LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW		(LED_RED_LOW     + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM	(LED_RED_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH		(LED_RED_HIGH    + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW	(LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW		(LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM		(LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH		(LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW		(LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)





// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    uint16_t RGB_bmp_fixed[w * h];
    for (uint16_t pixel=0; pixel<w*h; pixel++) {
	uint8_t r,g,b;
	uint16_t color = pgm_read_word(bitmap + pixel);

	//Serial.println(color);
	b = (color & 0xF00) >> 8;
	g = (color & 0x0F0) >> 4;
	r = color & 0x00F;
	//Serial.print(" ");
	//Serial.print(b);
	//Serial.print("/");
	//Serial.print(g);
	//Serial.print("/");
	//Serial.print(r);
	//Serial.print(" -> ");
	// expand from 4/4/4 bits per color to 5/6/5
	b = map(b, 0, 15, 0, 31);
	g = map(g, 0, 15, 0, 63);
	r = map(r, 0, 15, 0, 31);
	//Serial.print(r);
	//Serial.print("/");
	//Serial.print(g);
	//Serial.print("/");
	//Serial.print(b);
	RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
	//Serial.print(" -> ");
	//Serial.println(RGB_bmp_fixed[pixel], HEX);
    }
    matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}


            
void piskelDrawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h, uint8_t frame) {
    uint16_t RGB_bmp_fixed[w * h];
    //So because progmem is weird I'm gonna use the animation offset to
    //render through frames because progmem is bestmem (aka it's not)
    uint32_t animationOffset = (w * h) * frame;
    
   // for (uint16_t pixel=0; pixel<w*h; pixel++) {
   for (uint16_t pixel=0; pixel<w*h; pixel++) {
      uint32_t r,g,b;
      uint8_t red, green, blue;
      uint32_t color = pgm_read_dword(bitmap+pixel+animationOffset);
      //uint32_t color = pgm_read_dword(bitmap24+pixel);
      // uint32_t color = pgm_read_dword(bitmap24+pixel);

      //uint32_t ReadData = pgm_read_dword(&hello_01_data[ index ] );

     /*Serial.println(color,HEX);
      Serial.print(" ");*/
     //strips the raw values for r g and b from piskel
     blue = (color & 0xFF0000) >> 16;
     green = (color & 0xFF00) >> 8;
     red = (color & 0xFF);
     
     b =  (blue  >> 3)        & 0x001F;
     g = ((green >> 2) <<  5) & 0x07E0; // not <
     r = ((red   >> 3) << 11) & 0xF800; // not <
   
/*
           r = (r * 249 + 1014) >> 11;
            g = (g * 253 + 505) >> 10;
            b = (b * 249 + 1014) >> 11;
              Serial.print(" ");*/
  /*
  r = map(r, 0, 255, 0, 31);
  g = map(g, 0, 255, 0, 63);
  b = map(b, 0, 255, 0, 31);
//long RGB = ((long)R << 16L) | ((long)G << 8L) | (long)B;
*/
/*
  Serial.print(" ");
  Serial.print(red);
  Serial.print("/");
  Serial.print(green);
  Serial.print("/");
  Serial.print(blue);
  RGB_bmp_fixed[pixel] = r+g+b;
  Serial.print(" -> ");
  Serial.println(r + g + b, HEX);*/

  //save new bmp pixel
 //Gotta reverse it otherwise it's mirrored and upside down
  RGB_bmp_fixed[((h*w) - 1) -pixel] = r+g+b;
 // Serial.println(RGB_bmp_fixed[pixel], HEX);
    }

    matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}

void audioDraw() {
   matrix->clear();
    uint8_t w = 8, h = 8;
    uint16_t y_draw = 0; //final y led height value after processing
    uint16_t sampleWindow = 30; // Sample window width in mS (50 mS = 20Hz)
    uint16_t sample;
    //where we draw that mug
    uint16_t audio_bmp[w * h];

    uint64_t startMillis= millis();  // Start of sample window
    uint16_t peakToPeak = 0;   // peak-to-peak level

    uint16_t signalMax = 0;
    uint16_t signalMin = 1024;

    //dead nigger storage top bottom middle`
    const uint32_t colorPalettes[4+1][3+1] = {
    {0xF800, 0x17E0, 0xF7E0},
    {0xEDB1, 0xDA05, 0xD344},
    {0xF800, 0xD344, 0x17E0},
    {0x07E0, 0x00FF, 0xD81F}};

   // pickPalette = random(0, 2);

  
   
   //code i didn't write
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0); 
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;



   // map 1v p-p level to the max scale of the display
   uint16_t displayPeak = map(peakToPeak, 0, audioSensitivity, 0, h);
   uint16_t displayPeak_constrain = constrain(displayPeak, 0, 9);
   
     Serial.println(displayPeak);
     

 for(uint16_t x_pixel = 0; x_pixel < w; x_pixel++){
    //y_draw = random(y_pixel-3, y_pixel+3); full screen scramble
      y_draw = random(0, displayPeak_constrain);
   for (uint16_t y_pixel=0; y_pixel<y_draw; y_pixel++) {

      
     
      //red
      matrix->drawPixel(x_pixel, y_pixel,colorPalettes[pickPalette][0]);
      //Green for the first 1/3d pix the math may not always work out
      //for larger or smaller displays
      if(y_pixel <= (h*.33))
      matrix->drawPixel(x_pixel, y_pixel,colorPalettes[pickPalette][1]);
      //Yellow the next 1/3d pix the math may not always work out
      //for larger or smaller displays
      if(y_pixel >= (h * .33) && y_pixel < (h*.66))
      matrix->drawPixel(x_pixel, y_pixel,colorPalettes[pickPalette][2]);    
      
      
      }
 }
    
  
     matrix->show();
}

// Fill the screen with multiple levels of white to gauge the quality
void display_four_white() {
    matrix->clear();
    matrix->fillRect(0,0, MATRIX_WIDTH,MATRIX_HEIGHT, LED_WHITE_HIGH);
    matrix->drawRect(1,1, MATRIX_WIDTH-2,MATRIX_HEIGHT-2, LED_WHITE_MEDIUM);
    matrix->drawRect(2,2, MATRIX_WIDTH-4,MATRIX_HEIGHT-4, LED_WHITE_LOW);
    matrix->drawRect(3,3, MATRIX_WIDTH-6,MATRIX_HEIGHT-6, LED_WHITE_VERYLOW);
    matrix->show();
}



void display_rgbBitmap(uint8_t bmp_num) { 
  /*static uint16_t bmx,bmy;

    fixdrawRGBBitmap(bmx, bmy, RGB_bmp[bmp_num], 8, 8);
    bmx += 8;
    if (bmx >= MATRIX_WIDTH) bmx = 0;
    if (!bmx) bmy += 8;
    if (bmy >= MATRIX_HEIGHT) bmy = 0;
    matrix->show();*/
}

void display_lines() {
    matrix->clear();

    // 4 levels of crossing red lines.
    matrix->drawLine(0,MATRIX_HEIGHT/2-2, MATRIX_WIDTH-1,2, LED_RED_VERYLOW);
    matrix->drawLine(0,MATRIX_HEIGHT/2-1, MATRIX_WIDTH-1,3, LED_RED_LOW);
    matrix->drawLine(0,MATRIX_HEIGHT/2,   MATRIX_WIDTH-1,MATRIX_HEIGHT/2, LED_RED_MEDIUM);
    matrix->drawLine(0,MATRIX_HEIGHT/2+1, MATRIX_WIDTH-1,MATRIX_HEIGHT/2+1, LED_RED_HIGH);

    // 4 levels of crossing green lines.
    matrix->drawLine(MATRIX_WIDTH/2-2, 0, MATRIX_WIDTH/2-2, MATRIX_HEIGHT-1, LED_GREEN_VERYLOW);
    matrix->drawLine(MATRIX_WIDTH/2-1, 0, MATRIX_WIDTH/2-1, MATRIX_HEIGHT-1, LED_GREEN_LOW);
    matrix->drawLine(MATRIX_WIDTH/2+0, 0, MATRIX_WIDTH/2+0, MATRIX_HEIGHT-1, LED_GREEN_MEDIUM);
    matrix->drawLine(MATRIX_WIDTH/2+1, 0, MATRIX_WIDTH/2+1, MATRIX_HEIGHT-1, LED_GREEN_HIGH);

    // Diagonal blue line.
    matrix->drawLine(0,0, MATRIX_WIDTH-1,MATRIX_HEIGHT-1, LED_BLUE_HIGH);
    matrix->drawLine(0,MATRIX_HEIGHT-1, MATRIX_WIDTH-1,0, LED_ORANGE_MEDIUM);
    matrix->show();
}

void display_boxes() {
    matrix->clear();
    matrix->drawRect(0,0, MATRIX_WIDTH,MATRIX_HEIGHT, LED_BLUE_HIGH);
    matrix->drawRect(1,1, MATRIX_WIDTH-2,MATRIX_HEIGHT-2, LED_GREEN_MEDIUM);
    matrix->fillRect(2,2, MATRIX_WIDTH-4,MATRIX_HEIGHT-4, LED_RED_HIGH);
    matrix->fillRect(3,3, MATRIX_WIDTH-6,MATRIX_HEIGHT-6, LED_ORANGE_MEDIUM);
    matrix->show();
}

void display_circles() {
    matrix->clear();
    matrix->drawCircle(MATRIX_WIDTH/2,MATRIX_HEIGHT/2, 2, LED_RED_MEDIUM);
    matrix->drawCircle(MATRIX_WIDTH/2-1-min(MATRIX_WIDTH,MATRIX_HEIGHT)/8, MATRIX_HEIGHT/2-1-min(MATRIX_WIDTH,MATRIX_HEIGHT)/8, min(MATRIX_WIDTH,MATRIX_HEIGHT)/4, LED_BLUE_HIGH);
    matrix->drawCircle(MATRIX_WIDTH/2+1+min(MATRIX_WIDTH,MATRIX_HEIGHT)/8, MATRIX_HEIGHT/2+1+min(MATRIX_WIDTH,MATRIX_HEIGHT)/8, min(MATRIX_WIDTH,MATRIX_HEIGHT)/4-1, LED_ORANGE_MEDIUM);
    matrix->drawCircle(1,MATRIX_HEIGHT-2, 1, LED_GREEN_LOW);
    matrix->drawCircle(MATRIX_WIDTH-2,1, 1, LED_GREEN_HIGH);
    if (min(MATRIX_WIDTH,MATRIX_HEIGHT)>12) matrix->drawCircle(MATRIX_WIDTH/2-1, MATRIX_HEIGHT/2-1, min(MATRIX_HEIGHT/2-1,MATRIX_WIDTH/2-1), LED_CYAN_HIGH);
    matrix->show();
}


void display_scrollText() {
    matrix->clear();
    matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
    matrix->setTextSize(1);
    matrix->setRotation(0);
    for (int8_t x=7; x>=-42; x--) {
	matrix->clear();
	matrix->setCursor(x,0);
	matrix->setTextColor(LED_GREEN_HIGH);
	matrix->print("Hello");
	if (MATRIX_HEIGHT>11) {
	    matrix->setCursor(-20-x,MATRIX_HEIGHT-7);
	    matrix->setTextColor(LED_ORANGE_HIGH);
	    matrix->print("World");
	}
	matrix->show();
       delay(50);
    }

    matrix->setRotation(3);
    matrix->setTextColor(LED_BLUE_HIGH);
    for (int8_t x=7; x>=-45; x--) {
	matrix->clear();
	matrix->setCursor(x,MATRIX_WIDTH/2-4);
	matrix->print("Rotate");
	matrix->show();
       delay(50);
    }
    matrix->setRotation(0);
    matrix->setCursor(0,0);
    matrix->show();
}

// Scroll within big bitmap so that all if it becomes visible or bounce a small one.
// If the bitmap is bigger in one dimension and smaller in the other one, it will
// be both panned and bounced in the appropriate dimensions.
void display_panOrBounceBitmap (uint8_t bitmapSize) {
    // keep integer math, deal with values 16 times too big
    // start by showing upper left of big bitmap or centering if the display is big
    int16_t xf = max(0, (MATRIX_WIDTH-bitmapSize)/2) << 4;
    int16_t yf = max(0, (MATRIX_HEIGHT-bitmapSize)/2) << 4;
    // scroll speed in 1/16th
    int16_t xfc = 3;
    int16_t yfc = 2;
    // scroll down and right by moving upper left corner off screen 
    // more up and left (which means negative numbers)
    int16_t xfdir = -1;
    int16_t yfdir = -1;

    for (uint16_t i=1; i<1000; i++) {
	    bool updDir = false;

	    // Get actual x/y by dividing by 16.
	    int16_t x = xf >> 4;
	    int16_t y = yf >> 4;

	matrix->clear();
	// bounce 8x8 tri color smiley face around the screen
//	if (bitmapSize == 8) fixdrawRGBBitmap(x, y, RGB_bmp[10], 8, 8);
	// pan 24x24 pixmap
	//matrix->drawRGBBitmap(x, y, (const uint16_t *) bitmap24, bitmapSize, bitmapSize);
  //if (bitmapSize == 24)  piskelDrawRGBBitmap(0, 0, heart8, 8, 8);
	matrix->show();
	 
	// Only pan if the display size is smaller than the pixmap
	// but not if the difference is too small or it'll look bad.
	if (bitmapSize-MATRIX_WIDTH>2) {
	  
	     xf += xfc*xfdir;
	    
	    if (xf >= 0)                      
	    { 
	      xfdir = -1; 
	      updDir = true ; 
	    }
	    // we don't go negative past right corner, go back positive
	    if (xf <= ((MATRIX_WIDTH-bitmapSize) << 4)) { xfdir = 1;  updDir = true ; };
	}
	if (bitmapSize-MATRIX_HEIGHT>2) {
	    yf += yfc*yfdir;
	    // we shouldn't display past left corner, reverse direction.
	    if (yf >= 0)                      
	    { 
	      yfdir = -1; 
	      updDir = true ; 
	    }
	    if (yf <= ((MATRIX_HEIGHT-bitmapSize) << 4)) 
	    {
	      yfdir = 1;  
	      updDir = true ; 
	    }
	}

/*	// only bounce a pixmap if it's smaller than the display size
	if (MATRIX_WIDTH>bitmapSize) {
	    xf += xfc*xfdir;
	    // Deal with bouncing off the 'walls'
	    if (xf >= (MATRIX_WIDTH-bitmapSize) << 4) { xfdir = -1; updDir = true ; };
	    if (xf <= 0)           { xfdir =  1; updDir = true ; };
	}
	if (MATRIX_HEIGHT>bitmapSize) {
	    yf += yfc*yfdir;
	    if (yf >= (MATRIX_HEIGHT-bitmapSize) << 4) { yfdir = -1; updDir = true ; };
	    if (yf <= 0)           { yfdir =  1; updDir = true ; };
	}*/

  //random accelleration
  
	/*if (updDir) {
	    // Add -1, 0 or 1 but bind result to 1 to 1.
	    // Let's take 3 is a minimum speed, otherwise it's too slow.
	    xfc = constrain(xfc + random(-1, 2), 3, 16);
	    yfc = constrain(xfc + random(-1, 2), 3, 16);
	}*/
	delay(10);
    }
}


/* Animations only work with 2 dimensional matrix objects using piskel as the
 * output.
 * x start x position
 * y start y position
 * bitmapAnimation is the array from PROGMEM
 * height of thing
 * width of thing
 * how many frames to animate
 */
void display_animation (const uint32_t *bitmapAnimation , uint8_t height, uint8_t width, uint8_t numFrames) {
  int x =0, y=0;

  /*Basically take the stated animation dimensions,
  / then compares them to the actual dimensions and
  / does the ole subtraction.  This places the animation
  / in the corner --not needed now
  */
 /* x = MATRIX_WIDTH - width;
  y = MATRIX_HEIGHT - height;*/

   
   //Do animations at a certain FPS
   if (millis() - startAnimationMillis > tickRate)
   {
      //Serial.println(tickRate);

     
      //advance frame
      piskelDrawRGBBitmap(x, y, (const uint32_t *)bitmapAnimation, height, width, animationFrame);
      matrix->show();

      //If the animation reaches its end, then restart animation
      //or say you change the animation mid way, it will actually still
      //finish the current animation as long as the frames don't exceede
      //the max number of frames of the new animation.  Set to 0 otherwise.
      //Also it's numFrames - 1 because number of frames is 1 larger than
      //the array size since arrays start at 0
      if(animationFrame >= (numFrames-1))
      {
        animationFrame = 0;
      }
      else
      {
        animationFrame = animationFrame + 1;
      }
      
      
   
      //basically reset the trigger for the frame of animation
      startAnimationMillis = millis();
   }

}

void loop() {
   

    // if we just woke up, do wake up stuff
    if(justWoke){
        onWake();
        justWoke = false;
    }

    
 
     switch (menu) {
      case 2:
        audioDraw();
        break;
      case 0:
        //display_animation((const uint32_t *) pacman, 8, 8, 2);
        audioDraw();
        break;
      case 1:
        display_animation((const uint32_t *) tetris, 8, 8, 55);
        break;
      
     
        }

    digitalButtonTick(btn);
    digitalButtonTick(btn2);
}

   
    
void setup() {
    Serial.begin(115200);
    pinMode(LED_POWER_PIN, OUTPUT); 
    pinMode(MIC_POWER_PIN, OUTPUT); 
    matrix->begin();

    matrix->setTextWrap(false);
    matrix->setBrightness(BRIGHTNESS);
    currentPresetIndex = EEPROM.read(EEPROM_PRESET_LOCATION);
     
    
    // if EEPROM gave us garbage
    if(currentPresetIndex > MAX_PRESETS || currentPresetIndex < 0){
        currentPresetIndex = 0;
    }

    buttonOnTap(btn, onButtonTap);
    buttonOnHold(btn, onButtonHold, BUTTON_OFF_TIME);
    buttonOnTap(btn2, onButtonTap2);

    
    
    
    matrix->clear();
    ledsOn();
}

