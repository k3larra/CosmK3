#include <Ethernet.h>
#include <SPI.h>
#include <CosmK3.h>
/*
   CosmK3 - Library for connection arduino to cosm.
   Created by Lars Holmberg, september, 2012.
   Released into the public domain.
 */
 
//Your cosm ApiKey and feed ID(cosm.com)
char cosmApiKey[]  = "XXXX";
long feedID = 00000;      //Cosm Feed ID

//Your own Arduino MAC-address (you find it on a sticker on your TCP/IP shield)
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x9E, 0xBD };

//Instansiate the client
CosmK3 cosmClient = CosmK3(cosmApiKey,feedID,mac);

//Take it cool with sensor read
long previousMillis = 0;    
long interval = 4000;   

void setup(){
  Serial.begin(57600);
  randomSeed(analogRead(0));
  //Setup Tcp/IP socket
  cosmClient.setupEthernet();
  Serial.println ("Local Arduino IPaddress");
  Serial.println (cosmClient.getDynamicIP());
}

void loop(){
  //This reads new commands from stream3. Any value greater then 0 is a new command (Differs from last reading);
  //ClearCommandFromCosm resets the command on Cosm to 0;
  if (cosmClient.getCommandFromCosmStream3() > 0){
      Serial.print ("New command from Cosm: ");
      Serial.println(cosmClient.getCommandFromCosmStream3());
      cosmClient.clearCommandFromCosm();
  }
  
  //Send data either as int or float
    unsigned long currentMillis = millis();
   if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;   
      //Send data either as int or float
      cosmClient.sendStatusToCosmStream2(3);
      cosmClient.sendValueToCosmStream4((double)random(100)/100);
      cosmClient.sendValueToCosmStream5((int)random(1000));
      cosmClient.sendValueToCosmStream6(3.14145);
   }
  
  //Send and receive updates Cosm every 10th second;
  cosmClient.readAndWrite();
}
