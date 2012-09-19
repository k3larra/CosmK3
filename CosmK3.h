#ifndef CosmK3_h
#define CosmK3_h
#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <Udp.h>

/*
   CosmK3.h - Library for connecting Arduino with Cosm.
   Created by Lars Holmberg, Lars.Holmberg@mah.se , September, 2012.
   Released into the public domain.
   Buggs Why can't I use IPaddress as type
 */
class CosmK3
{
  public:
    CosmK3(char cosmApiKey[], uint32_t cosmFeedID, byte arduinoMAC[] ); 
    CosmK3(char cosmApiKey[],uint32_t cosmFeedID, byte arduinoMAC[] , IPAddress arduinoStaticIP);
    IPAddress getDynamicIP();
    void setupEthernet();
    void readAndWrite();
    void setUpdateInterval(unsigned long seconds);
    void sendStatusToCosmStream2(int arduinoStatus);
    int getCommandFromCosmStream3();
    //float
    void sendValueToCosmStream4(double value);
    void sendValueToCosmStream5(double value);
    void sendValueToCosmStream6(double value);
    //int
    void sendValueToCosmStream4(int value);
    void sendValueToCosmStream5(int value);
    void sendValueToCosmStream6(int value);
    //misc
    void setDebug(boolean b);
    
    //private
    private:
    void checkForResponse();
    void disconnect_cosm();
    void clean_buffer();
    String doubleToString(double input);
    uint32_t _cosmFeedID;
    char *_cosmApiKey;
    byte *_arduinoMAC;
    IPAddress _arduinoStaticIP;
    IPAddress _arduinoDynamicIP;
    unsigned long _updateInterval;
    
    
    void init();    
    //internal
    boolean useDHCP;
    
};
#endif  
