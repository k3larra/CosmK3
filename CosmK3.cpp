#include "Arduino.h"
#include <Ethernet.h>
#include "CosmK3.h"
#include <SPI.h>

int _arduinoStatus=0;
String _valueToCosmStream4 = "0.00";
String _valueToCosmStream5 = "0.00";
String _valueToCosmStream6 = "0.00";
EthernetClient _cosmclient;
// Cosm variables
//char *_cosmserver = "api.cosm.com";
const byte _cosmserver[] = { 173,203,98,29 };
//Variable declarations for cosm
boolean _clearCommandFromCosm = false;
boolean debug = false; 
boolean debugVerbose = false;
char *found;
char buff[64];
int pointer = 0;
unsigned int successes = 1;
unsigned int failures = 0;
unsigned int maxSuccesses = 10000;
boolean ready_to_update = true;
boolean reading_pachube = false;
boolean request_pause = false;
unsigned long last_connect = 0;
unsigned int content_length = 0;
unsigned long connection_duration =0;
int commandToExecute = 0;
unsigned int last_command = 0; //Tracks new commands
unsigned int command_tail = strlen("HTTP/1.1 200 OK")+2; //

CosmK3::CosmK3(char cosmApiKey[], uint32_t cosmFeedID, byte arduinoMAC[] )
{
   useDHCP = true;
   _updateInterval = 10000;
   _cosmApiKey = cosmApiKey;
   _cosmFeedID = cosmFeedID;
   _arduinoMAC = arduinoMAC; 
}

CosmK3::CosmK3(char cosmApiKey[],uint32_t cosmFeedID, byte arduinoMAC[], IPAddress arduinoStaticIP)
{
  useDHCP = false;
  _updateInterval = 10000;
  _cosmApiKey = cosmApiKey;
   _cosmFeedID = cosmFeedID;
  _arduinoMAC = arduinoMAC;
  _arduinoStaticIP = arduinoStaticIP;
}

void CosmK3::setupEthernet(){
   if(useDHCP){ //Configure DHCP
      Ethernet.begin(_arduinoMAC);
     _arduinoDynamicIP = Ethernet.localIP();
    }else{  //Configure static
      Ethernet.begin(_arduinoMAC, _arduinoStaticIP);
  }
  delay(1000);
}

IPAddress CosmK3::getDynamicIP(){
  return  _arduinoDynamicIP;
}

void CosmK3::setUpdateInterval(unsigned long seconds){
//  if ((seconds < 6000) && (seconds >0)){
//    _updateInterval = seconds*1000;
//  }
}

void CosmK3::readAndWrite(){
    if(millis()<last_connect){   //When millis reaches maxint after a few weeks this check is needed
      last_connect=millis();
  }
  
  //This metod is used when paused and waiting for _updateInterval to pass
  if (request_pause){
     if (debug){
        if((abs(millis() - last_connect)%6000)==0){ //get an idea of waiting time
          Serial.print("Time to next request: ");
          Serial.print(abs(_updateInterval - (millis() - last_connect))/1000);
          Serial.println("(s)");
        }
      }
    if (abs(millis() - last_connect) > _updateInterval){ //Time to connect?
      ready_to_update = true;
      reading_pachube = false;
      request_pause = false;
    }
  }

  //Run this when it is time to update Cosm
  if (ready_to_update){
    if (debug){Serial.println("Connecting...");}
    if (_cosmclient.connect(_cosmserver, 80)) {
      //We are connected lets ask for data
      if (debug){Serial.println("Sending GET request");}
      _cosmclient.print("GET /api/");
      _cosmclient.print(_cosmFeedID);
      _cosmclient.print(".csv HTTP/1.1\nHost: pachube.com\nX-PachubeApiKey: ");
      _cosmclient.print(_cosmApiKey);
      _cosmclient.print("\nUser-Agent: Arduino (Pachube controller v1.1)");
      _cosmclient.println("\n");
      //Put some data
      //comma-separated values put to cosm_data, this will update Cosm datastreams
      //statusOfBoard,successes, failures,command,status(not sent",sensor11,sensor2,sensor3
      //Test data static sensorvalues
      //sprintf(cosm_data,"%d,%d,%d,,4,5,6",successes,failures,statusOfBoard);
      //Data
      char data[32];
      sprintf(data,"%d,%d,%d,%d,",successes,failures,_arduinoStatus,0);
      String string_to_send = data;
      string_to_send +=_valueToCosmStream4;
      string_to_send += ",";
      string_to_send +=_valueToCosmStream5;
      string_to_send += ",";
      string_to_send +=_valueToCosmStream6;
      string_to_send.trim();
      content_length = string_to_send.length();
      _cosmclient.print("PUT /api/");
      _cosmclient.print(_cosmFeedID);
      _cosmclient.print(".csv HTTP/1.1\nHost: pachube.com\nX-PachubeApiKey: ");
      _cosmclient.print(_cosmApiKey);
      _cosmclient.print("\nUser-Agent: Arduino (Pachube controller v1.1)");
      _cosmclient.print("\nContent-Type: text/csv\nContent-Length: ");
      _cosmclient.print(content_length);
      _cosmclient.print("\nConnection: close\n\n");
      _cosmclient.print(string_to_send);
      _cosmclient.print("\n");
      if(debug){
        Serial.print("Wrote data: ");
        Serial.print(string_to_send);
        Serial.print(" containing ");
        Serial.print(content_length);
        Serial.print(" characters to feed with ID: ");
        Serial.print(_cosmFeedID);
        Serial.println(" on Cosm ");
      } 
      ready_to_update = false;
      reading_pachube = true;
      request_pause = false;
    } 
    else { //Connection faild restart Ethernet
      ++failures;
      if (debug){
        Serial.print("Connection failed nbrtimes: ");
        Serial.println(failures);
      }
      ready_to_update = false;
      reading_pachube = false;
      request_pause = true;
      last_connect = millis();
      if (debug){
        Serial.print("Failed to conect last_connect: ");
        Serial.println(last_connect);
      }
      setupEthernet();
    }
  }
  
/*Read this locks the processor for some time*/
  while (reading_pachube){
    while (_cosmclient.available()) {
      checkForResponse();  //Check and read
    } 
    if (!_cosmclient.connected()) {
      disconnect_cosm();
    }
  }
}

/*Method reding response on upload and reading command from cosm*/
  void CosmK3::checkForResponse(){ 
    char c = _cosmclient.read();
    //moved unsigned int command_tail = strlen("HTTP/1.1 200 OK")+2;
    if (debugVerbose){
      Serial.print(c);
    } 
    buff[pointer] = c; //read into buffer
    if (pointer > 63){
      Serial.print("Buffer size: ");
      Serial.println(pointer);
    }
    if (pointer < (sizeof(buff)-1))
      {
        pointer++;
      }
    //OK have we read a line?
    if (c == '\n') {
      found = strstr(buff, "HTTP/1.1 200 OK");
      char *found2 = strstr(buff, "500 Internal Server Error");
      if (found2 != NULL){
        ++failures;
        Serial.println("500 Internal Server Error");
      }
      
      //Did we receive a command line with data and ending with HTTP/1.1 200 OK
      if ((found != NULL)&&(pointer > command_tail)){
          if(debug){
            Serial.print("Read csvline = ");
            Serial.print(buff);
          }
      //Parse the csv line for command
        char * pch;
        pch = strtok (buff,",");
        int i =0; 
        while (pch != NULL)
        {
          if (i==3){ //Datastream  with id3 is the one that contains the
              commandToExecute = (int)atof(pch);;
              if (last_command != commandToExecute&&commandToExecute>0){ //is it a new command
                last_command = commandToExecute;
              }else{
                commandToExecute = 0;
              }
          }   
          pch = strtok (NULL, ","); 
          i++;
        }     
        //EndParseCSV
      }
      //Did Upload work then we received a line with "HTTP/1.1 200 OK"
      if ((found != 0)&&(pointer == command_tail)){ //Did the upload work?
          ++successes;
             if (successes>maxSuccesses){
             successes = 0;
             Ethernet.maintain();//renews IP
          } 
        if(debug){
         Serial.print("Successful updates = ");
         Serial.println(successes);
         Serial.print("Failiures = ");
         Serial.println(failures);
        }
      }
      clean_buffer(); //clean the buffer beforn reading an new line
    }
  }  
  
  /*Disconnect and clean up*/
  void CosmK3::disconnect_cosm(){
     if (debug){
          Serial.print("Disconnecting: ");
          connection_duration = abs(millis()-(last_connect+_updateInterval));
          Serial.print(connection_duration);
          Serial.println(" millis connection time now disconnecting.\n=====");
        }
    clean_buffer();
    ready_to_update = false;
    reading_pachube = false;
    request_pause = true;
    last_connect = millis();
    _cosmclient.stop();
  }

/*clean buffers*/  
void CosmK3::clean_buffer() {
    pointer = 0;
    memset(buff,'\0',sizeof(buff)); 
}



int CosmK3::getCommandFromCosmStream3(){
 return commandToExecute;
}


void CosmK3::sendStatusToCosmStream2(int arduinoStatus){
    _arduinoStatus = arduinoStatus;
}

void CosmK3::sendValueToCosmStream4(double value){
  _valueToCosmStream4 = doubleToString(value);
}

void CosmK3::sendValueToCosmStream5(double value){
  _valueToCosmStream5 = doubleToString(value);
}

void CosmK3::sendValueToCosmStream6(double value){
  _valueToCosmStream6 = doubleToString(value);
}

void CosmK3::sendValueToCosmStream4(int value){
  _valueToCosmStream4 = String(value);
}

void CosmK3::sendValueToCosmStream5(int value){
  _valueToCosmStream5 = String(value);
}

void CosmK3::sendValueToCosmStream6(int value){
  _valueToCosmStream6 = String(value);
}

//Funkar inte!!!s
String CosmK3::doubleToString(double input){
  char tmp[15];
  //return dtostrf(input,8,2, &tmp[12]);
  String s = dtostrf(input,8,2, tmp);
  s.trim();
  return s;
}

void CosmK3::setDebug(boolean d){
  debug = d;
}
