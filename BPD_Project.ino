// Getting started with Arduino IDE
// ================================
//
// If not already installed, install the following libraries using Arduino IDE Library Manager (Menu Tools->Manage Libraries)
//
// Adafruit Unified Sensor
// Adafruit GFX Library
// MPU6050_light
// ArduinoGraphics
// ArduinoOSC
//
// Then, install support for Arduino UNO R4 WiFi. Connect your Arduino board via USB, and select it from the drop down in the top left of the Arduino IDE
//
// Then, modify the contents of the file 'arduino_secrets.h' with the appropriate WiFi SSID and password
//
// Then, just compile and upload the project to the connected arduino


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
// The digital ports of the vibrotactors
#define X_PIN                       0
#define Y_PIN                       1
#define X_PIN_BEHIND                2
#define Y_PIN_BEHIND                3

// Intervals for sending OSC data, or printing status information
#define TRANSMISSION_INTERVAL_MS    100
#define DEBUG_PRINT_INTERVAL_MS     10000

// Threshold values used for vibrotactor activation calculation
#define THRESHOLD_DEGREES           20
#define THRESHOLD_VELOCITY          10

// OSC configuration
#define OSCDESTINATIONIP            IPAddress(255,255,255,255)
#define OSCDESTINATIONPORT          2323
#define OSCRECEIVEPORT              2323
#define OSCSENDADDRESSANGLE         "/Brandeis/BPD/SendAngle"
#define OSCSENDADDRESSGYRO          "/Brandeis/BPD/SendGyro"
#define OSCSENDADDRESSACCELANGLE    "/Brandeis/BPD/SendAccelAngle"
#define OSCSENDADDRESSACCEL         "/Brandeis/BPD/SendAccel"
#define OSCSENDADDRESSTEMP          "/Brandeis/BPD/SendTemp"
#define OSCRECEIVEADDRESS           "/Brandeis/BPD/Receive"

// Debug output configuration
// #define DEBUGLOG_DISABLE_LOG

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
// arduino includes
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DebugLog.h>
#include <Adafruit_GFX.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
// project includes
#include "arduino_secrets.h"
#include "WifiHandler.h"
#include "OscHandler.h"
#include "SensorHandler.h"

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
// globally defined ojects and variables

// For accessing the LED matrix
ArduinoLEDMatrix gLedMatrix;
// For accessing wifi
WifiHandlerPtr gWifiHandler;
// For accessing OSC
OscHandlerPtr gOscHandler;
// For accessing IMU
SensorHandlerPtr gSensorHandler;

// Holds msec of when last a given activity took place
unsigned long gLastOscMessageSendMillis = 0;
unsigned long gLastDebugPrintMillis = 0;

// frame for displaying on LED matrix. See https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix/
const uint32_t gLedMatrixFrame_Happy[] = {
    0x19819,
    0x80000001,
    0x81f8000
};

// frame for displaying on LED matrix. See https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix/
const uint32_t gLedMatrixFrame_Heart[] = {
    0x3184a444,
    0x44042081,
    0x100a0040
};

// Whether to use angular velocity or relative angle to calculate tactor activation
enum VibrotactorActivationMode {
  ANGULAR_VELOCITY,
  RELATIVE_ANGLE
};
VibrotactorActivationMode gVibrotactorActivationMode = RELATIVE_ANGLE;

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void do_not_continue(){
  while(true){}
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void initializeLedMatrix(){
  LOG_INFO("Setting up LED Matrix");
  gLedMatrix.begin();
  gLedMatrix.loadFrame(gLedMatrixFrame_Happy);
  delay(1000);
  gLedMatrix.loadFrame(gLedMatrixFrame_Heart);
  delay(1000);
  gLedMatrix.clear();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void initializeSensor(){
  LOG_INFO("Setting up Sensor");
  gSensorHandler = SensorHandlerPtr(new SensorHandler());
  if(!gSensorHandler->Initialize()){
    do_not_continue();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void initializeWifi(){
  LOG_INFO("Setting up WiFi");
  gWifiHandler = WifiHandlerPtr(new WifiHandler());
  if(!gWifiHandler->Initialize(SECRET_SSID, SECRET_PASS)){
     do_not_continue();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void initializeOsc(){
  LOG_INFO("Setting up Osc");
  gOscHandler = OscHandlerPtr(new OscHandler());
  if(!gOscHandler->Initialize(OSCDESTINATIONPORT, OSCDESTINATIONIP, OSCRECEIVEPORT, OSCRECEIVEADDRESS )){
     do_not_continue();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void initializeVibrotactors(){
  LOG_INFO("Setting up Vibrotactors");

  //
  // Configure I/O pins
  //
  pinMode(X_PIN, OUTPUT);         // assumed to drive vibrotactor
  pinMode(Y_PIN, OUTPUT);         // assumed to drive vibrotactor
  pinMode(X_PIN_BEHIND, OUTPUT);  // assumed to drive vibrotactor
  pinMode(Y_PIN_BEHIND, OUTPUT);  // assumed to drive vibrotactor

  //
  // Activate vibrotactors for 1000 ms to show a sign of life
  //  
  digitalWrite(X_PIN, HIGH);  
  digitalWrite(Y_PIN, HIGH);  
  digitalWrite(X_PIN_BEHIND, HIGH);  
  digitalWrite(Y_PIN_BEHIND, HIGH);  

  delay(1000);

  digitalWrite(X_PIN, LOW);  
  digitalWrite(Y_PIN, LOW);  
  digitalWrite(X_PIN_BEHIND, LOW);  
  digitalWrite(Y_PIN_BEHIND, LOW);  
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void updateVibrotactors(){
  SensorData sensorData = gSensorHandler->GetSensorData();

  bool x_axis_at_threshold = false;
  bool y_axis_at_threshold = false;
  bool z_axis_at_threshold = false;

  bool x_axis_behind_threshold = false;
  bool y_axis_behind_threshold = false;
  bool z_axis_behind_threshold = false;

  // use current sensor data to determine if vibrotactor should be activated
  // the calculation depends on a pre-configured mode
  switch(gVibrotactorActivationMode){
    case RELATIVE_ANGLE:
      x_axis_at_threshold = sensorData.angleXRelative() > THRESHOLD_DEGREES;
      y_axis_at_threshold = sensorData.angleYRelative() > THRESHOLD_DEGREES;
      z_axis_at_threshold = sensorData.angleZRelative() > THRESHOLD_DEGREES;

      x_axis_behind_threshold = sensorData.angleXRelative() < THRESHOLD_DEGREES * -1;
      y_axis_behind_threshold = sensorData.angleYRelative() < THRESHOLD_DEGREES * -1;
      z_axis_behind_threshold = sensorData.angleZRelative() < THRESHOLD_DEGREES * -1;
      break;

    case ANGULAR_VELOCITY:
      x_axis_at_threshold = sensorData.gyroX > THRESHOLD_VELOCITY;
      y_axis_at_threshold = sensorData.gyroY > THRESHOLD_VELOCITY;
      z_axis_at_threshold = sensorData.gyroZ > THRESHOLD_VELOCITY;

      x_axis_behind_threshold = sensorData.gyroX < THRESHOLD_VELOCITY * -1;
      y_axis_behind_threshold = sensorData.gyroY < THRESHOLD_VELOCITY * -1;
      z_axis_behind_threshold = sensorData.gyroZ < THRESHOLD_VELOCITY * -1;

      break;
  }

  //
  // Apply digital outputs, to drive vibrotactors
  //
  digitalWrite(X_PIN, x_axis_at_threshold);  
  digitalWrite(Y_PIN, y_axis_at_threshold);  
  digitalWrite(X_PIN_BEHIND, x_axis_behind_threshold);  
  digitalWrite(Y_PIN_BEHIND, y_axis_behind_threshold);  

  //
  // Log some debug information every DEBUG_PRINT_INTERVAL_MS milliseconds - for now, configured to go to serial 
  // 
  String msg;
  if ((millis() - gLastDebugPrintMillis >= DEBUG_PRINT_INTERVAL_MS)) {
    msg = "Vibrotactor Activation Status:";
    LOG_INFO(msg);

    msg = "X at threshold: " + String(x_axis_at_threshold);
    msg += ", behind threshold: " + String(x_axis_behind_threshold);
    LOG_INFO(msg);

    msg = "Y at threshold: " + String(y_axis_at_threshold);
    msg += ", behind threshold: " + String(y_axis_behind_threshold);
    LOG_INFO(msg);

    msg = "Z at threshold: " + String(z_axis_at_threshold);
    msg += ", behind threshold: " + String(z_axis_behind_threshold);
    LOG_INFO(msg);

    gOscHandler->PrintStatus();
    gWifiHandler->PrintStatus();
    gSensorHandler->PrintStatus();
    gLastDebugPrintMillis = millis();
  }

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup(void) {
  double setup_start_time = millis();

  //
  // Initialize serial connection to host
  //
  Serial.begin(115200);                         

  //
  // Wait for serial interface connection to host machine
  //  
  unsigned long previousMillis=0;
  int max_serial_time=2000;
  while(!Serial){
    unsigned long currentMillis = millis();
    if (millis() - previousMillis>max_serial_time){
      String msg = "Serial Monitor Not Connected after ";
      msg += max_serial_time;
      msg += " msec";
      do_not_continue();
    }
    delay(250);
  }

  String msg = "BPD project\r\n==========";
  LOG_INFO(msg);

  //
  // Initialize / configure vibrotactors
  //
  initializeVibrotactors();
  
  //
  // Initialize sensor
  //
  initializeSensor();

  //
  // Initialize Wifi
  //
  initializeWifi();

  //
  // Initialize Osc
  //
  initializeOsc();

  //
  // Hello (LED) world
  //
  initializeLedMatrix();

  double setup_end_time=millis();
  double setup_elapsed_seconds=(setup_end_time-setup_start_time)/1000;
  msg = "Completed Setup in ";
  msg += setup_elapsed_seconds;
  msg += "\tseconds";
  LOG_INFO(msg);
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendOSCMessages(){
  //
  // Send OSC messaged every TRANSMISSION_INTERVAL_MS milliseconds
  // 
  if (millis() - gLastOscMessageSendMillis >= TRANSMISSION_INTERVAL_MS) {
    //
    // Obtain copy of most current IMU data
    //  
    SensorData sensorData = gSensorHandler->GetSensorData();

    gOscHandler->Send(sensorData.gyroX, sensorData.gyroY, sensorData.gyroZ, OSCSENDADDRESSGYRO);
    gOscHandler->Send(sensorData.angleXAbsolute, sensorData.angleYAbsolute, sensorData.angleZAbsolute, OSCSENDADDRESSANGLE);
    gOscHandler->Send(sensorData.accX, sensorData.accY, sensorData.accZ, OSCSENDADDRESSACCEL);
    gOscHandler->Send(sensorData.accAngleX, sensorData.accAngleY, OSCSENDADDRESSACCELANGLE);
    gOscHandler->Send(sensorData.temp, OSCSENDADDRESSTEMP);
    gLastOscMessageSendMillis = millis();
  }

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void receiveOSCMessages(){


    // TODO: RECEIVE OSC
  
//    messageSize = mqttClient.parseMessage();
    
//    if (messageSize) {
//      Serial.print("received message size:\t");
//      Serial.print(messageSize);
//      Serial.print("\t");
//      message="";
//      while (mqttClient.available()) {
//        character=char((mqttClient.read()));
//        message+=character;
//        Serial.print(character);
//      }
//      Serial.println();
//      split_index=message.indexOf(",");
//      parsed_x_axis_angle=message.substring(0,split_index);
//      parsed_y_axis_angle=message.substring(split_index+1,message.length());
//      x_axis_angle=parsed_x_axis_angle.toFloat();
//      y_axis_angle=parsed_y_axis_angle.toFloat();
  
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  //
  // Update subsystems
  //
  OscHandler::Update();
  gSensorHandler->Update();

  // 
  // Handle sending/receiving OSC
  //
  sendOSCMessages();
  receiveOSCMessages();
  
  //
  // Update vibrotactors. Need OSC receiving to be implemented first
  //
  // updateVibrotactors();
}