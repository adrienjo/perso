#include  "Neopixel_Classe.h"

void setup() {

  Serial.begin(9600);

  LED LEDS[5];

  Adafruit_NeoPixel strip = Adafruit_NeoPixel(NB_LEDS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

  strip.begin();

  for(int led = 0;  led<5;  led++){
/*
    LEDS[led].setColor(RGB2COL(led*255/NB_LEDS,0,255-led*255/NB_LEDS));

    LEDS[led].ON(1);

    LEDS[led].refresh(strip,  led);
*/
    strip.setPixelColor(led, strip.Color(led*255/NB_LEDS,0,led*255/NB_LEDS));

    Serial.println("test");

  }

  strip.show();

}

void loop() {
  // put your main code here, to run repeatedly:

}
