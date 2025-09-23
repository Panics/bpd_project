#include "WifiHandler.h"
#include <DebugLog.h>

// ------------------------------------------------------------------------------------------------
WifiHandler::WifiHandler():_isInitialized(false){

}

// ------------------------------------------------------------------------------------------------
bool WifiHandler::Initialize(String ssid, String password){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    LOG_ERROR("Communication with WiFi module failed!");
    return false;
  }

  unsigned int numRetries = 3;
  for(int i=0; i<numRetries; i++){
    String msg = String("Attempting to connect to WPA SSID: ") + ssid;
    LOG_INFO(msg);
    if(WiFi.begin(ssid.c_str(), password.c_str()) == WL_CONNECTED){
      _isInitialized=true;
      PrintStatus();
      break;
    }
    delay(3000);
  }
  if(!IsInitialized()){
    LOG_ERROR("Could not connect to WiFi");
  }
  return IsInitialized();
}

// ------------------------------------------------------------------------------------------------
void WifiHandler::PrintStatus(){
    LOG_INFO("Wifi Status:");
    _printCurrentNet();
    _printWifiData();
}

// ------------------------------------------------------------------------------------------------
bool WifiHandler::IsInitialized(){
  return _isInitialized;
}

// ------------------------------------------------------------------------------------------------
WiFiClient WifiHandler::GetWifiClient(){
  return _wifiClient;
}

// ------------------------------------------------------------------------------------------------
void WifiHandler::_printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  LOG_INFO("IP Address: " + ip.toString());
  
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  LOG_INFO(String("MAC address: ") + _macAddressToString(mac));
}

// ------------------------------------------------------------------------------------------------
void WifiHandler::_printCurrentNet() {
  // print the SSID of the network you're attached to:
  LOG_INFO("SSID: " + String(WiFi.SSID()));

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  LOG_INFO("BSSID: " + _macAddressToString(bssid));

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  LOG_INFO("signal strength (RSSI): " + String(rssi));

/*
  // Print the encryption type. This code is commented out because it takes a relative long time to run
  byte encryption = WiFi.encryptionType();
  char hexString[3] = {0,0,0}; // 2 characters for hex, 1 for null terminator
  sprintf(hexString, "%02X", encryption); // "%02X" formats as 2 uppercase hex digits, padded with leading zero if needed
  LOG_INFO(String("Encryption Type: ") + String(hexString));
*/
}

// ------------------------------------------------------------------------------------------------
String WifiHandler::_macAddressToString(byte mac[]) {
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3] = {0,0,0}; // Buffer for two hex digits + null terminator
    sprintf(buf, "%02X", mac[i]); // Format byte as two uppercase hex digits
    s += buf;
    if (i < 5) {
      s += ':'; // Add colon separator
    }
  }
  return s;
}