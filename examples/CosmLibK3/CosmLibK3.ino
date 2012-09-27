#include <Ethernet.h>
#include <SPI.h>
#include <CosmK3.h>
/*
   CosmK3 - Library for connection arduino to cosm.
   Created by Lars Holmberg, september, 2012.
   Released into the public domain.
 */
 
//Your cosm ApiKey and feed ID(cosm.com)
char cosmApiKey[]  = "XXX";
long feedID = 00000;      //Cosm Feed ID

//Your own Arduino MAC-address (you find it on a sticker on your TCP/IP shield)
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x9E, 0x66 };

//Instansiate the client
CosmK3 cosmClient = CosmK3(cosmApiKey,feedID,mac);

//Take it cool (no need to call cosm to often)
long previousMillis = 0;    
long interval = 1000;   

unsigned long i = 10;

void setup(){
  Serial.begin(57600);
  randomSeed(analogRead(0));
  //Setup Tcp/IP socket
  cosmClient.setupEthernet();
  Serial.println ("Local Arduino IPaddress");
  Serial.println (cosmClient.getDynamicIP());
  //Activate debugging
  //cosmClient.setDebug(true);
}

void loop(){
    unsigned long currentMillis = millis();
   if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;   
      //This reads new commands from stream3. Any value greater then 0 is a new command (Differs from last reading);
      //THE COMMAND IS ALWAYS RESET TO 0 after reading
      if (cosmClient.getCommandFromCosmStream3() > 0){
          Serial.print ("Command from Cosm: ");
          Serial.println(cosmClient.getCommandFromCosmStream3());
      }
      //Send data either as int or float
      cosmClient.sendStatusToCosmStream2(3);
      cosmClient.sendValueToCosmStream4((double)random(100)/100);
      cosmClient.sendValueToCosmStream5((int)random(1000));
      cosmClient.sendValueToCosmStream6(3.14);
      //Send and receive updates Cosm every 5th second;
      cosmClient.readAndWrite();
   }
}
