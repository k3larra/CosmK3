Arduino to Cosm and back versin 1.1
---
   CosmK3 - Library for connection arduino to cosm.
   Created by Lars Holmberg, september, 2012.
   Released into the public domain.

How to use the example
---
 * Create a Cosm feed and enter Feed_ID, Cosm API_KEY and MACAddress in the code.
 * Run Android code and check via Serial Monitor that you get an IP-address and that the values are written to Cosm.
 * Change value on datastream 3 on COSM and check that Arduino reads and resets the value.

About the program
---
 * This code reads and writes data from a Cosm feed, it uses 6 datastreams on Cosm 
 * Datastream 0 counts and writes number of successful updates from Arduino to Cosm
 * Datastream 1 counts number of failure while updating Cosm from Arduino 
 * Datastream 2 is written from Arduino to Cosm and used for sending Arduino status to Cosm 
 * Datastream 3 is read by Arduino from Cosm and is intended to use for sending commands from Cosm to Arduino, The command is reset on cosm i every reading
 * Datastream 4 is written from Arduino to Cosm and used for sending sensor data to Cosm
 * Datastream 5 is written from Arduino to Cosm and used for sending sensor data to Cosm

 Tested with Arduino UNO,Duemillanove and Arduino 1.0.1
  This code, is tested with the official Arduino Ethernet shield.
 v1.1 rewrote reading command so it always clears commands (sets to 0) every time its read from cosm.
	reading intervall changed to 5 seconds.	 
v1.0 rewrote most of Cosm functions cleand out unesesary code ad converted to library

Implemented Methods
---
  Constructors
  CosmK3(char cosmApiKey[], uint32_t cosmFeedID, byte arduinoMAC[] ); 
  CosmK3(char cosmApiKey[],uint32_t cosmFeedID, byte arduinoMAC[] , IPAddress arduinoStaticIP);
  Methods	
    IPAddress getDynamicIP(); /Retreives IP adress frpm socket
    void setupEthernet(); //Create socket
    void readAndWrite(); //Runs with UpdateInterval without respect to how often it is called.
    void setUpdateInterval(unsigned long seconds);
    void sendStatusToCosmStream2(int arduinoStatus);
    int getCommandFromCosmStream3();
    void clearCommandFromCosm(); //Sets stream 3 to 0,
    void sendValueToCosmStream4(double value);
    void sendValueToCosmStream5(double value);
    void sendValueToCosmStream6(double value);
    void sendValueToCosmStream4(int value);
    void sendValueToCosmStream5(int value);
    void sendValueToCosmStream6(int value);
    void setDebug(boolean b);

Bugs:
---
Not stable for more than 6 - 12 hours....

 