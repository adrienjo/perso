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

void  LED::refresh(Adafruit_NeoPixel* strip,int  num_led){

  //Serial.print("LED : "); Serial.println(num_led); 
  //Serial.print("R : "); Serial.println(this->color.R); 
  //Serial.print("G : "); Serial.println(this->color.G); 
  //Serial.print("B : "); Serial.println(this->color.B); 

  if(this->on)  strip->setPixelColor(num_led, strip->Color(this->color.R,this->color.G,this->color.B));
  else          strip->setPixelColor(num_led, strip->Color(0,0,0));
}

///////////////////////////////////////////////////////////////////

void  set_Timer0(){
  
  //Timer déclenche interruption à F_PWM*256
  //IT sur match + CTC

  TCCR0A  = 0b00000010; //WGM mode 4 (CTC)
  TCCR0B  = 0b00000001; //no prescaler

  OCR0A   = 100;

  TIMSK   = 0b00010000;

  DDRB    |=  0b100;

  PORTB   |=  0b100;

  sei();
}

ISR(TIMER0_COMPA_vect){
  
  PORTB ^=  0b100;
}

VIBREUR::VIBREUR(){

  set_Timer0();
  this->on  = 0;
  this->rpm = 0;
}

bool  VIBREUR::isOn(){

  return  this->on;
}
 
void  VIBREUR::ON(bool ON){

  this->on  = ON;
}

void  VIBREUR::setRPM(byte  RPM){

  this->rpm = RPM;
}

///////////////////////////////////////////////////////////////////

PRESS::PRESS(){

}

byte  PRESS::getVal(){

  return  this->val;
}
 
void  PRESS::setVal(byte  VAL){

  this->val = VAL;
}

