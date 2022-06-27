#include <Adafruit_NeoPixel.h>
#include <ezButton.h>

#define MODE_PIN 8
#define VALUE_PIN 9
#define PIXEL_PIN 6
#define POT_PIN A5
#define MUSIC_PIN A4

#define PIXEL_COUNT 150

#define DC_OFFSET 0
#define SAMPLES 100
#define NOISE 15
#define FADE_DELAY 10

int samples[SAMPLES]; // storage for a sample 
int periodFactor = 0; // For period calculation
int t1 = -1; 
int T; 
int slope; 
byte periodChanged = 0;

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

ezButton modeButton(MODE_PIN);
ezButton valueButton(VALUE_PIN);

uint8_t modeCount = 0;
uint8_t valueCount = 0;

void setup() {
  Serial.begin(9600);
  
  
  
  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(VALUE_PIN, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(255);

  modeButton.setDebounceTime(20);
  modeButton.setCountMode(COUNT_FALLING);

  valueButton.setDebounceTime(20);
  valueButton.setCountMode(COUNT_FALLING);

  modeButton.loop();
  modeButton.resetCount();

  valueButton.loop();
  valueButton.resetCount();

}

void loop(){

  modeButton.loop();
  modeCount = modeButton.getCount();

  switch(modeCount){
    case 0:
      // grayscale
      grayscale();
      break;
    case 1:
      // solid colour
      staticColour();
      break;
    case 2:
      // rainbow
      rainbow();
      break;
    case 3:
      // music reactive
      music();
      break;
     case 4:
       modeButton.resetCount();
       break;
  }
}

void grayscale(){

  while(modeCount == 0){
    
    uint8_t strength = analogRead(POT_PIN) / 4;
    strip.fill(strip.Color(strength, strength, strength));
    strip.show();

    modeButton.loop();
    modeCount = modeButton.getCount();
  }
  
}

void staticColour(){

  valueButton.resetCount();
  
  uint16_t hue;
  uint8_t brightness = 100;

  while(modeCount == 1){
        
      valueButton.loop();
      valueCount = valueButton.getCount();
      
        switch(valueCount){
          case 0:
            hue = analogRead(POT_PIN) * 64;
            break;
          case 1:
            // brightness
            brightness = analogRead(POT_PIN) / 4;
            break;
          case 2:
            // red
            hue = 0;
            break;
          case 3:
            // yellow
            hue = 7700;
            break;
          case 4:
            // green
            hue = 20000;
            break;
          case 5:
            // blue
            hue = 44000;
            break;
          case 6:
            // pink
            hue = 59000;
            break;
          case 7:
            valueButton.resetCount();
            break;
        }
        strip.fill(strip.gamma32(strip.ColorHSV(hue, 255, brightness)));
        strip.show();
  
        modeButton.loop();
        modeCount = modeButton.getCount();
      }
}

void rainbow() {

  valueButton.resetCount();

  uint8_t wait = 10;
  uint8_t brightness = 100;

  while(modeCount == 2){
    
    for(uint16_t hue = 0; hue < 65536; hue += 256) {
      // takes hue, number of repitions, saturation, brightness, gamma correction
      strip.rainbow(hue, 1, 255, brightness, true);
      strip.show();
      
      modeButton.loop();
      modeCount = modeButton.getCount();
  
      if(modeCount != 2){
        break;
      }
      
      valueButton.loop();
      valueCount = valueButton.getCount();
  
      if(valueCount == 1){
        wait = analogRead(POT_PIN) / 4;
      }
      else if(valueCount == 2){
        brightness = analogRead(POT_PIN) / 4;
      }
      else if(valueCount > 2){
        valueButton.resetCount();
      }
      delay(wait);
    }
  }
}

void music(){

  strip.clear();
  strip.show();

   while(modeCount == 3){
    
      for(int i = 0; i < SAMPLES; i++){
        samples[i] = analogRead(MUSIC_PIN);
        if(i>0) {
          slope = samples[i] - samples[i-1];
        }
        else {
          slope = samples[i] - samples[SAMPLES-1];
        }
      
        if(abs(slope) > NOISE) {
          if(slope < 0) {
            calculatePeriod(i);
            if(periodChanged == 1) {
              displayColour(getColour(T));
            }
          }
        }
        else {
          ledsOff();
        }
        periodFactor += 1;
        delay(1);
    }
  }
}

void calculatePeriod(int i) {
  if(t1 == -1) {
  
    t1 = i;
  }
  else {
    
    int period = periodFactor*(i - t1);
    periodChanged = T==period ? 0 : 1;
    T = period;
  // Serial.println(T);
   
    t1 = i;
    periodFactor = 0;
  }
}

uint32_t getColour(int period) {
  if(period == -1)
    return Wheel(0);
  else if(period > 400)
    return Wheel(5);
  else
    return Wheel(map(-1*period, -400, -1, 50, 255));
}

void fadeOut(){
  for(int i = 0; i < 5; i++) {
    strip.setBrightness(110 - i*20);
    strip.show();
    delay(FADE_DELAY);
    periodFactor += FADE_DELAY;
  }
}

void fadeIn() {
  
  for(int i = 0; i < 5; i++) {
    strip.setBrightness(20*i + 30);
    strip.show();
    delay(FADE_DELAY);
    periodFactor += FADE_DELAY;
  }
}

void ledsOff() {  
  
  fadeOut();
  for(int i=0; i<PIXEL_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
}

void displayColour(uint32_t colour) {
  
  for(int i =0 ; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, colour);
  }
  fadeIn();
}

uint32_t Wheel(byte WheelPos) {
  // Serial.println(WheelPos);
  if(WheelPos < 85) {
   
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
