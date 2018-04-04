#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

struct serial_packet
{
  int32_t serial_start;
  float serial_angle[5];
  float serial_pressure[5];
  float serial_orientation[3];
  int32_t serial_temp;
} packet_to_send;

struct test_packet
{
  char value;
} test_to_send;

const int32_t start_val = -1386103603;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS);
}

void loop()
{
  imu::Vector<3> orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  
  packet_to_send.serial_start = start_val;
  packet_to_send.serial_angle[0] = 0; //thumb angle
  packet_to_send.serial_angle[1] = 0; //index angle
  packet_to_send.serial_angle[2] = 0; //middle angle
  packet_to_send.serial_angle[3] = 0; //ring angle
  packet_to_send.serial_angle[4] = 0; //little angle
  packet_to_send.serial_pressure[0] = 0; //thumb pressure
  packet_to_send.serial_pressure[1] = 0; //index pressure
  packet_to_send.serial_pressure[2] = 0; //middle pressure
  packet_to_send.serial_pressure[3] = 0; //ring pressure
  packet_to_send.serial_pressure[4] = 0; //little pressure
  packet_to_send.serial_orientation[0] = orientation.x(); //x
  packet_to_send.serial_orientation[1] = orientation.y(); //y
  packet_to_send.serial_orientation[2] = orientation.z(); //z
  packet_to_send.serial_temp = 0; //tempurature
  
  Serial.write((byte*)&packet_to_send, sizeof(serial_packet));
}
