#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ioctl.h>
#include <fcntl.h>
#include <termios.h>

char *portname = "/dev/ttyUSB0";

// Configuring the serial ports.
void PortConfig(int fd, termios toptions) {
        // get current serial port settings
        tcgetattr(fd, &toptions);
        // set 9600 baud both ways
        cfsetispeed(&toptions, B9600);
        cfsetospeed(&toptions, B9600);
        // 8 bits, no parity, no stop bits
        toptions.c_cflag &= ~PARENB;
        toptions.c_cflag &= ~CSTOPB;
        toptions.c_cflag &= ~CSIZE;
        toptions.c_cflag |= CS8;
        // Canonical mode
        toptions.c_lflag |= ICANON;
        // commit the serial port settings
        tcsetattr(fd, TCSANOW, &toptions);
}

int main() {
        int fd;
        struct termios toptions;
        PortConfig(fd, toptions);

        while (true) {
                write(fd, "0", 1);
                sleep(0.5);
        }

        return 0;
}
