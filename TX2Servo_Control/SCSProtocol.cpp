/*
 * SCSProtocol.cpp
 * Author: Xiongle Tan
 * Date: 2016.8.25
 *
 *
 * Co-author: Bifei Liu
 * Date: 2016.4.21
 */

#include <stddef.h>
#include "SCServo.h"

SCSProtocol::SCSProtocol()
{
	Level = 1;//Generate acknowledgment except broadcast command
	End = 0;//big endian
}

//a 16-bit integer divided into two 8-bit integers
//DataL is low 8-bit and DataH is high 8-bit
void SCSProtocol::Host2SCS(u8 *DataL, u8* DataH, int Data)
{
	if(End){
		*DataL = (Data>>8);
		*DataH = (Data&0xff);
	}else{
		*DataH = (Data>>8);
		*DataL = (Data&0xff);
	}
}

//two 8-bit integers combined to one 16-bit integer
//DataL is low 8-bit and DataH is high 8-bit
int SCSProtocol::SCS2Host(u8 DataL, u8 DataH)
{
	int Data;
	if(End){
		Data = DataL;
		Data<<=8;
		Data |= DataH;
	}else{
		Data = DataH;
		Data<<=8;
		Data |= DataL;
	}
	return Data;
}

void SCSProtocol::writeBuf(u8 ID, u8 MemAddr, u8 *nDat, u8 nLen, u8 Fun)
{
	u8 msgLen = 2;
	u8 bBuf[6];
	u8 CheckSum = 0;
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = ID;
	bBuf[4] = Fun;
	if(nDat){
		msgLen += nLen + 1;
		bBuf[3] = msgLen;
		bBuf[5] = MemAddr;
		writeSCS(bBuf, 6);

	}else{
		bBuf[3] = msgLen;
		writeSCS(bBuf, 5);
	}
	CheckSum = ID + msgLen + Fun + MemAddr;
	u8 i = 0;
	if(nDat){
		for(i=0; i<nLen; i++){
			CheckSum += nDat[i];
		}
	}
	writeSCS(nDat, nLen);
	writeSCS(~CheckSum);
}

/* Description: general writing command
 * Parameters:
 * ID           -- servo ID
 * MemAddr      -- memory address
 * nDat         -- written data
 * nLen         -- written length
 */
int SCSProtocol::genWrite(u8 ID, u8 MemAddr, u8 *nDat, u8 nLen)
{
	flushSCS();
	writeBuf(ID, MemAddr, nDat, nLen, INST_WRITE);
	return Ack(ID);
}

/* Description: asynchronous writing command
 * ID            -- servo ID
 * MemAddr       -- memory address
 * nDat          -- written data
 * nLen          -- written length
 */
int SCSProtocol::regWrite(u8 ID, u8 MemAddr, u8 *nDat, u8 nLen)
{
	flushSCS();
	writeBuf(ID, MemAddr, nDat, nLen, INST_REG_WRITE);
	return Ack(ID);
}

/* Description: synchronous writing command
 * ID            -- servo ID array
 * IDN           -- array length
 * MemAddr       -- memory address
 * nDat          -- written data
 * nLen          -- written length
 */
void SCSProtocol::snycWrite(u8 ID[], u8 IDN, u8 MemAddr, u8 *nDat, u8 nLen)
{
	u8 mesLen = ((nLen+1)*IDN+4);
	u8 Sum = 0;
	u8 bBuf[7];
	bBuf[0] = 0xff;
	bBuf[1] = 0xff;
	bBuf[2] = 0xfe;
	bBuf[3] = mesLen;
	bBuf[4] = INST_SYNC_WRITE;
	bBuf[5] = MemAddr;
	bBuf[6] = nLen;
	writeSCS(bBuf, 7);

	Sum = 0xfe + mesLen + INST_SYNC_WRITE + MemAddr + nLen;
	u8 i, j;
	for(i=0; i<IDN; i++){
		writeSCS(ID[i]);
		writeSCS(nDat, nLen);
		Sum += ID[i];
		for(j=0; j<nLen; j++){
			Sum += nDat[j];
		}
	}
	writeSCS(~Sum);
}

int SCSProtocol::writeByte(u8 ID, u8 MemAddr, u8 bDat)
{
	flushSCS();
	writeBuf(ID, MemAddr, &bDat, 1, INST_WRITE);
	return Ack(ID);
}

int SCSProtocol::writeWord(u8 ID, u8 MemAddr, u16 wDat)
{
	flushSCS();
	u8 buf[2];
	Host2SCS(buf+0, buf+1, wDat);
	writeBuf(ID, MemAddr, buf, 2, INST_WRITE);
	return Ack(ID);
}

int SCSProtocol::EnableTorque(u8 ID, u8 Enable)
{
	return writeByte(ID, P_TORQUE_ENABLE, Enable);
}

int SCSProtocol::writePos(u8 ID, u16 Position, u16 Time, u16 Speed, u8 Fun)
{
	flushSCS();
	u8 buf[6];
	Host2SCS(buf+0, buf+1, Position);
	Host2SCS(buf+2, buf+3, Time);
	Host2SCS(buf+4, buf+5, Speed);
	writeBuf(ID, P_GOAL_POSITION_L, buf, 6, Fun);
	return Ack(ID);
}

//general position writing command
int SCSProtocol::WritePos(u8 ID, u16 Position, u16 Time, u16 Speed)
{
	return writePos(ID, Position, Time, Speed, INST_WRITE);
}

//asynchronous position writing command
int SCSProtocol::RegWritePos(u8 ID, u16 Position, u16 Time, u16 Speed)
{
	return writePos(ID, Position, Time, Speed, INST_REG_WRITE);
}

void SCSProtocol::RegWriteAction()
{
	writeBuf(0xfe, 0, NULL, 0, INST_ACTION);
}

/* Description: synchronous position writing command
 * ID              -- servo ID array
 * IDN             -- array length
 * Position        -- targeted position
 * Time            -- execution time
 * Speed           -- execution speed
 */
void SCSProtocol::SyncWritePos(u8 ID[], u8 IDN, u16 Position, u16 Time, u16 Speed)
{
	u8 buf[6];
	Host2SCS(buf+0, buf+1, Position);
	Host2SCS(buf+2, buf+3, Time);
	Host2SCS(buf+4, buf+5, Speed);
	snycWrite(ID, IDN, P_GOAL_POSITION_L, buf, 6);
}

/* Description: reading command
 * ID            -- servo ID
 * MemAddr       -- memory address
 * nDat          -- written data
 * nLen          -- written length
 */
int SCSProtocol::Read(u8 ID, u8 MemAddr, u8 *nData, u8 nLen)
{
	flushSCS();
	writeBuf(ID, MemAddr, &nLen, 1, INST_READ);
	u8 bBuf[5];
	if(readSCS(bBuf, 5)!=5){
		return 0;
	}
	int Size = readSCS(nData, nLen);
	if(readSCS(bBuf, 1)){
		return Size;
	}
	return 0;
}

//reading one byte, return -1 if overtime
int SCSProtocol::readByte(u8 ID, u8 MemAddr)
{
	u8 bDat;
	int Size = Read(ID, MemAddr, &bDat, 1);
	if(Size!=1){
		return -1;
	}else{
		return bDat;
	}
}

//reading two bytes, return -1 if overtime
int SCSProtocol::readWord(u8 ID, u8 MemAddr)
{
	u8 nDat[2];
	int Size;
	u16 wDat;
	Size = Read(ID, MemAddr, nDat, 2);
	if(Size!=2)
		return -1;
	wDat = SCS2Host(nDat[0], nDat[1]);
	return wDat;
}

//reading position, return -1 if overtime
int SCSProtocol::ReadPos(u8 ID)
{
	return readWord(ID, P_PRESENT_POSITION_L);
}

//multiple-turn control command
int SCSProtocol::WriteSpe(u8 ID, s16 Speed)
{
	if(Speed<0){
		Speed = -Speed;
		Speed |= (1<<10);
	}
	return writeWord(ID, P_GOAL_TIME_L, Speed);
}

//reading voltage, return -1 if overtime
int SCSProtocol::ReadVoltage(u8 ID)
{
	return readByte(ID, P_PRESENT_VOLTAGE);
}

//reading temperature, return -1 if overtime
int SCSProtocol::ReadTemper(u8 ID)
{
	return readByte(ID, P_PRESENT_TEMPERATURE);
}

//ping command, return servo ID if normal, return -1 if overtime
int SCSProtocol::Ping(u8 ID)
{
	flushSCS();
	u8 bBuf[6];
	writeBuf(ID, 0, NULL, 0, INST_PING);
	int Size = readSCS(bBuf, 6);
	if(Size==6){
		return bBuf[2];
	}else{
		return -1;
	}
}

int SCSProtocol::wheelMode(u8 ID)
{
	u8 bBuf[4];
	bBuf[0] = 0;
	bBuf[1] = 0;
	bBuf[2] = 0;
	bBuf[3] = 0;
	return genWrite(ID, P_MIN_ANGLE_LIMIT_L, bBuf, 4);
}

int SCSProtocol::joinMode(u8 ID, u16 minAngle, u16 maxAngle)
{
	u8 bBuf[4];
	Host2SCS(bBuf, bBuf+1, minAngle);
	Host2SCS(bBuf+2, bBuf+3, maxAngle);
	return genWrite(ID, P_MIN_ANGLE_LIMIT_L, bBuf, 4);
}

//reset to default values
int SCSProtocol::Reset(u8 ID)
{
	flushSCS();
	writeBuf(ID, 0, NULL, 0, INST_RESET);
	return Ack(ID);
}

int	SCSProtocol::Ack(u8 ID)
{
	if(ID != 0xfe && Level){
		u8 buf[6];
		u8 Size = readSCS(buf, 6);
		if(Size!=6){
			return 0;
		}
	}
	return 1;
}
