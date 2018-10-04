#ifndef Electrode_Tester_H
#define Electrode_Tester_H

#include        "ADS1248.h"

#define PIN_TEST_LED  13
#define PIN_LEDS      21
#define PIN_ADC_CS    15
#define PIN_ADC_DRDY  32
#define PIN_ADC_START 33
#define NB_SENSORS    8
#define VREF_ADC      2048  //mV
#define ITEST_ADC     10    //µA

extern  Adafruit_NeoPixel leds;
extern  uint8_t           leds_brightness;

const byte READ = 0b11111100;     // SCP1000's read command
const byte WRITE = 0b00000010;   // SCP1000's write command

bool  new_value_available = 0;

void SPI_Write(byte thisRegister, byte thisValue) {

  // SCP1000 expects the register address in the upper 6 bits
  // of the byte. So shift the bits left by two bits:
  thisRegister = thisRegister << 2;
  // now combine the register address and the command into one byte:
  byte dataToSend = thisRegister | WRITE;

  // take the chip select low to select the device:
  digitalWrite(PIN_ADC_CS, LOW);

  SPI.transfer(dataToSend); //Send register location
  SPI.transfer(thisValue);  //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(PIN_ADC_CS, HIGH);
}

void  init_ADC(){

  SPI.begin();
  pinMode(PIN_ADC_CS,OUTPUT);
  pinMode(PIN_ADC_DRDY, INPUT_PULLUP);
  //

  //SPI_Write(0x69,0x42);
}

void  ADC_DataReady_ISR(){

  Serial.println("New data !");
  new_value_available = 1;
}

uint32_t  get_value(uint8_t nb_values_mean){

  uint32_t values[nb_values_mean];
  
  uint8_t  val = 0;
  
  attachInterrupt(digitalPinToInterrupt(PIN_ADC_DRDY), ADC_DataReady_ISR, FALLING);

  digitalWrite(PIN_ADC_START,HIGH); //On lance les conversions (datarate config ADC)

  Serial.println("Debut des conversions ! (ADC_START = 1)");

  while(val<nb_values_mean){  //bloquant!

    //Serial.println("Meanwhile...");

    new_value_available = 1;
  
    if(new_value_available){

      //Serial.println("Nouvelle valeur disponible ! (ADC_/DRDY = 0)");

      //envoie de 24 cycles sur SCLK et récupération des valeurs
      digitalWrite(PIN_ADC_CS,LOW);
      values[val] = SPI.transfer(ADS1248_CMD_NOP)<<16;
      values[val] |= SPI.transfer(ADS1248_CMD_NOP)<<8;
      values[val] |= SPI.transfer(ADS1248_CMD_NOP);
      digitalWrite(PIN_ADC_CS,HIGH);

      //Serial.print("Valeur ");  Serial.print(val);  Serial.print(" : ");
      //Serial.println(values[val]);

      val++;

      //TEST SUR VALEUR (pas saturation, ...)
      //SI VALEUR CORRECTE  : val++;          (SINON on prendra la prochaine)
      //SINON       : incrément d'un compteur de valeurs foireuses (pour sortir de la boucle quoi qu'il arrive)
      new_value_available = 0;
    }
  }
  
  digitalWrite(PIN_ADC_START,LOW);  //Arrêt des conversions

  Serial.println("Fin des conversions ! (ADC_START = 0)");

  
  detachInterrupt(digitalPinToInterrupt(PIN_ADC_DRDY));
  
  //Moyenne des valeurs obtenues :
  
  uint32_t  final_value = 0;
  
  for(val=0;val<nb_values_mean;val++)  final_value +=  values[val]/nb_values_mean;
  
  return  final_value;

}

uint32_t  value_2_res(uint32_t value){

  //Courant de test = 10uA

  float temp  = value;

  Serial.println(temp);  

  temp  = (temp*100)/8192;

  //temp  = temp*(1000*VREF_ADC/16777216)/ITEST_ADC;

  
  Serial.println(temp);  

  uint32_t res  = (uint32_t)temp;

  
  Serial.println(res);  
  
  return res;
}

void  init_LEDS(){

  leds.begin();
  for(uint8_t led = 0; led<NB_SENSORS; led++) leds.setPixelColor(led,leds.Color(leds_brightness,0,leds_brightness));

  leds.show();
}

void  refresh_LEDS(uint32_t res,uint8_t sensor){

  uint8_t lum_val = res/1000;

  leds.setPixelColor(sensor, leds.Color(lum_val,255-lum_val,0));

  leds.show();
}


#endif
