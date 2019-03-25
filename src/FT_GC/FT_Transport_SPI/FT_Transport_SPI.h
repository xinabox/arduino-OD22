/*****************************************************************************
* Copyright (c) Future Technology Devices International 2014
* propriety of Future Technology devices International.
*
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by FTDI. 
* FTDI accept no responsibility for any issues resulting from its use. 
* The developer of the final application incorporating any parts of this 
* sample project is responsible for ensuring its safe and correct operation 
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file                           FT_Transport_i2cspi.h
* @brief                          Contains FT transport layer specific to SPI for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later
								  Dependencies: arduino standard SPI library
* @version                        1.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT_TRANSPORT_SPI_H_
#define _FT_TRANSPORT_SPI_H_

#include "SC18IS602.h"
//#include "SPI.h"
SC18IS602 i2cspi = SC18IS602(SC18IS602_ADDRESS_111);

class FT_Transport_SPI
{
	public:
	/* Constructor and destructor methods */
	//initialize spi library
	FT_Transport_SPI()
	{
		//CSPin = cspin; //new library interface
	}
	
	//api to set the cs pin number 
	void SetCSpin(uint8_t cspin)
	{
		CSPin = cspin;
	}
	
	~FT_Transport_SPI(void){}//exit spi library
	void Init()
	{
		i2cspi.begin(1);
		/* Initialize SPI library */
		i2cspi.pinMode(CSPin, OUTPUT);
		i2cspi.digitalWrite(CSPin, HIGH);    

		/* SPI library specific */
		
		
/* handle due, yue, galileo and other hardware */
#if defined(__SAM3X8E__)
		//i2cspi.setClockDivider(4);//hardcoding for due board
#else	
		/* Other Arduino boards */
		//i2cspi.setClockDivider(SPI_CLOCK_DIV2);//for galileo this value should be fine
#endif	

		//i2cspi.setBitOrder(MSBFIRST);
		//i2cspi.setDataMode(SPI_MODE0);

	}
	void Exit()
	{
		/* close the spi channel */
		i2cspi.end();
	}
	void ChangeClock(uint32_t ClockValue)
	{
		/* For due it is straight forward, for others only a switch */
#if defined(__SAM3X8E__)
		//i2cspi.setClockDivider((uint32_t)(84*1000000/ClockValue));//hardcoding for due board
#else	
		/* Other Arduino boards */
		///if(ClockValue < 1000000)
		//	i2cspi.setClockDivider(SPI_CLOCK_DIV32);//for galileo this value should be fine
		//else
		//	i2cspi.setClockDivider(SPI_CLOCK_DIV2);//for galileo this value should be fine
#endif			
	}
	/* APIs related to memory read & write/transport */
	void StartRead(uint32_t Addr)
	{
		//make sure the union array indexes are modified for big endian usecase
		union
		{
			uint32_t UUint32;
			uint8_t  A[4];
		};
		UUint32 = Addr;
		i2cspi.digitalWrite(CSPin, LOW);
		i2cspi.transfer(A[2]);
		i2cspi.transfer(A[1]);
		i2cspi.transfer(A[0]);
		i2cspi.transfer(0x00);//dummy byte for read
	}
	void StartWrite(uint32_t Addr)
	{
		union
		{
			uint32_t UUint32;
			uint8_t  A[4];
		};
		UUint32 = Addr;
		i2cspi.digitalWrite(CSPin, LOW);
		i2cspi.transfer(A[2] | 0x80);
		i2cspi.transfer(A[1]);
		i2cspi.transfer(A[0]);
	}
	
	//Read a byte from Addr location
	uint8_t Read(uint32_t Addr)
	{
		uint8_t ReadByte;
		StartRead(Addr);
		ReadByte = i2cspi.transfer(0x00);
		EndTransfer();
		return (ReadByte);
	}
	
	//read a short from Addr location
	uint16_t Read16(uint32_t Addr)
	{
		union
		{
			uint16_t UUint16;
			uint8_t  A[2];
		};
		///little endian read
		StartRead(Addr);
		A[0] = i2cspi.transfer(0x00);
		A[1] = i2cspi.transfer(0x00);		
		EndTransfer();
		return (UUint16);
	}
	//read a word from Addr location
	uint32_t Read32(uint32_t Addr)
	{
		union
		{
			uint32_t UUint32;
			uint8_t  A[4];
		};
		//little endian read
		StartRead(Addr);		
		A[0] = i2cspi.transfer(0x00);
		A[1] = i2cspi.transfer(0x00);		
		A[2] = i2cspi.transfer(0x00);
		A[3] = i2cspi.transfer(0x00);		
		EndTransfer();
		return (UUint32);
	}
	//read N bytes from Addr location
	void Read(uint32_t Addr, uint8_t *Src, uint32_t NBytes)
	{
		uint32_t i;
		StartRead(Addr);
		for(i=0;i<NBytes;i++)
			*Src++ = i2cspi.transfer(0x00);
		EndTransfer();
	}
	
	//write a byte to Addr location
	void Write(uint32_t Addr, uint8_t Value8)
	{	
		StartWrite(Addr);
		i2cspi.transfer(Value8);
		EndTransfer();
	}
	//write a short to Addr location
	void Write16(uint32_t Addr, uint16_t Value16)
	{
		union
		{
			uint16_t UUint16;
			uint8_t  A[2];
		};
		UUint16 = Value16;
		///little endian read
		StartWrite(Addr);
		i2cspi.transfer(A[0]);
		i2cspi.transfer(A[1]);		
		EndTransfer();
	}	
	//write a word to Addr location
	void Write32(uint32_t Addr, uint32_t Value32)
	{
		union
		{
			uint32_t UUint32;
			uint8_t  A[4];
		};
		UUint32 = Value32;
		///little endian read
		StartWrite(Addr);
		i2cspi.transfer(A[0]);
		i2cspi.transfer(A[1]);		
		i2cspi.transfer(A[2]);
		i2cspi.transfer(A[3]);		
		EndTransfer();
	}	
	
	void Write(uint32_t Addr, uint8_t *Src, uint32_t NBytes)
	{
		uint32_t i;
		StartWrite(Addr);
		for(i=0;i<NBytes;i++)
			i2cspi.transfer(*Src++);
		EndTransfer();
	}
	
	void Writefromflash(uint32_t Addr, prog_uchar *Src, uint32_t NBytes)
	{
		uint32_t i;
		StartWrite(Addr);
		for(i=0;i<NBytes;i++)
		{
			i2cspi.transfer(pgm_read_byte_near(Src));
			Src++;
		}		
		EndTransfer();
	}
	
	//apis useful for performance via spi - write only functionality

	//assert CSpin and send write command
	void StartTransfer(uint32_t Addr)
	{
		union
		{
			uint32_t UUint32;
			uint8_t  A[4];
		};
		UUint32 = Addr;
		i2cspi.digitalWrite(CSPin, LOW);
		i2cspi.transfer(A[2] | 0x80);
		i2cspi.transfer(A[1]);
		i2cspi.transfer(A[0]);
	}

	//de assert CSpin
	void EndTransfer(void)
	{
		i2cspi.digitalWrite(CSPin, HIGH);
	}
	//transfer a single byte
	void Transfer(uint8_t Value8)
	{
		i2cspi.transfer(Value8);
	}
	//transfer 2 bytes
	void Transfer16(uint16_t Value16)
	{
		union
		{
			uint16_t UUint16;
			uint8_t  A[2];
		};
		UUint16 = Value16;
		A[0] = i2cspi.transfer(A[0]);
		A[1] = i2cspi.transfer(A[1]);		
	}
	//transfer 4 bytes
	void Transfer32(uint32_t Value32)
	{
		union
		{
			uint32_t UUint32;
			uint8_t  A[4];
		};
		UUint32 = Value32;		
		i2cspi.transfer(A[0]);
		i2cspi.transfer(A[1]);
		i2cspi.transfer(A[2]);
		i2cspi.transfer(A[3]);		
	}
	//transfer N bytes
	void Transfer(uint8_t *Buff,uint32_t NBytes)
	{
		uint32_t i;
		for(i=0;i<NBytes;i++)
		{
			i2cspi.transfer(*Buff++);
		}
	}
	
	uint8_t CSPin;//chip select pin of spi driver
};


#endif /* _FT_TRANSPORT_SPI_H_ */

