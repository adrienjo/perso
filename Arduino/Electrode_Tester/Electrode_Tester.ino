#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <SPI.h>

#include <Adafruit_NeoPixel.h>

#include "Electrode_Tester.h"

//WiFi :
const	char*	ssid		=	"Freebox-5F46E4";
const	char*   password	=	"gubernasti*-jaciens5-inriti?-distinctus#";

//UDP :
WiFiUDP	udp;
const	char*	udpAddress = "192.168.1.5";
const	int		udpPort = 52396;

uint8_t sensor;
uint8_t res_values[NB_SENSORS];

//LEDS :
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NB_SENSORS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

uint8_t leds_brightness = 50;
uint8_t test_cpt  = 0;

hw_timer_t*   timer = NULL;
bool          timer_refresh = 0;

/*
void IRAM_ATTR onTimer(){

  timer_refresh = 1;
}
*/

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(PIN_TEST_LED,OUTPUT);
  digitalWrite(PIN_TEST_LED,HIGH);

  init_LEDS();
  init_ADC();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(500);
    ESP.restart();
  }

  udp.begin(WiFi.localIP(),udpPort);

  ArduinoOTA.setHostname("Huzzy");
  //ArduinoOTA.setPassword("lemotdepasse");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
/*
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 100000, true);

  // Start an alarm
  timerAlarmEnable(timer);
*/
  uint32_t test = 8192000;  //correspond à 1V correspond à 100kohm si Itest=10µA

  test  = value_2_res(test);

  Serial.print("Rmes = ");  Serial.print(test);  Serial.println("ohm");

  test  = 0;

  for(sensor=0;sensor<8;sensor++) refresh_LEDS(test,sensor);
}

void loop() {
  
  ArduinoOTA.handle();
/*
  if(timer_refresh){

    for(sensor  = 0;  sensor<NB_SENSORS;  sensor++){
      
      res_values[sensor]  = measure_res();

      res_values[sensor]  = test_cpt+sensor*64;
      test_cpt++;
    }

    refresh_LEDS(res_values);
    
    timer_refresh = 0;
  }
*/
}

/*
 * Le timer ordonne un refresh durant lequel on doit tester successivement
 * toutes les électrodes, renvoyer une valeur de résistance, puis actualiser
 * toutes les LEDs en fonction des ces valeurs
 */
