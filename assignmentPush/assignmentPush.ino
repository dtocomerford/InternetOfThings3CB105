#include <SPI.h>
#include <WiFiNINA.h>
#include "ThingSpeak.h"
#include <Keypad.h>


char ssid[] = "Dan";
char pass[] = "pass1234";  


//KEYPAD VARIABLES
const byte ROWS = 4;
const byte COLS = 3;
String codeEntered = "";
String passCode = "1234#";

char hexaKeys[ROWS][COLS]={
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
  };

byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
//KEYPAD VARIABLES


//THINGSPEAK VARIABLES
unsigned long myChannelNumber = 1619452;
const char * myWriteAPIKey = "XVWNBPUB866FV0DJ";
//THINGSPEAK VARIABLES


WiFiClient client;



void setup()
{
  int status = WL_IDLE_STATUS;    
 
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
 
}


void loop()
{
  int statusCode = 0;
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    //Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }

  


  //KEYPAD FUCTION
  char customKey = customKeypad.getKey();
 
  if(customKey)
  {
     Serial.println(customKey);
     codeEntered += customKey;
  }
  if(customKey == '*')
  {
    Serial.println(codeEntered);
    codeEntered = "";
  }
  
  if(customKey == '#')
  {
    checkCode();
  } 
  //KEYPAD FUCTION
}


//CHECK CODE FUNCTION
void checkCode()
{
  if(codeEntered == passCode)
  {
    Serial.println("Code correct");
    
    for(int i = 0; i < 2; i++)
    {
      Serial.println("Posting " + String(1, 2) + " to ThingSpeak");
      ThingSpeak.setField(1, String(1, 2));
      ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      delay(10000);
    }
  }else{
      Serial.println("Incorrect code entered");
      for(int i = 0; i < 2; i++)
      {
        Serial.println("Posting " + String(0, 2) + " to ThingSpeak");
        ThingSpeak.setField(1, String(0, 2));
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        delay(10000);
      }
  }
  
}
   
