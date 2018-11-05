#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;

float   res_freq        =       0;

void setup() {
  Serial.begin(9600);
  Serial.println("DRV2605 Audio responsive test");
  drv.begin();

  drv.useLRA();
  
  drv.setMode(DRV2605_MODE_AUDIOVIBE);

  drv.writeRegister8(DRV2605_REG_AUDIOCTRL,0x3);        //10ms peak detection, 200Hz LP

  drv.writeRegister8(DRV2605_REG_AUDIOLVL,0x10);         //pas de min de détection

  drv.writeRegister8(DRV2605_REG_AUDIOMAX,0xFF);        //maximum : 1.8Vpp

  // ac coupled input, puts in 0.9V bias
  drv.writeRegister8(DRV2605_REG_CONTROL1, 0x20);  
 
  // analog input
  drv.writeRegister8(DRV2605_REG_CONTROL3, 0xA3);  
}


void loop() {

 Serial.println(drv.readRegister8(DRV2605_REG_LRARESON));
/*
        res_freq        =       1000000/(98.46*drv.readRegister8(DRV2605_REG_LRARESON));
        Serial.print("f_res = ");
        Serial.print(res_freq);
        Serial.println(" Hz.");
*/
}

/* Fréquence résonnance mesurée au tutut : 201Hz
 *  Donnée par le registre période : 53Hz
 * 
 * 
 */
