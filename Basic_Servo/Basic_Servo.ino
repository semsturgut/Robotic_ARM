#include <SCServo.h>

SCServo SERVO;

char b_receive;

void baseWritePos(int , int);
void clawWritePos(int , int);

void setup()
{
  Serial.begin(1000000);
  SERVO.pSerial = &Serial;
  delay(500);
  SERVO.EnableTorque(1, 1);
  SERVO.EnableTorque(2, 1);
  SERVO.EnableTorque(3, 1);
  SERVO.EnableTorque(4, 1);
  SERVO.EnableTorque(5, 1);
  SERVO.EnableTorque(6, 1);
  SERVO.EnableTorque(7, 1);
  SERVO.EnableTorque(8, 1);
  SERVO.EnableTorque(9, 1);
}

void loop()
{
  int speed_delay = 1000;
  baseWritePos(660, speed_delay);
  SERVO.WritePos(3, 510, speed_delay);// Servo ID:1, rotate to the position:0x2FF
  SERVO.WritePos(4, 600, speed_delay);// Servo ID:1, rotate to the position:0x2FF
  SERVO.WritePos(5, 550, speed_delay);// Servo ID:1, rotate to the position:0x2FF
  SERVO.WritePos(6, 560, speed_delay);// Servo ID:1, rotate to the position:0x2FF
  SERVO.WritePos(7, 250, speed_delay);// Servo ID:1, rotate to the position:0x2FF
  delay(1500);
}

void baseWritePos(int pos, int time_ms) {
  SERVO.WritePos(1, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
  SERVO.WritePos(2, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
}

void clawWritePos(int pos, int time_ms) {
  SERVO.WritePos(8, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
  SERVO.WritePos(9, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
}

