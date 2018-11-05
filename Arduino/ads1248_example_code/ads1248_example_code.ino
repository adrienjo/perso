#include        <SPI.h>
#include        "ADS1248.h"

#define PIN_ADC_CS    15
#define PIN_ADC_DRDY  32
#define PIN_ADC_START 33
#define PIN_ADC_RESET 12

SPISettings settingsA(100000, MSBFIRST, SPI_MODE1);

bool    new_value_available     =       0;
bool    disp    =       0;

int32_t value,last_value;

float   voltage                  =       0;
float   measured_res    =       0;

uint8_t content =       0;

void  ADC_DataReady_ISR(){

  new_value_available = 1;
}

void  init_ADC(){

  SPI.begin();
  pinMode(PIN_ADC_CS,OUTPUT);
  pinMode(PIN_ADC_START,OUTPUT);
  pinMode(PIN_ADC_RESET,OUTPUT);
  pinMode(PIN_ADC_DRDY, INPUT_PULLUP);

  digitalWrite(PIN_ADC_CS,HIGH);
  digitalWrite(PIN_ADC_RESET,HIGH);

  delay(20);
  
  digitalWrite(PIN_ADC_START,HIGH);
  
  attachInterrupt(digitalPinToInterrupt(PIN_ADC_DRDY), ADC_DataReady_ISR, FALLING);

        digitalWrite(PIN_ADC_CS,LOW);
        
        ADC_Command(ADS1248_CMD_RESET); 

        delay(1);

        ADC_Command(ADS1248_CMD_SDATAC);

        ADC_Command(0x40);      //Ecriture config registres (à partir du registres 0x00)
        ADC_Command(0x03);      //3+1 = 4 registres :
        
        ADC_Command(0b10000001);        //reg 0x00              2uA current + AIN0(~2.2V) = AIN+ et AIN1(~1.1V) = AIN-
        //ADC_Command(0x00);      //reg 0x01              No biasing
        ADC_Command(0b00000010);      //reg 0x01              bias d'AIN- à 1.65V
        ADC_Command(0x30);      //reg 0x02
        ADC_Command(0x01);      //reg 0x03              Gain = 1, ODR = 10 SPS

         ADC_Command(ADS1248_CMD_SYNC);

        digitalWrite(PIN_ADC_CS,HIGH);

}

void ADC_Command(byte com_id) {

        SPI.beginTransaction(settingsA);

        //digitalWrite(PIN_ADC_CS, LOW);

        SPI.transfer(com_id);

        //digitalWrite(PIN_ADC_CS, HIGH);

        SPI.endTransaction();
}

uint8_t ADC_Read(byte thisRegister) {

   uint8_t      read_val;
   
   SPI.beginTransaction(settingsA);

  digitalWrite(PIN_ADC_CS, LOW);

SPI.transfer(0b00100000|thisRegister);        //First register to read 's adress
SPI.transfer(0b00000000|0);                              //Number of successive registers to read -1

  read_val      =       SPI.transfer(ADS1248_CMD_NOP);

  digitalWrite(PIN_ADC_CS, HIGH);

     SPI.endTransaction();

  return        read_val;
}

void ADC_Write(byte thisRegister, byte thisValue) {

        SPI.beginTransaction(settingsA);

        digitalWrite(PIN_ADC_CS, LOW);

        SPI.transfer(0b01000000|thisRegister);        //First register to read 's adress
        SPI.transfer(0b00000000|0);
        SPI.transfer(thisValue);

        digitalWrite(PIN_ADC_CS, HIGH);

        SPI.endTransaction();
}

int32_t        ADC_fetchValue(){

        int32_t        value   =       0;

        uint8_t  data[3];
        
        digitalWrite(PIN_ADC_CS,LOW);
        
            // RDATA command
        SPI.transfer(0x12);
         data[0] = SPI.transfer(ADS1248_CMD_NOP);
         data[1] = SPI.transfer(ADS1248_CMD_NOP);
         data[2] = SPI.transfer(ADS1248_CMD_NOP);
        digitalWrite(PIN_ADC_CS,HIGH);

        const uint8_t *byte = &data[0];
        for (size_t  n = 0; n < 3; ++n) {
                value = (value<< 8) | (int)*byte;
                byte++;
        }

        //Extension valeur signée à 32bits :
        if(value&(1<<23)) {                                     //si négatif
        value = 0xFF000000|value;
        }  

        return  value;
}

void setup() {

        

}

void loop() {

        if(new_value_available){

                //ADC_Command(ADS1248_CMD_RDATA);
                value   =       ADC_fetchValue()-21990;
                disp    =       1;
                new_value_available     =       0;
        }

        if(disp){

                value   =       0.5*value+0.5*last_value;
                last_value      =       value;
                
                voltage =       value*2.048/8388608;

                measured_res    =       voltage*500000;

                Serial.print(0);  // To freeze the lower limit
                Serial.print(" ");
                Serial.print(8388608);  // To freeze the upper limit
                Serial.print(" ");
                Serial.println(value);
         
                //Serial.print("Vdiff = ");
                //Serial.print(voltage,3);
                //Serial.print("V, RES = ");
                //Serial.println(measured_res,0);
                //Serial.println("Ohm.");

        }
}
