#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>

const int FLEX_SENSOR_1 = A0;
const float VCC = 5.03; //5V line voltage
const float R_DIV_FLEX = 39000; //Divider resistance
const float STRAIGHT_R = 13000;
const float NINETY_DEG_R = 35000;

const int PRESSURE_SENSOR_1 = A1;
const float R_DIV_PRESSURE = 3300;

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup()
{
  Serial.begin(9600);
  pinMode(FLEX_SENSOR_1, INPUT);
  Wire.begin();
  bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS); //Turns off magnetometer
}

void loop()
{
  /*FLEX SENSOR*/
  int flexADC = analogRead(FLEX_SENSOR_1);
  
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  float angle = map(flexR, STRAIGHT_R, NINETY_DEG_R, 0, 90.0);
  
  Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();

  /*PRESSURE SENSOR*/
  int fsrADC = analogRead(PRESSURE_SENSOR_1);
  
  float fsrV = fsrADC * VCC / 1023.0;
  float fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  float force;
  float fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  Serial.println("Force: " + String(force) + " g");
  Serial.println();

  /*IMU*/
  imu::Vector<3> orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  Serial.print("X: ");
  Serial.print(orientation.x());
  Serial.print(" Y: ");
  Serial.print(orientation.y());
  Serial.print(" Z: ");
  Serial.print(orientation.z());
  Serial.println();

  delay(500);
}
