#include  "Monoblink_Receiver.h"

unsigned  char  cpt;
bool  up;

LED LEDS[NB_LEDS];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NB_LEDS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

void setup() {

  strip.begin();

  LEDS[led].setColor(RGB2COL(100,100,100));

  LEDS[led].ON(1);

  LEDS[led].refresh(&strip,  led);

  strip.show();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(cpt==0)    up = 1;
  if(cpt==255)  up = 0;
  
  if(up)  cpt++;
  else    cpt--;

  for(int led = 0;  led<NB_LEDS;  led++){

    LEDS[led].setColor(RGB2COL(cpt,0,255-cpt));

    LEDS[led].refresh(&strip,  led);

  }

  strip.show();

  delay(5);

}
