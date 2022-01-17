#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include <Keypad.h>
#include <Servo.h>

//Servo variables
int servoPin;
Servo servo; 
boolean isLocked = true;


//Keypad variables 
String password;
String correctPassword = "7290";




char ssid[] = "Dan";   // your network SSID (name) 
char pass[] = "pass1234";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;


//THINGSPEAK VARIABLES
unsigned long myChannelNumber = 1619452;
const char * writeAPIKey = "XVWNBPUB866FV0DJ";
const char * readAPIKey = "M56AP7G7VRSFSN98";
unsigned int counterFieldNumber = 1; 
//THINGSPEAK VARIABLES





void setup() 
{

  long count = 0;
  
  
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
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }  


  // Read in field 1 of the private channel which is a counter  
  long count = ThingSpeak.readLongField(myChannelNumber, counterFieldNumber, readAPIKey);  

   // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200)
  {
    Serial.println("Counter: " + String(count));


    //If count is equal to 1 then the code entered was correct
    if(count == 1)
    {
      Serial.println("Password correct");  
      //The lockUnlock function is called 
      lockUnlock();
      Serial.println(isLocked);
    }
    if(count == 0)
    {
      Serial.println("Password incorrect");
    }
  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  
  delay(15000); // No need to read the counter too often.
}




void lockUnlock()
{
  if(isLocked == 1)
  {
    servoPin = 3;
    servo.attach(servoPin);
    servo.write(90);
    delay(1000); 
    servo.detach();

    //Once the lock has been turned I post the number 0 back to the ThingSpeak channel to stop the device 
    //from turning the lock again
    for(int i = 0; i < 2; i++)
    {
      Serial.println("Resetpassword");
      Serial.println("Posting " + String(0, 2) + " to ThingSpeak");
      ThingSpeak.setField(1, String(0, 2));
      ThingSpeak.writeFields(myChannelNumber, writeAPIKey);
      delay(10000);
    }
  }
    
}
