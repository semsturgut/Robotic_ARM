int servo_ID, servo_Position, servo_Duration;
String input_String;
int ind1,ind2,ind3;

void setup() {
        Serial.begin(19200);
        Serial1.begin(9600);
}

void loop() {
        if (Serial1.available()) {
                char input_Char = Serial1.read();
                if (input_Char !='#') {
                        input_String += input_Char;
                } else {
                        // Serial.println(input_String);
                        ind1 = input_String.indexOf(','); // finds location of first ,
                        servo_ID = input_String.substring(0, ind1).toInt(); // captures first data String
                        ind2 = input_String.indexOf(',', ind1+1 ); // finds location of second ,
                        servo_Position = input_String.substring(ind1+1, ind2+1).toInt(); // captures second data String
                        ind3 = input_String.indexOf(',', ind2+1 ); // finds location of third ,
                        servo_Duration = input_String.substring(ind2+1, ind3+1).toInt(); // captires third data String
                        Serial.print("$");
                        Serial.print(servo_ID);
                        Serial.print(",");
                        Serial.print(servo_Position);
                        Serial.print(",");
                        Serial.print(servo_Duration);
                        Serial.println("#");
                        input_String = "";
                        servo_ID = servo_Position = servo_Duration = 0;
                }
        }
}
