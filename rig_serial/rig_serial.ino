#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MCP3008.h>

struct serial_packet
{
  int32_t serial_start;
  float serial_angle[4];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} packet_to_send;

//Global Resistor Constants
const float VCC = 4.28; //5V line voltage

//Flex Resistor Constants
const float R_DIV_FLEX = 39000; //Divider resistance
const float STRAIGHT_R_1 = 21450; //Index
const float NINETY_DEG_R_1 = 57500;
const float STRAIGHT_R_2 = 15000; //Middle
const float NINETY_DEG_R_2 = 40800;
const float STRAIGHT_R_3 = 14450; //Ring
const float NINETY_DEG_R_3 = 43750;
const float STRAIGHT_R_4 = 17150; //Little
const float NINETY_DEG_R_4 = 49500;

//Force Resistor Constants
const float R_DIV_PRESSURE = 3300;

const int32_t START_VAL = -1386103603;

Adafruit_BNO055 bno = Adafruit_BNO055(55);
MCP3008 adc(13, 11, 12, 10);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS);
}

void loop()
{ 
  packet_to_send.serial_start = START_VAL;
  
  //FLEX SENSORS
  //Index
  int flexADC = analogRead(A0);
  
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  packet_to_send.serial_angle[0] = map(flexR, STRAIGHT_R_1, NINETY_DEG_R_1, 0, 90.0);

  //Middle
  flexADC = analogRead(A1);
  
  flexV = flexADC * VCC / 1023.0;
  flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  packet_to_send.serial_angle[1] = map(flexR, STRAIGHT_R_2, NINETY_DEG_R_2, 0, 90.0);

  //Ring
  flexADC = adc.readADC(7);
  
  flexV = flexADC * VCC / 1023.0;
  flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  packet_to_send.serial_angle[2] = map(flexR, STRAIGHT_R_3, NINETY_DEG_R_3, 0, 90.0);

  //Little
  flexADC = adc.readADC(6);
  
  flexV = flexADC * VCC / 1023.0;
  flexR = R_DIV_FLEX * (VCC / flexV - 1.0);
  
  packet_to_send.serial_angle[3] = map(flexR, STRAIGHT_R_4, NINETY_DEG_R_4, 0, 90.0);

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

  packet_to_send.serial_pressure[0] = force;  

  //Palm 1
  fsrADC = adc.readADC(4);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  packet_to_send.serial_pressure[1] = force;  
  
  //Palm 2
  fsrADC = adc.readADC(3);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  packet_to_send.serial_pressure[2] = force;  
  
  //Palm 3
  fsrADC = adc.readADC(2);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  packet_to_send.serial_pressure[3] = force;  
  
  //Little-most
  fsrADC = adc.readADC(1);
  
  fsrV = fsrADC * VCC / 1023.0;
  fsrR = R_DIV_PRESSURE * (VCC / fsrV - 1.0);
  
  fsrG = 1.0 / fsrR;
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
    
  packet_to_send.serial_pressure[4] = force;  

  //IMU
  imu::Vector<3> orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  
  packet_to_send.serial_orientation[0] = orientation.x(); //x
  packet_to_send.serial_orientation[1] = orientation.y(); //y
  packet_to_send.serial_orientation[2] = orientation.z(); //z

  //TEMPERATURE
  packet_to_send.serial_temp = bno.getTemp(); //tempurature
  
  Serial.write((byte*)&packet_to_send, sizeof(serial_packet)); //sends packet

  delay(30); //~33.3hz sampling rate
}
