#ifndef _WIFITEST_
#define _WIFITEST_

#include <Arduino.h>
#include <WiFiS3.h>

// ------------------------------------------------------------------------------------------------
class WifiHandler {
  
  public:
    WifiHandler(); 

    bool IsInitialized();
    bool Initialize(String ssid, String password);     
    WiFiClient GetWifiClient();
    void PrintStatus();

  private:
    bool _isInitialized;
    WiFiClient _wifiClient;
    String _macAddressToString(byte mac[]);
    void _printCurrentNet();
    void _printWifiData();
};

#endif
