#ifndef Electrode_Tester_H
#define Electrode_Tester_H


#define PIN_TEST_LED  13
#define PIN_LEDS      21
#define PIN_ADC_CS    15
#define PIN_ADC_DRDY  32
#define NB_SENSORS    8

extern  Adafruit_NeoPixel leds;
extern  uint8_t           leds_brightness;

const byte READ = 0b11111100;     // SCP1000's read command
const byte WRITE = 0b00000010;   // SCP1000's write command


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

void  ADC_DataReady(){    //déclenché par DRDY : à faire : aller lire les données et reconf pour sensor suivant

  Serial.println("Data is Ready !");
}

void  set_ADC(uint8_t sensor){

  SPI.begin();
  pinMode(PIN_ADC_CS,OUTPUT);
  pinMode(PIN_ADC_DRDY, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ADC_DRDY), ADC_DataReady, FALLING);

  SPI_Write(0x69,0x42);
}

uint8_t  measure_res(){

  SPI_Write(0x69,0x42);

  return 0;
}

void  init_LEDS(){

  leds.begin();
  for(uint8_t led = 0; led<NB_SENSORS; led++) leds.setPixelColor(led,leds.Color(leds_brightness,0,leds_brightness));

  leds.show();
}

void  refresh_LEDS(uint8_t res_values[]){

  for(uint8_t led = 0; led<NB_SENSORS; led++){

    
    
    leds.setPixelColor(led, leds.Color(res_values[led],255-res_values[led],0));
  }

  leds.show();
}


#endif
