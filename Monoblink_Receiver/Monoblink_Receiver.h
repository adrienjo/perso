#ifndef NEOPIXEL_CLASSE_H
#define NEOPIXEL_CLASSE_H

#include  <Arduino.h>
#include  <Adafruit_NeoPixel.h>

#define   PIN_STRIP   3
#define   NB_LEDS     1

typedef struct{
  unsigned  char  R;
  unsigned  char  G;
  unsigned  char  B;
}RGB;

RGB   RGB2COL(byte,byte,byte);

class LED{

  public  :

          LED();
    bool  isOn();
    void  ON(bool);
    void  setColor(RGB);
    void  refresh(Adafruit_NeoPixel*,int);  

  private :

    bool  on;
    RGB   color;
};

class VIBREUR{

  public  :

          VIBREUR();
    bool  isOn();
    void  ON(bool);
    void  setRPM(byte);

  private :

    bool  on;
    byte  rpm;
};

class PRESS{

  public  :

          PRESS();
    byte  isOn();
    byte  getVal();
    void  setVal(byte);

  private :

    byte  val;
};

#endif
