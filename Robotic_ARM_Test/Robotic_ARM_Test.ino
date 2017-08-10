#include <SCServo.h>

SCServo SERVO;

char b_receive;
int cur_pos;

void baseWritePos(int, int);
void clawWritePos(int, int);
void setupPosition(int);
void homePosition(int);
void Movement_1(int);
void Movement_2(int);
void Beer(int);

int servo_ID, servo_Position, servo_Duration, last_Position;
String input_String;
int ind1,ind2,ind3;


void setup() {
        // Serial.begin(19200);
        Serial1.begin(9600);// Serial port for bluetooth TX1, RX1
        Serial.begin(1000000);// Serial port for SCServos TX0, RX0
        SERVO.pSerial = &Serial;
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

        setupPosition(1500);// This function is set servos to default positions
}

void loop() {
        if (Serial1.available()) {
                char input_Char = Serial1.read();
                if (input_Char !='#') {
                        input_String += input_Char;
                } else {
                        ind1 = input_String.indexOf(','); // finds location of first ,
                        servo_ID = input_String.substring(0, ind1).toInt(); // captures first data String
                        ind2 = input_String.indexOf(',', ind1+1 ); // finds location of second ,
                        servo_Position = input_String.substring(ind1+1, ind2+1).toInt(); // captures second data String
                        ind3 = input_String.indexOf(',', ind2+1 ); // finds location of third ,
                        servo_Duration = input_String.substring(ind2+1, ind3+1).toInt(); // captires third data String
                        last_Position = SERVO.ReadPos(servo_ID);

                        // Fix here later this code is trash ...
                        // We're getting minus values on servo_Duration variable, find out why!
                        // if(last_Position >= 300 && last_Position <= 700) {
                        //         servo_Duration = 10 * abs(last_Position - servo_Position);
                        switch (servo_ID) {
                        case 1:
                        case 2:            // This case control servo ID: 1 and 2. (Base servo.)
                                if (servo_Position >= 300 && servo_Position <= 700) {
                                        baseWritePos(servo_Position, servo_Duration);
                                        // Serial.println(servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position < 300) {
                                        baseWritePos(300, servo_Duration);
                                        // Serial.println(servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position > 700) {
                                        baseWritePos(700, servo_Duration);
                                        // Serial.println(servo_Duration);
                                        delay(servo_Duration);
                                }
                                break;
                        case 7:            // This case control servo ID: 7. Claw rotation.
                                if (servo_Position >= 140 && servo_Position <= 400) {
                                        SERVO.WritePos(servo_ID, servo_Position, servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position < 140) {
                                        baseWritePos(300, servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position > 400) {
                                        baseWritePos(400, servo_Duration);
                                        delay(servo_Duration);
                                }
                                break;
                        case 8:
                        case 9:            // This case control servo ID: 8 and 9. Claw open and close.
                                if (servo_Position >= 450 && servo_Position <= 600) {
                                        clawWritePos(servo_Position, servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position < 450) {
                                        baseWritePos(450, servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position > 600) {
                                        baseWritePos(600, servo_Duration);
                                        delay(servo_Duration);
                                }
                                break;
                        // Home position
                        case 10:
                                homePosition(servo_Duration);
                                break;
                        // Home position
                        case 11:
                                Movement_1(servo_Duration);
                                break;
                        case 12:
                                Movement_2(servo_Duration);
                                break;
                        case 13:
                                Beer(servo_Duration);
                                break;

                        default:            // This case control rest of the servos.
                                if (servo_Position >= 300 && servo_Position <= 700) {
                                        SERVO.WritePos(servo_ID, servo_Position, servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position < 300) {
                                        baseWritePos(512, servo_Duration);
                                        delay(servo_Duration);
                                } else if (servo_Position > 700) {
                                        baseWritePos(700, servo_Duration);
                                        delay(servo_Duration);
                                }
                                break;
                        }
                        input_String = "";
                        servo_ID = servo_Position = servo_Duration = 0;
                }
        }
}


void baseWritePos(int pos, int time_ms) {
        SERVO.WritePos(1, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
        SERVO.WritePos(2, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
}

void setupPosition(int time_ms) {
        baseWritePos(420, time_ms);
        SERVO.WritePos(3, 500, time_ms);
        SERVO.WritePos(4, 770, time_ms);
        SERVO.WritePos(5, 510, time_ms);
        SERVO.WritePos(6, 710, time_ms);
        SERVO.WritePos(7, 510, time_ms);
        SERVO.WritePos(8, 430, time_ms);
        SERVO.WritePos(9, 430, time_ms);
        delay(time_ms);
}

void clawWritePos(int pos, int time_ms) {
        SERVO.WritePos(8, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
        SERVO.WritePos(9, pos, time_ms);// Servo ID:1, rotate to the position:0x2FF
}

void homePosition(int time_ms) {
        baseWritePos(420, time_ms);
        SERVO.WritePos(3, 500, time_ms);
        SERVO.WritePos(4, 770, time_ms);
        SERVO.WritePos(5, 510, time_ms);
        SERVO.WritePos(6, 710, time_ms);
        SERVO.WritePos(7, 510, time_ms);
        SERVO.WritePos(8, 430, time_ms);
        SERVO.WritePos(9, 430, time_ms);
        delay(time_ms);
}

void Movement_1(int time_ms){
        // Select object
        baseWritePos(650, time_ms);
        SERVO.WritePos(3, 510, time_ms);
        SERVO.WritePos(4, 880, time_ms);
        SERVO.WritePos(5, 510, time_ms);
        SERVO.WritePos(6, 260, time_ms);
        SERVO.WritePos(7, 540, time_ms);
        SERVO.WritePos(8, 500, time_ms);
        SERVO.WritePos(9, 520, time_ms);
        delay(time_ms+1000);
        // Move forward
        baseWritePos(770, time_ms);
        SERVO.WritePos(3, 530, time_ms);
        SERVO.WritePos(4, 640, time_ms);
        SERVO.WritePos(5, 510, time_ms);
        SERVO.WritePos(6, 380, time_ms);
        SERVO.WritePos(7, 520, time_ms);
        SERVO.WritePos(8, 500, time_ms);
        SERVO.WritePos(9, 520, time_ms);
        delay(time_ms+1000);
        // Clutch
        SERVO.WritePos(8, 450, time_ms-200);
        SERVO.WritePos(9, 450, time_ms-200);
        delay(time_ms+1000);
        // Pick up
        baseWritePos(550, time_ms);
        SERVO.WritePos(3, 500, time_ms);
        SERVO.WritePos(4, 510, time_ms);
        SERVO.WritePos(5, 490, time_ms);
        SERVO.WritePos(6, 730, time_ms);
        SERVO.WritePos(7, 540, time_ms);
        SERVO.WritePos(8, 450, time_ms);
        SERVO.WritePos(9, 450, time_ms);
        delay(time_ms+1000);
        // Move
        baseWritePos(670, time_ms);
        SERVO.WritePos(3, 820, time_ms);
        SERVO.WritePos(4, 600, time_ms);
        SERVO.WritePos(5, 490, time_ms);
        SERVO.WritePos(6, 400, time_ms);
        SERVO.WritePos(7, 400, time_ms);
        SERVO.WritePos(8, 450, time_ms);
        SERVO.WritePos(9, 450, time_ms);
        delay(time_ms+1000);
        // Release
        SERVO.WritePos(8, 580, time_ms);
        SERVO.WritePos(9, 580, time_ms);
        delay(time_ms);
}

void Movement_2(int time_ms){
        baseWritePos(480, time_ms);
        SERVO.WritePos(3, 240, time_ms);
        SERVO.WritePos(4, 910, time_ms);
        SERVO.WritePos(5, 480, time_ms);
        SERVO.WritePos(6, 370, time_ms);
        SERVO.WritePos(7, 510, time_ms);
        SERVO.WritePos(8, 500, time_ms);
        SERVO.WritePos(9, 520, time_ms);
        delay(time_ms+1000);
        // Clutch
        SERVO.WritePos(8, 420, time_ms-200);
        SERVO.WritePos(9, 420, time_ms-200);
        delay(time_ms+1000);
        // Move
        baseWritePos(540, time_ms);
        SERVO.WritePos(3, 650, time_ms);
        SERVO.WritePos(4, 420, time_ms);
        SERVO.WritePos(5, 620, time_ms);
        SERVO.WritePos(6, 600, time_ms);
        SERVO.WritePos(7, 400, time_ms);
        SERVO.WritePos(8, 420, time_ms);
        SERVO.WritePos(9, 420, time_ms);
        delay(time_ms+1000);
}


void Beer(int time_ms){
        baseWritePos(480, time_ms);
        SERVO.WritePos(3, 500, time_ms);
        SERVO.WritePos(4, 510, time_ms);
        SERVO.WritePos(5, 530, time_ms);
        SERVO.WritePos(6, 680, time_ms);
        SERVO.WritePos(7, 540, time_ms);
        SERVO.WritePos(8, 520, time_ms);
        SERVO.WritePos(9, 540, time_ms);
        delay(time_ms+1000);
        // Move
        baseWritePos(630, time_ms);
        SERVO.WritePos(3, 700, time_ms);
        SERVO.WritePos(4, 600, time_ms);
        SERVO.WritePos(5, 420, time_ms);
        SERVO.WritePos(6, 500, time_ms);
        SERVO.WritePos(7, 380, time_ms);
        SERVO.WritePos(8, 530, time_ms);
        SERVO.WritePos(9, 550, time_ms);
        delay(time_ms+1000);
        // Clutch
        SERVO.WritePos(8, 470, time_ms-500);
        SERVO.WritePos(9, 370, time_ms-500);
        delay(time_ms+1000);

        SERVO.WritePos(4, 630, time_ms);
        SERVO.WritePos(5, 380, time_ms);
        delay(time_ms+2000);

        SERVO.WritePos(4, 590, time_ms);
        SERVO.WritePos(5, 430, time_ms);
        SERVO.WritePos(8, 350, time_ms-500);
        SERVO.WritePos(9, 550, time_ms-500);
        delay(time_ms+2000);

        SERVO.WritePos(8, 520, time_ms);
        SERVO.WritePos(9, 540, time_ms);
        delay(time_ms+1000);

        homePosition(1500);
}
