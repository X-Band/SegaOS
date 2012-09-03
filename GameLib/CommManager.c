/*
	File:		CommManager.c

	Contains:	Simple comm routines for mortal kombat prototype

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/18/94	ADS		OBSOLETE
		 <2>	 5/14/94	SAH		Fixed global init in kHardInitialize.

	To Do:
*/


/*
*	This manager handles communication. The current version only knows about a strict serial connection.
*	When we have modems, this file will need to be revved.
*/

#if 0			// obsolete!!!

#include "SegaOS.h"
#include "CommPriv.h"
#include "Reliability.h"
#include "serial.h"


#ifdef SIMULATOR
#include <stdio.h>
#include <stdlib.h>
#endif


CommError	_InitConnection ( void );
CommError	_ReadSerialByte ( unsigned char *byte );
CommError	_WriteSerialByte ( unsigned char byte );
CommError	_TransmitBufferFree ( void );
CommError	_WaitForConnection ( void );
CommError	_TestForConnection ( void );
CommError	_AcknowledgeConnection ( void );
CommError	_DialNumber ( char * number );
CommError	_Disconnect ( void );



#define	kConnectToken		0xae

/* we need a better way to define these */


#ifndef	SIMULATOR
CommGlobals comms;
#endif

long
_CommControl ( short command, long data )
{
CommGlobals *globals;
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
		
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(comms,offset);
			
			error = AllocateGlobalSpace ( kCommManager, offset, sizeof(CommGlobals), (Ptr *) &globals );
			if ( error != noErr )
				{
				return error;
				}
			
			globals->status = 0;
			globals->lastError = 0;
			
			/* install our selectors */
			SetDispatchedFunction ( kInitConnection,		kCommManager,	_InitConnection );
			SetDispatchedFunction ( kReadSerialByte,		kCommManager,	_ReadSerialByte );
			SetDispatchedFunction ( kWriteSerialByte,		kCommManager,	_WriteSerialByte );
			SetDispatchedFunction ( kTransmitBufferFree,	kCommManager,	_TransmitBufferFree );
			SetDispatchedFunction ( kWaitForConnection,		kCommManager,	_WaitForConnection );
			SetDispatchedFunction ( kTestForConnection,		kCommManager,	_TestForConnection );
			SetDispatchedFunction ( kAcknowledgeConnection, kCommManager,	_AcknowledgeConnection );
			SetDispatchedFunction ( kDialNumber,			kCommManager,	_DialNumber );
			SetDispatchedFunction ( kDisconnect,			kCommManager,	_Disconnect );
			break;
		
		case kSoftInialize:
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
#ifdef SIMULATOR
			printf ( "comm code block moved\n");
#endif
			error = noErr;
			break;
					
		case kGlobalsMoved:
#ifdef SIMULATOR
			printf ( "comm global block moved\n");
#endif
			error = noErr;
			break;
		}
		
	return error;
}



CommError
_InitConnection ( void )
{
	*sIntEn = 0;				// clear all interrupts
	*sLineCtl = sLCRDLab;		// get ready to set divisor
	*sDivLow = 85;				// 2400 baud with 3.2768 Mhx crystal
	*sDivHi = 0;
	
	*sLineCtl = sLCR8Bits + sLCR1Stop + sLCRPrtyEn;
}


CommError
_ReadSerialByte ( unsigned char *byte )
{
short error;
short lineStatus;

	error = 0;
	
	/* get the serial status */
	lineStatus = *sLineSts;
	
	/* check for transmission errors */
	if ( lineStatus & sParityError )
		{
		error = eParityError;
		FLAGANALYZER();
		}
	else
	if ( lineStatus & sFrameError )
		{
		error = eFrameError;
		FLAGANALYZER();
		}
	else
	/* read the data if there is some */
	if ( *sLineSts & sDataReady )
		{
		*byte = *sRcvTrx;
		}
	else
		{
		error = eNoByteReady;
		}
	
	REFGLOBAL( comms, lastError ) = error;
	return error;
}


CommError
_WriteSerialByte ( unsigned char byte )
{
short error;

	error = 0;
	
	*sRcvTrx = byte;
	
	REFGLOBAL( comms, lastError ) = error;
	return error;
}


CommError
_TransmitBufferFree ( void )
{
short error;

	error = eTransmitBufferFull;
	
	if ( *sLineSts & sXmitHoldingEmpty )
		{
		error = 0;
		}

	REFGLOBAL( comms, lastError ) = error;
	return error;
}


CommError
_WaitForConnection ( void )
{
short error;

	error = 0;

	REFGLOBAL( comms, lastError ) = error;
	return error;
}


CommError
_TestForConnection ( void )
{
short error;
unsigned char byte;

	error = eNoConnection;
	
	/* if we got a byte, make sure it's the connect token */
	if ( ReadSerialByte ( &byte ) == 0 )
		{
		if ( byte == kConnectToken )
			{
			error = 0;
			}
		}

	REFGLOBAL( comms, lastError ) = error;
	return error;
}


CommError
_AcknowledgeConnection ( void )
{
	return WriteSerialByte ( kConnectToken );
}


CommError
_DialNumber ( char * number )
{
	return WriteSerialByte ( kConnectToken );
}


CommError
_Disconnect ( void )
{
short error;

	error = 0;

	REFGLOBAL( comms, lastError ) = error;
	return error;
}


#endif



