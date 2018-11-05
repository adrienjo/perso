#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <SPI.h>

#include <Adafruit_NeoPixel.h>

#include "Electrode_Tester.h"

//WiFi :
const	char*   ssid		=	"Freebox-5F46E4";
const	char*   password	=	"gubernasti*-jaciens5-inriti?-distinctus#";

//UDP :
WiFiUDP	udp;
const	char*	udpAddress = "192.168.1.5";
const	int		udpPort = 52396;

bool    new_value_available     =       0,      disp    =       0;

int32_t value,last_value;

float   voltage                  =       0;
float   measured_res    =       0;

//LEDS :
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NB_SENSORS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

uint8_t leds_brightness = 20;
uint8_t test_cpt  = 0;
uint8_t led;

void setup() {
        
        Serial.begin(115200);
        Serial.println("Booting");
        
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

}

void loop() {
  
        ArduinoOTA.handle();

        if(new_value_available){

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

                leds.show();
        }

}
