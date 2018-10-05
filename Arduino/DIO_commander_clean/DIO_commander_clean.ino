#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>

#include  "Prises_RF.h"
#include  "VideoProj_IR.h"
#include  "Alphabot_IR.h"

#define PIN_RF  12

#define FRAME_ON_1          1
#define FRAME_OFF_1         2
#define FRAME_ON_2          3
#define FRAME_OFF_2         4
#define FRAME_ON_3          5
#define FRAME_OFF_3         6
#define FRAME_ON_ALL        7
#define FRAME_OFF_ALL       8
#define FRAME_PROJ_ON_OFF   9
#define FRAME_ALPHABOT_IR_2 10
#define FRAME_ALPHABOT_IR_4 11
#define FRAME_ALPHABOT_IR_5 12
#define FRAME_ALPHABOT_IR_6 13
#define FRAME_ALPHABOT_IR_8 14


//const char* ssid          = "quedesvirus";                  //SSID du WiFi
//const char* password      = "0707106781186547";

const char* ssid          = "Connectify-me";                  //SSID du WiFi
const char* password      = "vocarof1";    

const char* wroom_name    = "DIO_Commander";

WiFiUDP   Udp;
char      packetBuffer[UDP_TX_PACKET_MAX_SIZE];   //buffer to hold incoming packet
char*     UDP_command;
int       len = 0;
int       packetSize  = 0;
char      UDP_response[50];
const int udp_port_number = 5040;

bool      refresh = 0,  carrier,  update_carrier, carrier_state,  state, frame_playing = 0;
uint8_t   state_counter = 0;
uint32_t  time_to_next_state;
uint8_t   frame_to_play;
uint16_t  frame_length;
uint32_t  current_counter_val,  last_counter_val, next_carrier_change,  carrier_half_period = 1053;


void  ICACHE_RAM_ATTR ontimerISR(){

  refresh = 1;
}

void setup()
{

  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  ArduinoOTA.setHostname(wroom_name);
  ArduinoOTA.begin();

  Udp.begin(udp_port_number);

  pinMode(PIN_RF,OUTPUT);

  timer1_attachInterrupt(ontimerISR);

}

void loop() {
  
  ArduinoOTA.handle();

  packetSize = Udp.parsePacket();
  len = 0;
  if (packetSize){ 

    len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;

    if(strcmp(packetBuffer,"1_ON")==0){
      frame_to_play = FRAME_ON_1;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing ON").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"1_OFF")==0){
      frame_to_play = FRAME_OFF_1;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing OFF").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"2_ON")==0){
      frame_to_play = FRAME_ON_2;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing ON").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"2_OFF")==0){
      frame_to_play = FRAME_OFF_2;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing OFF").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"3_ON")==0){
      frame_to_play = FRAME_ON_3;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing ON").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"3_OFF")==0){
      frame_to_play = FRAME_OFF_3;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing OFF").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"ALL_ON")==0){
      frame_to_play = FRAME_ON_ALL;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing ON").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"ALL_OFF")==0){
      frame_to_play = FRAME_OFF_ALL;
      frame_playing = 1;
      carrier       = 0;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
      String("Playing OFF").toCharArray(UDP_response,50);
    }
    else  if(strcmp(packetBuffer,"PROJ_ON_OFF")==0){
      frame_to_play = FRAME_PROJ_ON_OFF;
      frame_playing = 1;
      carrier       = 1;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    }
    else  if(strcmp(packetBuffer,"ALPHABOT_FRONT")==0){
      frame_to_play = FRAME_ALPHABOT_IR_2;
      frame_playing = 1;
      carrier       = 1;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    }
    else  if(strcmp(packetBuffer,"ALPHABOT_BACK")==0){
      frame_to_play = FRAME_ALPHABOT_IR_8;
      frame_playing = 1;
      carrier       = 1;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    }
    else  if(strcmp(packetBuffer,"ALPHABOT_LEFT")==0){
      frame_to_play = FRAME_ALPHABOT_IR_4;
      frame_playing = 1;
      carrier       = 1;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    }
    else  if(strcmp(packetBuffer,"ALPHABOT_RIGHT")==0){
      frame_to_play = FRAME_ALPHABOT_IR_6;
      frame_playing = 1;
      carrier       = 1;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    }
    else  if(strcmp(packetBuffer,"ALPHABOT_LEDS")==0){
      frame_to_play = FRAME_ALPHABOT_IR_5;
      frame_playing = 1;
      carrier       = 1;
      timer1_write(10000);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    }
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(UDP_response);
    Udp.endPacket();

    Serial.println(UDP_response);
  }

  if(frame_playing){

    switch(frame_to_play){
      
      case  FRAME_ON_1        : time_to_next_state  = ON_1_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;

      case  FRAME_OFF_1       : time_to_next_state  = OFF_1_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;
	  
	    case  FRAME_ON_2        : time_to_next_state  = ON_2_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;

      case  FRAME_OFF_2       : time_to_next_state  = OFF_2_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;
	  
	    case  FRAME_ON_3        : time_to_next_state  = ON_3_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;
      
      case  FRAME_OFF_3       : time_to_next_state  = OFF_3_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;
	  
	    case  FRAME_ON_ALL      : time_to_next_state  = ON_ALL_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;

      case  FRAME_OFF_ALL     : time_to_next_state  = OFF_ALL_states_durations[state_counter];
                                frame_length        = Prises_RF_frame_length;
      break;

      case  FRAME_PROJ_ON_OFF : time_to_next_state  = PROJ_ON_OFF_states_durations[state_counter];  
                                frame_length        = VideoProj_IR_frame_length;      
      break;

      case  FRAME_ALPHABOT_IR_2 : time_to_next_state  = Alphabot_IR_2_states_durations[state_counter];  
                                  frame_length        = Alphabot_IR_frame_length;      
      break;

      case  FRAME_ALPHABOT_IR_4 : time_to_next_state  = Alphabot_IR_4_states_durations[state_counter];  
                                  frame_length        = Alphabot_IR_frame_length;      
      break;

      case  FRAME_ALPHABOT_IR_5 : time_to_next_state  = Alphabot_IR_5_states_durations[state_counter];  
                                  frame_length        = Alphabot_IR_frame_length;      
      break;

      case  FRAME_ALPHABOT_IR_6 : time_to_next_state  = Alphabot_IR_6_states_durations[state_counter];  
                                  frame_length        = Alphabot_IR_frame_length;      
      break;

      case  FRAME_ALPHABOT_IR_8 : time_to_next_state  = Alphabot_IR_8_states_durations[state_counter];  
                                  frame_length        = Alphabot_IR_frame_length;      
      break;
    }

    if(carrier){

      

      //if(carrier_state) digitalWrite(PIN_RF,HIGH);
      if(state&&carrier_state) digitalWrite(PIN_RF,HIGH);
      else                     digitalWrite(PIN_RF,LOW);
    }
    else{

      if(state) digitalWrite(PIN_RF,HIGH);
      else      digitalWrite(PIN_RF,LOW);
    }
  }
  else{
    
    state_counter = 0;
    timer1_disable();
    
    digitalWrite(PIN_RF,LOW);
  }
//
  last_counter_val    = current_counter_val;

  current_counter_val = ESP.getCycleCount();

  update_carrier  = 0;

  if(current_counter_val<last_counter_val){
    
    next_carrier_change = carrier_half_period;
    update_carrier  = 1;    //débordement timer0
  }

  if(current_counter_val>=next_carrier_change){   //Génération porteuse télécommande IR 38kHz

    next_carrier_change +=  carrier_half_period;
    update_carrier  = 1;
  }

  if(update_carrier)  carrier_state = !carrier_state;
//  
  if(refresh){

    state = !(state_counter%2);       //On change l'état en fonction de l'avancement dans la trame

    timer1_write(time_to_next_state);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    
    state_counter++;
    if(state_counter>=frame_length){  //fin de trame
      frame_playing = 0;
    }

    refresh = 0;  
  }
}
