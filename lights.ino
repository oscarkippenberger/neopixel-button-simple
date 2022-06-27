#include <Adafruit_NeoPixel.h>
#include <ezButton.h>

#define MODE_PIN 8
#define VALUE_PIN 9
#define PIXEL_PIN 6
#define POT_PIN A5
#define PIXEL_COUNT 150

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
  strip.show();
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
  
}
