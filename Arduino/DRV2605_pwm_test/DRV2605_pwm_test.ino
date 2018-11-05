#include <Wire.h>
#include "Adafruit_DRV2605.h"

#define MAX_PWM 100

Adafruit_DRV2605 drv;

float   res_freq        =       0;
uint8_t cpt = 50;

void setup() {
  Serial.begin(9600);
  drv.begin();

  drv.useLRA();
  
  //drv.setMode(DRV2605_MODE_PWMANALOG);

  drv.setMode(DRV2605_MODE_REALTIME);
}


void loop() {

drv.setRealtimeValue(cpt);

        res_freq        =       1000000/(98.46*drv.readRegister8(DRV2605_REG_LRARESON));

         Serial.print("Val = ");
        Serial.print(cpt);
        Serial.println("/255.");
        
        Serial.print("f_res = ");
        Serial.print(res_freq);
        Serial.println(" Hz.\n");

        delay(250);

 //       if(cpt<MAX_PWM)         cpt++;
    //   else                                     cpt     =       0;

}

/*      Fréquence de résonnance mesurée varie au cours du temps
 * 
 * 
 * 
 */
