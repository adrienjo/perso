#include  "Monoblink_Receiver.h"

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

void  LED::refresh(Adafruit_NeoPixel* strip,int  num_led){

  //Serial.print("LED : "); Serial.println(num_led); 
  //Serial.print("R : "); Serial.println(this->color.R); 
  //Serial.print("G : "); Serial.println(this->color.G); 
  //Serial.print("B : "); Serial.println(this->color.B); 

  if(this->on)  strip->setPixelColor(num_led, strip->Color(this->color.R,this->color.G,this->color.B));
  else          strip->setPixelColor(num_led, strip->Color(0,0,0));
}

///////////////////////////////////////////////

bool  read_message(byte* mssg,unsigned char mssg_length){

        if(message[0]=='R') pixels.setPixelColor(0, pixels.Color(255,0,0));
        if(message[0]=='G') pixels.setPixelColor(0, pixels.Color(0,255,0));
        if(message[0]=='B') pixels.setPixelColor(0, pixels.Color(0,0,255));
        
        return true;
}
