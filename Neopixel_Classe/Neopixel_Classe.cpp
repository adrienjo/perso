#include  "Neopixel_Classe.h"

RGB   RGB2COL(byte  Red,byte  Green,byte  Blue){

  RGB col;
  col.R = Red;
  col.G = Green;
  col.B = Blue;

  return  col;
}

///////////////////////////////

LED::LED(){
  
}

bool  LED::isOn(){

  return  this->on;
}

void  LED::ON(bool  ON){
  
  this->on  = ON;
}

void  LED::setColor(RGB  col){
  
  this->color = col; 
}

void  LED::refresh(Adafruit_NeoPixel strip,int  num_led){

  Serial.print("LED : "); Serial.println(num_led); 
  Serial.print("R : "); Serial.println(this->color.R); 
  Serial.print("G : "); Serial.println(this->color.G); 
  Serial.print("B : "); Serial.println(this->color.B); 

  if(this->on)  strip.setPixelColor(num_led, strip.Color(this->color.R,this->color.G,this->color.B));
  else          strip.setPixelColor(num_led, strip.Color(0,0,0));
}


