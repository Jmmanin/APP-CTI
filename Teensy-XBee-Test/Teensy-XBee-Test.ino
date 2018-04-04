#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MCP3008.h>

struct serial_packet
{
  int32_t serial_start;
  float serial_angle[5];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} packet_to_send;

const int32_t start_val = -1386103603;
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
  /*
   * INSERT ADC MAPS HERE!!! 
   */
  
  imu::Vector<3> orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  
  packet_to_send.serial_start = start_val;
  packet_to_send.serial_angle[0] = adc.readADC(0); //thumb angle
  packet_to_send.serial_angle[1] = adc.readADC(1); //index angle
  packet_to_send.serial_angle[2] = adc.readADC(2); //middle angle
  packet_to_send.serial_angle[3] = adc.readADC(3); //ring angle
  packet_to_send.serial_angle[4] = adc.readADC(4); //little angle
  packet_to_send.serial_pressure[0] = adc.readADC(5); //thumb pressure
  packet_to_send.serial_pressure[1] = adc.readADC(6); //index pressure
  packet_to_send.serial_pressure[2] = adc.readADC(7); //middle pressure
  packet_to_send.serial_pressure[3] = analogRead(A0); //ring pressure
  packet_to_send.serial_pressure[4] = analogRead(A1); //little pressure
  packet_to_send.serial_orientation[0] = orientation.x(); //x
  packet_to_send.serial_orientation[1] = orientation.y(); //y
  packet_to_send.serial_orientation[2] = orientation.z(); //z
  packet_to_send.serial_temp = bno.getTemp(); //tempurature
  
  Serial.write((byte*)&packet_to_send, sizeof(serial_packet));

  delay(10);
}
