#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MCP3008.h>

//***Values may be inacurate due to changes in actual VCC***
//Global Resistor Constants
const float VCC = 4.28; //5V line voltage

//Flex Resistor Constants
const float R_DIV_FLEX = 39000; //Divider resistance
const float STRAIGHT_R_1 = 21450; //Index
const float NINETY_DEG_R_1 = 57500;
const float STRAIGHT_R_2 = 15000; //Middle
const float NINETY_DEG_R_2 = 40800;
const float STRAIGHT_R_3 = 14450; //Ring
const float NINETY_DEG_R_3 = 37200;
const float STRAIGHT_R_4 = 17150; //Little
const float NINETY_DEG_R_4 = 43400;

//Force Resistor Constants
const float R_DIV_PRESSURE = 3300;

Adafruit_BNO055 bno = Adafruit_BNO055(55);
MCP3008 adc(13, 11, 12, 10);

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  Wire.begin();
  bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS); //Turns off magnetometer
}

void loop()
{
  //FLEX SENSOR
  //Index
  int flexADC = analogRead(A0);
  
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  float angle = map(flexR, STRAIGHT_R_1, NINETY_DEG_R_1, 0, 90.0);

  Serial.println("===========================================");
  Serial.println("Bend Index: " + String(angle) + " degrees " + "Resistance: " + flexR + " Ohms");

  //Middle
  flexADC = analogRead(A1);
  
  flexV = flexADC * VCC / 1023.0;
  flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  angle = map(flexR, STRAIGHT_R_2, NINETY_DEG_R_2, 0, 90.0);

  Serial.println("Bend Middle: " + String(angle) + " degrees " + "Resistance: " + flexR + " Ohms");

  //Ring
  flexADC = adc.readADC(7);
  
  flexV = flexADC * VCC / 1023.0;
  flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  angle = map(flexR, STRAIGHT_R_3, NINETY_DEG_R_3, 0, 90.0);

  Serial.println("Bend Ring: " + String(angle) + " degrees " + "Resistance: " + flexR + " Ohms");

  //Little
  flexADC = adc.readADC(6);
  
  flexV = flexADC * VCC / 1023.0;
  flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  angle = map(flexR, STRAIGHT_R_4, NINETY_DEG_R_4, 0, 90.0);

  Serial.println("Bend Little: " + String(angle) + " degrees " + "Resistance: " + flexR + " Ohms");
  Serial.println();

  //PRESSURE SENSOR
  //Thumb-most
  int fsrADC = adc.readADC(0);
  
  float fsrV = fsrADC * VCC / 1023.0;
  float fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  float force;
  float fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  Serial.println("Force Thumb-most: " + String(force) + " g");

  //Palm 1
  fsrADC = adc.readADC(4);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  Serial.println("Force Palm 1: " + String(force) + " g");
  
  //Palm 2
  fsrADC = adc.readADC(3);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  Serial.println("Force Palm 2: " + String(force) + " g");
  
  //Palm 3
  fsrADC = adc.readADC(2);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  Serial.println("Force Palm 3: " + String(force) + " g");
  
  //Little-most
  fsrADC = adc.readADC(1);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  Serial.println("Force Little-most: " + String(force) + " g");
  
  //IMU
  imu::Vector<3> orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  Serial.print("X: ");
  Serial.print(orientation.x());
  Serial.print(" Y: ");
  Serial.print(orientation.y());
  Serial.print(" Z: ");
  Serial.print(orientation.z());
  Serial.println();
  Serial.println();

  //Temperature
  int8_t temp = bno.getTemp();

  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("===========================================");

  delay(1000);
}
