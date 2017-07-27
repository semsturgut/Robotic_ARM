/*
 * SCServo.h
 * Author: Xiongle Tan
 * Date: 2016.8.25
 *
 *
 * Co-author: Bifei Liu
 * Date: 2017.4.21
 */

#ifndef _SCSERVO_H
#define _SCSERVO_H

#include "SCSProtocol.h"
#include "serial.h"
#include <stdio.h>

class HardwareSerial
{
public:
	HardwareSerial();
	int write(unsigned char *data, int len);
	int read();
	void init(char *device, int baudrate);
public:
	struct serial_t serial;
};

class SCServo: public SCSProtocol
{
public:
	SCServo(char *device, int baudrate);
	virtual ~SCServo()
	{
		;
	}
	virtual int writeSCS(unsigned char *nDat, int nLen);
	virtual int readSCS(unsigned char *nDat, int nLen);
	virtual int writeSCS(unsigned char bDat);
	virtual void flushSCS();
public:
	unsigned long int IOTimeOut;
	HardwareSerial Serial;
};

#endif
