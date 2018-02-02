#include  "Monoblink_Receiver.h"

unsigned  char  cpt;
bool  up;

LED LEDS[NB_LEDS];

          byte message[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming messages
unsigned  char messageLength = VW_MAX_MESSAGE_LEN; // the size of the message

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NB_LEDS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

void setup() {

  strip.begin();

  for(int led = 0;  led<NB_LEDS;  led++){
    
    LEDS[led].setColor(RGB2COL(100,100,100));
  
    LEDS[led].ON(1);
  
    LEDS[led].refresh(&strip,  led);

  }

  strip.show();

}

void loop() {

 if (vw_get_message(message, &messageLength))
  {
      if(read_message(message,messageLength)) strip.show();
  }
}
