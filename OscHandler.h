#ifndef _OSCHANDLER_
#define _OSCHANDLER_

#include <Arduino.h>
#include <ArduinoOSCWiFi.h>

// ------------------------------------------------------------------------------------------------
class OscHandler {
  
  public:
    OscHandler(); 

    bool Initialize(unsigned int destinationPort, IPAddress destinationIP, unsigned int receivePort, String receiveAddress);     
    static void Update();
    bool IsInitialized();

    void Send(float x,  String sendAddress);
    void Send(float x, float y, String sendAddress);
    void Send(float x, float y, float z, String sendAddress);

    void PrintStatus();

  private:
    bool _isInitialized;
    unsigned int _receivePort;
    unsigned int _destinationPort;
    IPAddress _destinationIP;
};

#endif
