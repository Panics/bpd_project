
#include "OscHandler.h"
#include <ArduinoOSCWiFi.h>
#include <DebugLog.h>

// ------------------------------------------------------------------------------------------------
OscHandler::OscHandler() : _isInitialized(false), _destinationPort(2323), _receivePort(2323), _destinationIP(192, 168, 1, 254) {

}

// ------------------------------------------------------------------------------------------------
void OscHandler::PrintStatus(){
  LOG_INFO("OscHandler Status:");

  String msg = "Local IP address is: ";
  msg += WiFi.localIP().toString();
  LOG_INFO(msg);  

  msg = "Destination ip is: " + _destinationIP.toString();
  LOG_INFO(msg);

  msg = "Destination port is: " + String(_destinationPort);
  LOG_INFO(msg);

  msg = "Receive port is: " + String(_receivePort);
  LOG_INFO(msg);
}

// ------------------------------------------------------------------------------------------------
bool OscHandler::IsInitialized(){
  return _isInitialized;
}

// ------------------------------------------------------------------------------------------------
bool OscHandler::Initialize(unsigned int destinationPort, IPAddress destinationIP, unsigned int receivePort, String receiveAddress) { 
  _receivePort = receivePort;
  _destinationPort = destinationPort;
  _destinationIP = destinationIP;
  
  PrintStatus();
  
  OscWiFi.subscribe(_receivePort, receiveAddress.c_str(), [](const OscMessage& m) {
  String msg = "OSC Message Received: ";
  msg += m.remoteIP();
  msg += " ";
  msg += m.remotePort();
  msg += " ";
  msg += m.size();
  msg += " ";
  msg += m.address();
  LOG_INFO(msg);
});
 
//  OscWiFi.publish(_destinationIP.toString(), _destinationPort, sendAddress.c_str(), &millis, &micros)
//        ->setIntervalMsec(500.f);

  _isInitialized=true;
  return IsInitialized();
}

// ------------------------------------------------------------------------------------------------
void OscHandler::Send(float x, float y, float z, String sendAddress){
  if(!IsInitialized()){
    return;
  }
  OscWiFi.send(_destinationIP.toString(), _destinationPort, sendAddress.c_str(), x, y, z);
}

// ------------------------------------------------------------------------------------------------
void OscHandler::Send(float x, float y, String sendAddress){
  if(!IsInitialized()){
    return;
  }
  OscWiFi.send(_destinationIP.toString(), _destinationPort, sendAddress.c_str(), x, y);
}

// ------------------------------------------------------------------------------------------------
void OscHandler::Send(float x, String sendAddress){
  if(!IsInitialized()){
    return;
  }
  OscWiFi.send(_destinationIP.toString(), _destinationPort, sendAddress.c_str(), x);
}

// ------------------------------------------------------------------------------------------------
void OscHandler::Update(){
  OscWiFi.update();
}

    
