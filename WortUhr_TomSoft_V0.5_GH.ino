/********************************************************************

   WortUhr TomSoft
   on a ESP8266 - WeMos D1 mini

   The time is synchronised regularly with a NTP server
   NTP-Server: FritzBox  :-)

   Base: WortUhr von Ulrich Radig:
   http://www.ulrichradig.de/home/index.php/projekte/wort-uhr-neu
   Adaption and implementations from me

   TCP commands to control the clock functions:
   --------------------------------------------
   all values 0 - 255
   Colorpicker in NetIO
   SCL,rrr,ggg,bbb --> set color, from NetIO to clock
   GCL             --> get color, from clock to NetIO --> response: rrr,ggg,bbb
   Slider in NetIO
   SBN,vvv         --> set brightness, from NetIO to clock
   GBN             --> get brightness, from clock to NetIO --> response: vvv

   Used libraries:
   ---------------
   TimeLib:            http://www.arduino.cc/playground/Code/Time
   Timezone:           https://github.com/JChristensen/Timezone
   Adafruit Neopixel:  https://github.com/adafruit/Adafruit_NeoPixel

   Used pins:
   ----------
   Neopixel Data at D4 = GPIO2
   LDR           at A0 (analogue input WeMos) and at 3V3

   V0.1 - 08.09.2016: Initial release
   V0.2 - 09.09.2016: Initial values stores in EEPROM
   V0.3 - 09.09.2016: Implementation of TCP commands for NetIO
   V0.4 - 10.09.2016: Language "schwaebisch - dreiviertel statt viertel vor und viertel statt viertel nach"
   V0.5 - 18.09.2016: automatic adjustment of brightness using a LDR at A0

 *******************************************************************/


//****************************************
// Libraries

#include "config.h"
#include <TimeLib.h>
#include <Timezone.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "font.h"
#include "NTP.h"

#define DEBUG

//****************************************
// Variables

// Network data
const char ssid[] = "*************";        // network SSID (name)
const char pass[] = "*************";        // network password

IPAddress ip(192, 168, 178, 222);           // static IP of the clock
IPAddress gateway(192, 168, 178, 1);
IPAddress dns(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);

#define HOSTNAME "WORTUHR"

WiFiServer server(12340);

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);

time_t prevDisplay = 0;                                     // when the digital clock was displayed

unsigned int _sekunde = 0;
unsigned int _minute = 0;
unsigned int _stunde = 0;

uint32_t uhrzeit;

unsigned long previousMillis = 0;

// NeoPixel
Adafruit_NeoPixel ledstrip = Adafruit_NeoPixel(NUM_PIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
static byte pixelsR[NUM_PIXELS];
static byte pixelsG[NUM_PIXELS];
static byte pixelsB[NUM_PIXELS];

const long interval = 1000;
bool sendColor      = false;
bool sendBrightness = false;
bool change         = false;

struct GLOBAL {           // global variables for color and brightness
  int r;                  // color foreground
  int g;
  int b;
  byte bg_r;              // main color background
  byte bg_g;
  byte bg_b;
  byte rr;                // adjusted color background
  byte gg;
  byte bb;
  byte brn;               // brightness
};
GLOBAL G = { };

struct config_t {         // initial values, are read from EEPROM
  int sernr;
  int r;
  int g;
  int b;
  int bg_r;
  int bg_g;
  int bg_b;
  int brn;
} S;

// LDR
int LDRmax = 0;
int LDRmin = 1023;
int lastValue = 1;
byte outputValue = 0;
unsigned long lastBrightnessCheck = 0;

char ls_txt[50];            // variable for laufschrift

#include "uhr_func.h"


//------------------------------------------------------------------------------


//*************************************************
// Setup

void setup() {

  G.r     = 0;
  G.g     = 0;
  G.b     = 0;
  G.bg_r  = 0;
  G.bg_g  = 0;
  G.bg_b  = 0;
  G.rr    = 0;
  G.gg    = 0;
  G.bb    = 0;
  G.brn   = 0;

  //********************************************
  // Start serial
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("TomSoft WortUhr");
#endif
  delay(100);

  //********************************************
  // Adafruit NeoPixel
  ledstrip.begin();

  //********************************************
  // read initial values from EEPROM
  EEPROM.begin(9);
  S.sernr = EEPROM.read(1);
  S.r     = EEPROM.read(2);
  S.g     = EEPROM.read(3);
  S.b     = EEPROM.read(4);
  S.bg_r  = EEPROM.read(5);
  S.bg_g  = EEPROM.read(6);
  S.bg_b  = EEPROM.read(7);
  S.brn   = EEPROM.read(8);

#ifdef DEBUG
  Serial.printf("S.sernr:%u  S.r:%u   S.g:%u   S.b:%u S.brn:%u \n", S.sernr, S.r, S.g, S.b, S.brn);
#endif
  if (S.sernr != 100) {
    S.sernr = 100;
    S.r = 100;
    S.g = 100;
    S.b = 100;
    S.bg_r = 0;
    S.bg_g = 0;
    S.bg_b = 0;
    S.brn = 250;
    EEPROM.write(1, S.sernr);
    EEPROM.write(2, S.r);
    EEPROM.write(3, S.g);
    EEPROM.write(4, S.b);
    EEPROM.write(5, S.bg_r);
    EEPROM.write(6, S.bg_g);
    EEPROM.write(7, S.bg_b);
    EEPROM.write(8, S.brn);
    EEPROM.commit();
  }

  G.r     = S.r;
  G.g     = S.g;
  G.b     = S.b;
  G.bg_r  = S.bg_r;
  G.bg_g  = S.bg_g;
  G.bg_b  = S.bg_b;
  G.brn   = S.brn;

  ledstrip.setBrightness(G.brn);

  //********************************************
  // Start Network
#ifdef DEBUG
  Serial.println("Try to connect WiFi");
#endif
  String hostname(HOSTNAME);
  WiFi.hostname(hostname);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
  }

#ifdef DEBUG
  Serial.print("WiFi connected, using this IP address: ");
  Serial.println(WiFi.localIP());
#endif

  // Initialize NTP Client and get the time
  initNTP();
  // Set the time provider to NTP
  setSyncProvider(getLocalTime);
  setSyncInterval(300);                                 // every 5 minutes

  // OTA functions
  ArduinoOTA.onStart([]() {
    //Serial.println("Start");
    led_clear();
    ledstrip.show();
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
    led_clear();
    ledstrip.show();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    int led = map(progress, 0, total, 0, 11);
    ledstrip.setPixelColor(led, 100, 100, 100);
    ledstrip.show();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();

  // ******************************************
  // Start of the TCP Server
  server.begin();

  //********************************************
  // test run all LEDs
  test_led();
  led_clear();
  ledstrip.show();

  /*
    for ( unsigned int i = 10; i > 0; i--) {
      char d1[5];
      char d2[5];
      sprintf(d1,"%d",(int)(i / 10));
      sprintf(d2,"%d",(int)(i % 10));

      zahlen(d1[0], d2[0]);
      delay(500);
    }
    led_clear();
    strcpy( ls_txt, "HELLO WORLD " );
    laufschrift(100,100,100, 100);
    laufschrift(100,100,100, 100);
  */

  //********************************************
  // first set of time
  set_bg_color();
  set_bg();
  _sekunde = second();
  _minute  = minute();
  _stunde  = hour();
  set_uhrzeit();
  ledstrip.show();    //Adafruit NeoPixel
}

//------------------------------------------------------------------------------
//*************************************************
// the LOOP

void loop() {

  ArduinoOTA.handle();                 // function to handle OTA

  showTime();                          // function to show time if time has changed

  handleNetIOClient();                 // function to handle input from web client NetIO

  handleLDR();                         // function to handle the brightness with the LDR

  handleChange();                      // funtion to handle changes from the web client and save them to EEPROM

}

//------------------------------------------------------------------------------
//*************************************************
// the subroutines

time_t getLocalTime(void) {
  return CE.toLocal(getNTPTime());
}

//------------------------------------------------------------------------------

// function to show the time if time has changes since last call
void showTime() {
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) {                  // update the display only if time has changed
      prevDisplay = now();
      _sekunde = second();
      _minute  = minute();
      _stunde  = hour();
#ifdef DEBUG
      Serial.printf("Zeit: %d:%d:%d \n", _stunde, _minute, _sekunde);
#endif
      led_clear();
      set_bg_color();
      set_bg();
      set_uhrzeit();
      ledstrip.show();
    }
  }
}

//------------------------------------------------------------------------------

// function to check the commands from the web client
void parseCommand(String text) {
  //Serial.println(text);
  String cmd = text.substring(0, 3);

  if (cmd == "SCL") {                   // set color
    int p_r = text.indexOf(',');
    int p_g = text.indexOf(',', p_r + 1);
    int p_b = text.indexOf(',', p_g + 1);
    String v_r = text.substring(p_r + 1, p_g);
    String v_g = text.substring(p_g + 1, p_b);
    String v_b = text.substring(p_b + 1);
    G.r = v_r.toInt();
    G.g = v_g.toInt();
    G.b = v_b.toInt();
    //Serial.print(G.r); Serial.print(G.g); Serial.println(G.b);
    change = true;
  }

  else if (cmd == "SBN") {              // set brightness
    int p_bn = text.indexOf(',');
    String v_bn = text.substring(p_bn + 1);
    G.brn = v_bn.toInt();
    //Serial.println(G.brn);
    ledstrip.setBrightness(G.brn);
    change = true;
  }

  else if (cmd == "GCL" ) {             // get color = send color values to NetIO
    sendColor = true;
  }

  else if (cmd == "GBN" ) {             // get brightness = send brightness value to NetIO
    sendBrightness = true;
  }

}
//------------------------------------------------------------------------------

// function to get commands from the web client NetIO
void handleNetIOClient() {
  WiFiClient client = server.available();                   // check if a client has connected

  // Steuerung NetIO
  if (client) {
#ifdef DEBUG
    Serial.println("Client connected!");
#endif
    while (client.connected()) {                            // client is connected
      if (client.available()) {                             // client sends data
        String command = client.readStringUntil('\n');      // read the whole text until termination character
        //Serial.print(command + "   ");
        parseCommand(command);
        if (sendColor) {                                    // send color values
          client.println("color" + String(G.r) + "," + String(G.g) + "," + String(G.b));
          sendColor = false;
        }
        if (sendBrightness) {                               // send brightness value
          client.println("value" + String(G.brn));
          sendBrightness = false;
        }
      }
      showTime();
    }
#ifdef DEBUG
    Serial.println("Client disconnected!");
#endif
    client.stop();
  }
}

//------------------------------------------------------------------------------

// function to save changes to EEPROM if there are any
void handleChange() {
  if (change) {
    S.r     = G.r;
    S.g     = G.g;
    S.b     = G.b;
    S.bg_r  = G.bg_r;
    S.bg_g  = G.bg_g;
    S.bg_b  = G.bg_b;
    S.brn   = G.brn;
    EEPROM.write(2, S.r);
    EEPROM.write(3, S.g);
    EEPROM.write(4, S.b);
    EEPROM.write(5, S.bg_r);
    EEPROM.write(6, S.bg_g);
    EEPROM.write(7, S.bg_b);
    EEPROM.write(8, S.brn);
    EEPROM.commit(); delay(100);
    change = false;
  }
}

//------------------------------------------------------------------------------

// function to check the brightness with a LDR and react accordingly
void handleLDR() {
  if (millis() < lastBrightnessCheck) {                  // wir hatten einen Ueberlauf...
    lastBrightnessCheck = millis();
  }
  if (lastBrightnessCheck + LDR_CHECK_RATE < millis()) { // langsam nachsehen...
    byte lv = getLDRvalue();
    if (ledstrip.getBrightness() > lv) {
      ledstrip.setBrightness(ledstrip.getBrightness() - 1);
      //ledstrip.show();
    }
    else if (ledstrip.getBrightness() < lv) {
      ledstrip.setBrightness(ledstrip.getBrightness() + 1);
      //ledstrip.show();
    }
    lastBrightnessCheck = millis();
  }
}

//------------------------------------------------------------------------------

// function to get the value from the LDR
byte getLDRvalue() {
  int rawVal, val;
  rawVal = analogRead(LDRPIN);

  if ((rawVal != lastValue) && ((rawVal == 0) || (rawVal == 1023) || (rawVal > (lastValue + LDR_HYSTERESE) || (rawVal < lastValue - LDR_HYSTERESE)))) {
    val = rawVal;
    lastValue = val;

    if (val < LDRmin) {
      LDRmin = val;
    }
    if (val > LDRmax) {
      LDRmax = val;
    }

    int mapVal = map(val, LDRmin - 1, LDRmax, 0, G.brn);
    mapVal = constrain(mapVal, LDR_MIN, LDR_MAX);

#ifdef DEBUG
    Serial.print("rawVal: ");
    Serial.print(rawVal);
    Serial.print(" val: ");
    Serial.print(val);
    Serial.print(" min: ");
    Serial.print(LDRmin);
    Serial.print(" max: ");
    Serial.print(LDRmax);
    Serial.print(" mapValue: ");
    Serial.println(mapVal);
#endif

    outputValue = mapVal;
  }
  return outputValue;
}

