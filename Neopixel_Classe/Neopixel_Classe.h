#ifndef NEOPIXEL_CLASSE_H
#define NEOPIXEL_CLASSE_H

#include  <Arduino.h>
#include  <Adafruit_NeoPixel.h>

struct{
  unsigned  char  R;
  unsigned  char  G;
  unsigned  char  B;
}RGB

class LED{

  public  :

          LED();
    bool  isOn();
    void  setColor();    

  private :

    bool  on;
    RGB   color;
};

#endif
