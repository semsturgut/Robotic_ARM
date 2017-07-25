#include <stdint.h>
#include <stdio.h>
#include "serial.h"
#include "SCServo.h"
#include "unistd.h"

int main() {
	
        char device[20] = "/dev/ttyUSB0";
        SCServo servo(device, 115200);
        
		int pos = 1023;
		printf("received: %d\n", pos);
		servo.WritePos(1, pos, 4000, 100);
		sleep(2);
		servo.WritePos(1, 0, 4000, 50);
		sleep(2);
		servo.WritePos(2, pos, 4000, 100);
		sleep(2);
		servo.WritePos(2, 0, 4000, 50);
		sleep(2);
		servo.WritePos(3, pos, 4000, 100);
		sleep(2);
		servo.WritePos(3, 0, 4000, 50);

    	return 0;
}
