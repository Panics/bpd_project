#ifndef _SENSORHANDLER_
#define _SENSOEHANDLER_

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include "SharedPtr.h"

// ------------------------------------------------------------------------------------------------
struct SensorData {
  float angleXInitial;
  float angleYInitial;
  float angleZInitial;
  
  float angleXAbsolute;
  float angleYAbsolute;
  float angleZAbsolute;
  
  float accX;
  float accY;
  float accZ;

  float accAngleX;
  float accAngleY;

  float gyroX;
  float gyroY;
  float gyroZ;
    
  float temp;

  float angleXRelative() { return angleXAbsolute - angleXInitial; }
  float angleYRelative() { return angleYAbsolute - angleYInitial; }
  float angleZRelative() { return angleZAbsolute - angleZInitial; }
};

// ------------------------------------------------------------------------------------------------
class SensorHandler {
  
  public:
    SensorHandler(); 

    SensorData GetSensorData();
    void Update();
    bool IsInitialized();
    bool Initialize();

    void PrintStatus();

  private:
    SensorData _sensorData;
    MPU6050 _mpu;
    bool _isInitialized;
};

typedef SharedPtr<SensorHandler> SensorHandlerPtr;

#endif
