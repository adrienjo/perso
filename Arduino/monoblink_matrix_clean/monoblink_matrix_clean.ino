#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>

#include  "Monoblink_Matrix.h"

#include  "frames_sys.h"
#include  "frames_gabor.h"
#include  "frames_black_white.h"
#include  "frames_feedback.h"
#include  "frames_line_tilt.h"
#include  "frames_particules.h"
#include  "patterns.h"

#define SHUTDOWNPIN 12
#define CHARGE_PIN  5

#define NUMPIXELS   64
#define RAF_FREQ    60

#define COM_LUMI      1
#define COM_PATTERN   2
#define COM_STIM      3
#define COM_FEEDBACK  4
#define COM_SHUTDOWN  5
#define COM_RESYNC    6
#define COM_RSSI      7

#define SET_SYS       0
#define SET_BW        1
#define SET_LINES     2
#define SET_CHECKER   3
#define SET_PART      4

//const char* ssid          = "Livebox-2408";                  //SSID du WiFi
//const char* password      = "sigma,nearby,tourbillon,colt";  //mot de passe du WiFi

//const char* ssid          = "reso";                  //SSID du WiFi
//const char* password      = "jesuisunmotdepasse";  //mot de passe du WiFi

const char* ssid          = "Connectify-me";                  //SSID du WiFi
const char* password      = "vocarof1";             //mot de passe du WiFi

const char* wroom_name      = "WRM-03";
const int   udp_port_number = 5060;

WiFiUDP Udp;
char    packetBuffer[UDP_TX_PACKET_MAX_SIZE];   //buffer to hold incoming packet
char*   UDP_command;
int     len = 0;
int     packetSize  = 0;
char    UDP_response[50];

uint8_t comID, value;

unsigned  int pixel,frame_counter;

uint8_t       frame_set;
uint8_t       pattern;
uint8_t       feedback_level = 0;
bool          resync        = 0;
bool          sd            = 0;

uint8_t       rgb_balance[]   = {190,255,240};   // entre 0 et 255
uint8_t       composante;
uint8_t       disp_col        = 0;               //mode RGB par défaut
uint16_t      mean_rgb;
bool          rgb_fader[]     = {0,0,0};

uint8_t       user_brightness = 20;
uint8_t       hw_brightness = 1;                //0: ça va être tout noir, 31: mélanôme incoming


uint16_t      past_sensor = 1023;

bool          refresh;

uint32_t      period_count  = 80000000/(RAF_FREQ);

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

void ICACHE_RAM_ATTR onTimerISR(){

    refresh = 1;
}

void  UDP_parser(char* packetBuffer,uint8_t* comID,uint8_t* value){

  uint8_t c;

  char  command,value_char[9];

  command = packetBuffer[0];

  switch(command){

    case  'l' : *comID  = COM_LUMI;     break;
    case  'p' : *comID  = COM_PATTERN;  break;
    case  's' : *comID  = COM_STIM;     break;
    case  'f' : *comID  = COM_FEEDBACK; break;
    case  'x' : *comID  = COM_SHUTDOWN; break;
    case  'r' : *comID  = COM_RESYNC;   break;
    case  'w' : *comID  = COM_RSSI;     break;

  }

  if(packetBuffer[1]=='\0') return;  //si commande sans valeur (ex: resync ou shutdown)
  else{

    c = 1;

    while(packetBuffer[c]!='\0'){
  
      value_char[c-1] = packetBuffer[c];
      c++;
    }
  
    *value = atoi(value_char);
  }
}

void  update_brightness(uint8_t imposed_val){

  if(imposed_val){
    
    strip.setHWBrightness(imposed_val);
  }
  else{

    uint16_t  sensor  = analogRead(A0);

    sensor = 0.1*sensor+0.9*past_sensor;

    if(sensor==0) sensor  = 1;

    hw_brightness = 1024/sensor;
    if(hw_brightness<1) hw_brightness = 1;
    if(hw_brightness>31) hw_brightness = 31;

    strip.setHWBrightness(hw_brightness);

    past_sensor = sensor;
  }
}

void  refresh_matrix(uint8_t frame_set,uint8_t pattern,uint8_t frame_counter,uint8_t feedback_level,uint8_t rgb_balance[]){

  uint16_t   rgb_stim[3];
  uint16_t   rgb_fdbck[3];
  uint16_t  rgb_sum[3];
  uint32_t  output_RGB;

  bool      frame_state   = dgkPattern[pattern][frame_counter];

  uint16_t  frame_number  = frame_counter+frame_state*240;

  for(pixel=0;pixel<NUMPIXELS;pixel++){

    output_RGB  = 0x000000;

    for(composante=0;composante<3;composante++){

      //On va lire les valeurs des composantes pour le pixel en cours :
      switch(frame_set){
  
        case  SET_SYS   : rgb_stim[composante] = pgm_read_byte_near( frames_sys   +3*(NUMPIXELS*frame_counter+pixel)+composante );  break;
                                                    
        case  SET_CHECKER : rgb_stim[composante] = pgm_read_byte_near( frames_gabor +3*(NUMPIXELS*frame_state+pixel)+composante );  break;
                                                    
        case  SET_BW    : rgb_stim[composante] = pgm_read_byte_near( frames_BW    +3*(NUMPIXELS*frame_state+pixel)+composante );  break;
  
        case  SET_LINES   : rgb_stim[composante] = pgm_read_byte_near( frames_LINES    +3*(NUMPIXELS*frame_state+pixel)+composante );  break;

        case  SET_PART    : rgb_stim[composante] = pgm_read_byte_near( frames_PARTICULES    +3*(NUMPIXELS*frame_number+pixel)+composante );  break;

      }

      //Intégration du feedback :

      rgb_fdbck[composante] = pgm_read_byte_near( frames_feedback    +3*pixel+composante );
      
      //if(frame_state) rgb_fdbck[composante] = pgm_read_byte_near( frames_feedback    +3*(NUMPIXELS*0+pixel)+composante );
      //else            rgb_fdbck[composante] = pgm_read_byte_near( frames_feedback    +3*(NUMPIXELS*0+pixel)+composante )/2;

      //rgb_sum[composante] = rgb_stim[composante];
      //rgb_sum[composante] = rgb_stim[composante]/2+(feedback_level/100)*rgb_fdbck[composante]/2;
      //rgb_sum[composante] = ((100-(float)feedback_level)/100)*rgb_stim[composante]+((float)feedback_level/100)*rgb_fdbck[composante];
      if(rgb_fdbck[composante]>0) rgb_sum[composante] = ((100-(float)feedback_level)/100)*rgb_stim[composante]+((float)feedback_level/100)*rgb_fdbck[composante];
      else                        rgb_sum[composante] = rgb_stim[composante];

      if(rgb_sum[composante]>255) rgb_sum[composante] = 255;


      //On corrige les couleurs :
      rgb_sum[composante] = ((float)rgb_sum[composante]*rgb_balance[composante])/255;

      //On les corrige pour que la perception visuelle corresponde mieux aux valeurs :
      rgb_sum[composante] = strip.gamma8(rgb_sum[composante]);

      output_RGB  |=  rgb_sum[composante]<<(16-8*composante);
    }

    strip.setPixelColor(pixel,  output_RGB);
  }

  strip.show();
}

void setup() {

  strip.begin();
  strip.setHWBrightness(hw_brightness);

  //A faire après le strip.begin() qui initialise PIN12 en entrée (MISO du SPI)
  pinMode(SHUTDOWNPIN,OUTPUT);      //Le module s'éteint si cette sortie est pas immédiatement
  digitalWrite(SHUTDOWNPIN,HIGH);   //au niveau haut au démarrage (niveau bas = shutdown)

  //"Ecran d'accueil (ON)"
  refresh_matrix(SET_SYS,0,0,0,rgb_balance);

  WiFi.begin(ssid, password);
  /*
  IPAddress ip(192,168,1,74);
  IPAddress gateway(192,168,1,254);
  IPAddress subnet(255,255,255,0);
  WiFi.config(ip, gateway, subnet);
  */
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(200);
  }

  //Petite animation connexion wifi :
  refresh_matrix(SET_SYS,0,1,0,rgb_balance);
  delay(250);
  refresh_matrix(SET_SYS,0,2,0,rgb_balance);
  delay(250);
  refresh_matrix(SET_SYS,0,3,0,rgb_balance);
  delay(500);

  ArduinoOTA.setHostname(wroom_name);
  ArduinoOTA.begin();

  Udp.begin(udp_port_number);

  timer1_attachInterrupt(onTimerISR);
  timer1_write(period_count);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);

  frame_set = SET_CHECKER;     //On passe sur le set de frames correspondant à la stim gabor
  pattern   = 1;                //Et on choisit un des patterns définis dans patterns.h

}

void loop() {

  ArduinoOTA.handle();

  packetSize = Udp.parsePacket();
  len = 0;
  if (packetSize){ 

    len = Udp.read(packetBuffer, 255);
    packetBuffer[len] = '\0';
 
    UDP_parser(packetBuffer,&comID,&value);

    switch(comID){

      case  COM_LUMI      : user_brightness  = value;
                            String(String("Luminosite = ")+String(user_brightness,DEC)).toCharArray(UDP_response,50);
      break;
      
      case  COM_PATTERN   : pattern     = value;
                            String(String("Pattern = ")+String(pattern,DEC)).toCharArray(UDP_response,50);
      break;
      
      case  COM_STIM      : frame_set        = value;
                            String(String("Stim = ")+String(frame_set,DEC)).toCharArray(UDP_response,50);
      break;
      
      case  COM_FEEDBACK  : feedback_level    = value;
                            String(String("Feedback = ")+String(feedback_level,DEC)).toCharArray(UDP_response,50);
      break;
      
      case  COM_SHUTDOWN  : sd  = 1;              break;
      
      case  COM_RESYNC    : resync  = 1;
                            String("Stim resynchronisee !").toCharArray(UDP_response,50);
      break;

      case  COM_RSSI      : long rssi  = WiFi.RSSI();
                            String(String("RSSI = ")+String(rssi,DEC)).toCharArray(UDP_response,50);
      break;

    }

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(UDP_response);
    Udp.endPacket();

    packetSize  = 0;

  }

  if(resync){
    //reset du timer et retour à la première frame du pattern :
      frame_counter = 0;
      timer1_disable();
      timer1_write(period_count);
      timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);

      resync  = 0;

      refresh = 1;
  }

  if(refresh){

    if(sd){

      for(pixel=0;pixel<NUMPIXELS;pixel++)  strip.setPixelColor(pixel,  0x000000);
      strip.show();
      delay(500);
      digitalWrite(SHUTDOWNPIN,LOW);
      delay(2000);
    }

    //update_brightness(0); //Mesure la luminosité ambiante
    update_brightness(1); //Luminosité matrice imposée

    //feedback_level = frame_counter;
    //feedback_level = 0;

    refresh_matrix(frame_set,pattern,frame_counter,feedback_level,rgb_balance);

    frame_counter++;
    if(frame_counter>=PATTERN_LENGTH) frame_counter = 0;

    refresh = 0;
  }
}

