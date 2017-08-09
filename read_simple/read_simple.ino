#include <SCServo.h>

SCServo SERVO;
int LEDpin = 13;
int pos[9];

void setup() {
								Serial.begin(19200);
								Serial2.begin(1000000);// Serial port for SCServos TX0, RX0
								Serial1.begin(9600);// Serial port for bluetooth TX1, RX1
								SERVO.pSerial = &Serial2;
								delay(500);
// Enable servos torques
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

void loop() {
								for (size_t i = 1; i <= 9; i++) {
																pos[i] = SERVO.ReadPos(i);
																Serial.print(i);
																Serial.print(": ");
																Serial.println(pos[i]);
								}
								delay(5000);
								Serial.println();
}
