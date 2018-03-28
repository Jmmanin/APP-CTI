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

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
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
  packet_to_send.serial_orientation[0] = 0; //x
  packet_to_send.serial_orientation[1] = 0; //y
  packet_to_send.serial_orientation[2] = 0; //z
  packet_to_send.serial_temp = 666; //tempurature
  
  Serial1.write((byte*)&packet_to_send, sizeof(serial_packet));
}
