/*
 * SCServo.cpp
 * Author: Xiongle Tan
 * Date: 2016.8.9
 *
 *
 * Co-author: Bifei Liu
 * Date: 2017.4.21
 */

#include "SCServo.h"
#include <sys/time.h>

HardwareSerial::HardwareSerial()
{
	;
}

void HardwareSerial::init(char *device, int baudrate)
{
	serial.device = device;
	serial.baud = baudrate;
	//FIXME: have no idea why it cannot support special baud rate now
	serial.databit = 8;
	serial.parity = 'n';
	serial.stopbit = 1;
	serial_init(&serial);
}

int HardwareSerial::write(unsigned char *data, int len)
{
	return serial_send(&serial, (const char*)(data), len);
}

int HardwareSerial::read()
{
	char *buf = 0;
	if(serial_recv_noblock(&serial, buf, 1) == -1)
	{
		return -1;
	}
	return (int)(*buf);
}


SCServo::SCServo(char *device, int baudrate)
{
	IOTimeOut = 2;
	Serial.init(device, baudrate);
}

int SCServo::readSCS(unsigned char *nDat, int nLen)
{
	int Size = 0;
//	int ComData;
//	unsigned long t_user;
//	struct timeval stop, start;


	/*FIXME: can still not use the non-blocking way to receive data, having no idea why*/
	Size = serial_recv(&(Serial.serial), (char *)nDat, nLen);

	// while(1){
	// 	ComData = Serial.read();
	// 	if(ComData!=-1){
	// 		if(nDat){
	// 			nDat[Size] = ComData;
	// 		}
	// 		Size++;
	// 		gettimeofday(&start, NULL);
	// 	}
	// 	if(Size>=nLen){
	// 		break;
	// 	}
	// 	gettimeofday(&stop, NULL);
	// 	t_user =  stop.tv_usec - start.tv_usec;
	// 	if(t_user>IOTimeOut){
	// 		break;
	// 	}
	// }
	return Size;
}

int SCServo::writeSCS(unsigned char *nDat, int nLen)
{
	if(nDat==NULL){
		return 0;
	}
	return Serial.write(nDat, nLen);
}

int SCServo::writeSCS(unsigned char bDat)
{
	return Serial.write(&bDat, 1);
}

void SCServo::flushSCS()
{
	while(Serial.read()!=-1);
}
