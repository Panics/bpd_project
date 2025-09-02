#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>

// Needed
#include <Adafruit_Sensor.h>

// #define DEBUGLOG_DISABLE_LOG
#include <DebugLog.h>

// Not sure
#include <Adafruit_GFX.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>

#include "SharedPtr.h"
#include "WifiHandler.h"
#include "OscHandler.h"
#include "SensorHandler.h"

typedef SharedPtr<WifiHandler> WifiHandlerPtr;
typedef SharedPtr<OscHandler> OscHandlerPtr;
typedef SharedPtr<SensorHandler> SensorHandlerPtr;

enum VibrotactorActivationMode {
  ANGULAR_VELOCITY,
  RELATIVE_ANGLE
};

#endif