#include "SensorHandler.h"
#include <DebugLog.h>

// ------------------------------------------------------------------------------------------------
SensorHandler::SensorHandler():_isInitialized(false), _mpu(Wire), _sensorData() {

}

// ------------------------------------------------------------------------------------------------
bool SensorHandler::IsInitialized(){
  return _isInitialized;
}

// ------------------------------------------------------------------------------------------------
void SensorHandler::Update(){
  _mpu.update();

  _sensorData.angleXAbsolute = _mpu.getAngleX();
  _sensorData.angleYAbsolute = _mpu.getAngleY();
  _sensorData.angleZAbsolute = _mpu.getAngleZ();
  
  _sensorData.accX = _mpu.getAccX();
  _sensorData.accY = _mpu.getAccY();
  _sensorData.accZ = _mpu.getAccZ();

  _sensorData.accAngleX = _mpu.getAccAngleX();
  _sensorData.accAngleY = _mpu.getAccAngleY();

  _sensorData.gyroX = _mpu.getGyroX();
  _sensorData.gyroY = _mpu.getGyroY();
  _sensorData.gyroZ = _mpu.getGyroZ();
    
  _sensorData.temp = _mpu.getTemp();
}

// ------------------------------------------------------------------------------------------------
bool SensorHandler::Initialize(){
  LOG_INFO("Setting up IMU, don't move");
  Wire.begin();
  _mpu.begin();
  _mpu.calcOffsets(true, true);

  // store initial values for angles
  _sensorData.angleXInitial = _mpu.getAngleX();      
  _sensorData.angleYInitial = _mpu.getAngleY();
  _sensorData.angleZInitial = _mpu.getAngleZ();

  _isInitialized = true;

  PrintStatus();

  return IsInitialized();
}

// ------------------------------------------------------------------------------------------------
SensorData SensorHandler::GetSensorData(){
  return _sensorData;
}

// ------------------------------------------------------------------------------------------------
void SensorHandler::PrintStatus(){
  String msg = "IMU Status:";
  LOG_INFO(msg);
  msg = "Initial Angle: " + String(_sensorData.angleXInitial) + ", " + String(_sensorData.angleYInitial)  + ", " + String(_sensorData.angleZInitial);
  LOG_INFO(msg);
  msg = "Angle: " + String(_sensorData.angleXAbsolute) + ", "+ String(_sensorData.angleYAbsolute) + ", "+ String(_sensorData.angleZAbsolute);
  LOG_INFO(msg);
  msg = "Acc: " + String(_sensorData.accX) + ", "+ String(_sensorData.accY) + ", "+ String(_sensorData.accZ);
  LOG_INFO(msg);
  msg = "AccAngle: " + String(_sensorData.accAngleX) + ", "+ String(_sensorData.accAngleY);
  LOG_INFO(msg);
  msg = "Gyro: " + String(_sensorData.gyroX) + ", "+ String(_sensorData.gyroY) + ", "+ String(_sensorData.gyroZ);
  LOG_INFO(msg);
}
