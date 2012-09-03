/*
	File:		controls.c

	Contains:	Controller read routines

	Written by:	Shannon & Andy

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 8/26/94	JOE		add kEnterIsLF support, also keyboard detect wasn't returning
									valid value if timed out...fixed dat.
		<25>	 8/25/94	JOE		hardware keyboard tweaks
		<24>	 8/25/94	JOE		keyboard id now in keymap
		<23>	 8/25/94	JOE		this 'n that
		<22>	 8/25/94	JOE		fix bugs, add hardware keyboard stuff
		<21>	 8/17/94	JOE		Add Button X,Y, & Z support to stimulator
		<20>	 8/11/94	ADS		Add a NOP for older boxes
		<19>	  8/8/94	ADS		Read 15x per second instead of 60x
		<18>	  8/6/94	ADS		New interface for types & reading
		<17>	  8/1/94	MWF		add initControllers, cleanup control code w/vectors to specific
									routines...
		<16>	 7/31/94	SAH		Don't turn off z80 unless it's runnin.
		<15>	 7/30/94	MWF		support for EA four player adapter supports 3button & 6button in port A,
									only 3button in ports B-D (is this documented in EA manual?)
		<14>	 7/27/94	JOE		Safer Z80 pausing
		<13>	 7/25/94	ADS		All better now!  All controller types work.
		<12>	 7/23/94	ADS		works w/3button and 6button.  4 player still broken
		<11>	 7/22/94	ADS		didn't mean to leave in warnings
		<10>	 7/22/94	ADS		Newer version with slowed-down reads
		 <9>	 7/21/94	ADS		Lotsa new stuff to support new architecture
		 <8>	 7/18/94	JOE		pause Z80 around controller reads to prevent DMA conflict
		 <7>	 7/18/94	ADS		Dispatcher cleanup
		 <6>	 7/13/94	ADS		Added controller #2 key equiv's
		 <5>	  7/5/94	ADS		Sim can now do A,B,C fire buttons
		 <4>	 6/13/94	SAH		Moved modem state stuff to SegaIn.c
		 <3>	 6/10/94	KON		Added support to the SIMULATOR so if the delete key is held it's
									equivalent to starting a remote modem connect sequence. This is
									for testing.
		 <2>	 5/30/94	SAH		Invert the hardware controller read for kon.
		 <2>	 5/24/94	SAH		Added kReadHardwareController.

	To Do:
*/



#include "SegaOS.h"
#include "controls.h"
#include "ControlsPriv.h"
#include "Reliability.h"
#include "SegaIn.h"
#include "SegaSound.h"
#include "Time.h"
#include "Errors.h"
#include "DBTypes.h"
#include "DBConstants.h"
#include "BoxSer.h"

/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
ControlGlobals controls;
#endif


ControllerType	_InitControllers( void );
short	_ReadHardwareController ( short whichOne );
void	_ReadAllControllers ( ControllerType type, short* results, short flags );
void	_ControllerVBL( void );

Boolean _FlushHardwareKeyboardBuffer( void );
unsigned char _GetNextHardwareKeyboardChar( void );
long _GetHardwareKeyboardFlags( void );
void _SetHardwareKeyboardFlags( long flags );
unsigned char _GetNextESKeyboardRawcode( void );
unsigned char _GetNextESKeyboardStatus( void );
void BackUpKeycodeTail( void );
unsigned char _GetNextESKeyboardChar( void );
void _SendCmdToESKeyboard( unsigned char *cmdBuf, unsigned char cmdLen );

void	ReadESKeyboard ( void );
void	WriteESKeyboard ( void );
Boolean	FindESKeyboard ( void );
void	EmulateJoypadWithKeyboard( short *results );



long
_ControllerControl ( short command, long data )
{
ControlGlobals *globals;
long offset;
short error;
long theID;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
		
			GETMGRGLOBALSOFFSET(controls,offset);

			error = AllocateGlobalSpace ( kControlManager, offset,
				sizeof(ControlGlobals), (Ptr *) &globals );
			
			ASSERT_MESG ( !error, "Can't create controls globals" );
			if ( error != noErr )
				{
				return error;
				}
				

			/* install our selectors */
			SetDispatchedFunction ( kInitControllers,			kControlManager,	_InitControllers );
			SetDispatchedFunction ( kReadHardwareController,	kControlManager,	_ReadHardwareController );
			SetDispatchedFunction ( kControllerVBL,				kControlManager,	_ControllerVBL );
			SetDispatchedFunction ( kReadAllControllers,		kControlManager,	_ReadAllControllers );
			
			SetDispatchedFunction ( kFlushHardwareKeyboardBuffer,	kControlManager,	_FlushHardwareKeyboardBuffer );
			SetDispatchedFunction ( kGetNextHardwareKeyboardChar,	kControlManager,	_GetNextHardwareKeyboardChar );
			SetDispatchedFunction ( kGetHardwareKeyboardFlags,	kControlManager,	_GetHardwareKeyboardFlags );
			SetDispatchedFunction ( kSetHardwareKeyboardFlags,	kControlManager,	_SetHardwareKeyboardFlags );
			SetDispatchedFunction ( kGetNextESKeyboardRawcode,	kControlManager,	_GetNextESKeyboardRawcode );
			SetDispatchedFunction ( kGetNextESKeyboardStatus,	kControlManager,	_GetNextESKeyboardStatus );
			SetDispatchedFunction ( kGetNextESKeyboardChar,		kControlManager,	_GetNextESKeyboardChar );
			SetDispatchedFunction ( kSendCmdToESKeyboard,		kControlManager,	_SendCmdToESKeyboard );
			break;
		
		case kSoftInialize:
			REFGLOBAL( controls, samplePhase ) = -1;
			REFGLOBAL( controls, curType ) = -1;
			REFGLOBAL( controls, controlValues[0] ) = 0;
			REFGLOBAL( controls, controlValues[1] ) = 0;
			REFGLOBAL( controls, controlValues[2] ) = 0;
			REFGLOBAL( controls, controlValues[3] ) = 0;
			REFGLOBAL( controls, controlValues[4] ) = 0;
			
			REFGLOBAL( controls, keyboardPresent ) = false;
			REFGLOBAL( controls, sysKeysHead ) = 0;
			REFGLOBAL( controls, sysKeysTail ) = 0;
			REFGLOBAL( controls, keycodeHead ) = 0;
			REFGLOBAL( controls, keycodeTail ) = 0;
			REFGLOBAL( controls, statusHead ) = 0;
			REFGLOBAL( controls, statusTail ) = 0;
			REFGLOBAL( controls, cmdHead ) = 0;
			REFGLOBAL( controls, cmdTail ) = 0;

			REFGLOBAL( controls, keyMapTable ) = (KeyMappingTable *)DBGetItem( kCodeletsType, 1 );
			REFGLOBAL( controls, keyboardFlags ) = 0L;

			REFGLOBAL( controls, keyboardID ) = *(unsigned long *)REFGLOBAL( controls, keyMapTable );
			REFGLOBAL( controls, keyMapTable ) = (KeyMappingTable *)((char *)(REFGLOBAL( controls, keyMapTable )) + 4);	/* skip past ID */
			
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			WARNING_MESG ( "controller code block moved" );
			error = noErr;
			break;
					
		case kGlobalsMoved:
			WARNING_MESG ( "controller global block moved");
			error = noErr;
			break;
		}
		
	return error;
}




void	_ControllerVBL( void )
{
register short	whichRead = 0;		// +1 read controls, -1 sample controller types
register long	newPhase;

	// to avoid sucking down too much CPU, we do a few things based on
	// a phase counter.  We read the controls 15 times per second (instead
	// of 60), and every two seconds, we replace a controller read with
	// a controller-type sample.  This allows the user to change controllers
	// and we'll figure it out.
	
	newPhase = REFGLOBAL( controls, samplePhase ) - 1;
	if (newPhase < 0)	{
		whichRead = -1;
		newPhase = 2 * 60;						// recheck in 2 secs
	}
	else
	if ((newPhase & 3) == 0)
		whichRead = 1;

	REFGLOBAL( controls, samplePhase ) = newPhase;
		
	if (whichRead != 0)	{
	
#ifndef SIMULATOR
		if ( gSoundEnabled )
			gemsdmastart();						// pause the Z80
#endif
	
		if (whichRead < 0)
		{
			REFGLOBAL( controls, curType ) = InitControllers();
			if ( FindESKeyboard() )									// do we have an Eric Smith keyboard?
			{
				if (!REFGLOBAL( controls, keyboardPresent ))
				{
				unsigned char fuck;
					fuck = 0xFF;						/* reset keyboard */
					SendCmdToESKeyboard( &fuck, 1 );
					WriteESKeyboard();
					REFGLOBAL( controls, keyboardPresent ) = true;
					REFGLOBAL( controls, keyboardFlags ) = 0L;
				}	
			}
			else
				REFGLOBAL( controls, keyboardPresent ) = false;
		}
		else
		{
			ReadAllControllers( REFGLOBAL( controls, curType ),
								REFGLOBAL( controls, controlValues ), kWant6Buttons );
			if ( REFGLOBAL( controls, keyboardPresent ) )
			{
				ReadESKeyboard();
				WriteESKeyboard();
				EmulateJoypadWithKeyboard( REFGLOBAL( controls, controlValues ) );
			}
		}
	
#ifndef SIMULATOR
		if ( gSoundEnabled )
			gemsdmaend();						// release the Z80
#endif

	}	// whichRead != 0
}


short
_ReadHardwareController ( short whichOne )
{
	return REFGLOBAL( controls, controlValues[whichOne-1] );
}



#ifdef SIMULATOR

void	ReadESKeyboard ( void )
{
}

void	WriteESKeyboard ( void )
{
}

Boolean	FindESKeyboard ( void )
{
	return( false );
}

ControllerType	_InitControllers( void ) {
	return -2;							// magic value, to test preservation
}

void
_ReadAllControllers ( ControllerType type, short* results, short flags )
{
short	value1, value2;
Boolean	isOption = false;

	ASSERT(type == -2);	// make sure it's preserved correctly
	

	results[2] = 0;		// controllers 3 & 4 always zero on simulator
	results[3] = 0;
	results[4] = 0;		// never using a multi-tap on simulator
	
	if( isPressed( optionKey) )
		isOption = true;
			
	value1 = 0;
	value2 = 0;
	
	if( isPressed( leftArrow ) )
		value1 = kLEFT;
	else
	if( isPressed( rightArrow ) )
		value1 = kRIGHT;
	else
	if( isPressed( upArrow ) )
		value1 = kUP;
	else
	if( isPressed( downArrow ) )
		value1 = kDOWN;
	else
	if( isPressed( returnKey ) )
		value1 = kButtonPressed;				// enter presses are all-button presses
	else
	if( isOption && isPressed( letterAKey ) )
		value1 = kButtonA;						// letters A-C are single-button presses
	else										// need "option" to not screw up email kbd
	if( isOption && isPressed( letterBKey ) )
		value1 = kButtonB;
	else
	if( isOption && isPressed( letterCKey ) )
		value1 = kButtonC;
	else
	if( isOption && isPressed( letterXKey ) )
		value1 = kButtonX;
	else
	if( isOption && isPressed( letterYKey ) )
		value1 = kButtonY;
	else
	if( isOption && isPressed( letterZKey ) )
		value1 = kButtonZ;
	else
	if( isOption && isPressed( letterSKey ) )
		value1 = kStart;


	if( isPressed( kLeft2 ) )
		value2 = kLEFT;
	else
	if( isPressed( kRight2 ) )
		value2 = kRIGHT;
	else
	if( isPressed( kUp2 ) )
		value2 = kUP;
	else
	if( isPressed( kDown2 ) )
		value2 = kDOWN;
	else
	if( isPressed( kA2 ) )
		value2 = kButtonA;			// letters A-C are single-button presses
	else							// need "option" to not screw up email kbd
	if( isPressed( kB2 ) )
		value2 = kButtonB;
	else
	if( isPressed( kC2 ) )
		value2 = kButtonC;
	else
	if( isPressed( kX2 ) )
		value2 = kButtonX;
	else
	if( isPressed( kY2 ) )
		value2 = kButtonY;
	else
	if( isPressed( kZ2 ) )
		value2 = kButtonZ;
	else
	if( isPressed( kStart2 ) )
		value2 = kStart;

	results[0] = value1;
	results[1] = value2;

}

#else



//  new version which does lots of cool stuff!

typedef unsigned char		UChar;
typedef char				SChar;
typedef signed char			Char;
typedef unsigned short		UShort;
typedef unsigned long		ULong;

#define	kData2		0xA10005
#define kCtl2		0xA1000B
#define	kSerial2	0xA10019

#define	kData1		0xA10003
#define	kCtrl		6				// distance beyond datareg of each ctrlreg
#define kSer		0x10			// distance beyond DATA1 of the serial control reg
#define kNextPort	2				// distance to next set of registers

#define kTH			0x40			// controller mode control line
#define kTR			0x20			// controller handshake request
#define kTL			0x10			// controller handshack acknowledge

#define	kDataLines	0x0F			// data lines for 3-line hshk communication

#define nop			asm { nop }

#define	kUpToC			(kUP | kDOWN | kLEFT | kRIGHT | kButtonB | kButtonC)
#define	kAToStart		(kButtonA | kStart)
#define	kZToMode		(kButtonZ | kButtonY | kButtonX | kMode)

#define	kSegaFourPlayerID	3		// device ID numbers
#define	kSega3PadID			0
#define	kSega6PadID			1
#define	kSegaMouseID		3


// multiType constants
#define kNotMulti			0		// not a multi-port adaptor
#define kSegaMulti			1		// Sega multi-port adaptor
#define kEAMulti			2		// EA multi-port adaptor

#define kEAMultiBase		0x0c	// starting controller index (controls 1-4 = 0x0c-0x3c)
#define	kEANextPort			0x10	// increment to next EA FourPlayer controller index

#define kReadEAMulti 		0x8000	// mixin with "flags"


typedef void (*ControllerProcPtr)(short* results, short flags);

void	ReadEAMulti ( short* results, short flags );
void	ReadSegaMulti ( short* results, short flags );
void	ReadEAorSingles ( short* results, short flags );

short	FindEAMulti (void);
short	FindSegaMulti (void);

void	DeglitchControls ( short* results, short flags );
short	GetHandshakeNibble( short* hshkState );
short	GetHandshakeNibblePort2( short* hshkState );
void	PutHandshakeNibblePort2( short* hshkState, unsigned char byteToSend );


//
//----- First, the external interfaces
//

ControllerType
_InitControllers( void )
{
ControllerProcPtr	routine = ReadEAorSingles;			// default to simple controllers

	if (FindSegaMulti() != 0) 							// first check for Sega multi
	{							
		routine = ReadSegaMulti;
	}
	else 
	if (FindEAMulti() != 0)								// nope, check for EA multi 
	{					
		routine = ReadEAMulti;
	}

	return (ControllerType) routine;				// magic cookie is actually a proc
}


void
_ReadAllControllers( ControllerType type, short* results, short flags )
{
register	UChar*	resultPtr = (UChar*) results;
register	ControllerProcPtr readProc = (ControllerProcPtr) type;

	*(long*)(resultPtr) = 0;			// clear 0 and 1
	*(long*)(resultPtr+4) = 0;			// clear 2 and 3
	*(short*)(resultPtr+8) = 0;			// clear 4

	(*readProc) ( results, flags );
	
	DeglitchControls ( results, flags );
}


//
//----- Controller ID routines
//

short
FindSegaMulti(void) {
					UChar		readBuf[4];
register			UChar*		readScan = readBuf;
volatile register	UChar*		reg = (UChar*) kData1;
					short		hshkState;
register			long		timeout = 100;
		
	*(reg)			= kTH + kTR;						// both flags hi
	*(reg + kSer)	= 0;								// clear serial modes
	*(reg + kCtrl)	= kTH + kTR;						// both flags are outputs now
	
	nop; nop;
	*readScan++ = *reg & 0x0F;							// get 1st data nibble

	*reg = kTR;											// turn off TH to start hshk seq

	do {
		*readScan = *reg & 0x0F;
		}
		while ( (*readScan != 0x0F) && --timeout );		// 2nd nybble has no handshake

	if ( !timeout )
	{
		*reg = kTH + kTR;								// make sure we leave with TH & TR hi
		return ( FALSE );
	}

	readScan++;							
	
	hshkState = 0;										// start flipping TR
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 3rd data nibble
	*readScan = GetHandshakeNibble(&hshkState);			// get 4th data nibble

	*reg |= kTH;										// abort the transaction
		
	for ( timeout = 0; timeout != 50; timeout++ );		// spin a bit
	
	*reg = kTH + kTR;									// make sure we leave with TH & TR hi
		
	if ( *(ULong *) readBuf == 0x030f0000 )				// found a good Team Player
		return ( TRUE );
	else
		return ( FALSE );
}	


short
FindEAMulti(void) {

register			UChar	data;
volatile register	UChar*	reg5 = (UChar*) 0xa10005;
volatile register	UChar*	regb = (UChar*) 0xa1000b;
volatile register	UChar*	reg9 = (UChar*) 0xa10009;
volatile register	UChar*	reg3 = (UChar*) 0xa10003;


	*reg9 = 0x40;								// Ctrl 1 setup to read CBRLDU
	*regb = 0x43;								// Ctrl 2 setup to write on 0x43
	nop; nop;									// settle for 2
	*reg5 = 0x7c;								// writes a 0x40 to Ctrl 2
	nop; nop;									// settle for 2
	*regb = 0x7f;								// Ctrl 2 setup to write on 0x7F
	nop; nop;									// settle for 2
	*reg5 = 0x7c;								// writes a 0x7C to Ctrl 2
	nop; nop;									// settle for 2
	data = *reg3;
	data &= 0x3;								// reads and masks DU from Ctrl 1

	if (data == 0) {
		return	*reg5 = kEAMultiBase;			// setup controllerIndex for next read of ctrl 1
	} else {
		*regb = 0x40;							// setup to read Ctrl 2
		return 0;
	}
}


Boolean
FindESKeyboard(void) {
					UChar		readBuf[4];
register			UChar*		readScan = readBuf;
volatile register	UChar*		reg = (UChar*) kData2;	// only support keyboard on PORT 2!
					short		hshkState;
register			long		timeout = 100;
register			ULong		kbID = REFGLOBAL( controls, keyboardID );
		
	*(reg)				= kTH + kTR;					// both flags hi
	*(char *)kSerial2	= 0;							// clear serial modes
	*(char *)kCtl2		= kTH + kTR;					// both flags are outputs now
	
	nop; nop;
	*readScan++ = *reg & 0x0F;							// 1st nybble = identifying ID

	*reg = kTR;											// turn off TH to start hshk seq

	do {
		*readScan = *reg & 0x0F;
		}
		while ( (*readScan != ((kbID >> 16) & 0xF)) && --timeout );		// 2nd nybble has no handshake

	if ( !timeout )
	{
		*reg = kTH + kTR;								// make sure we leave with TH & TR hi
		return (false );
	}

	readScan++;							
	
	hshkState = 0;										// start flipping TR
	*readScan++ = GetHandshakeNibblePort2(&hshkState);	// 3rd nybble = local ID
	*readScan = GetHandshakeNibblePort2(&hshkState);	// 4th nybble = local ID

	*reg |= kTH;										// abort the transaction
		
	for ( timeout = 0; timeout != 50; timeout++ );		// spin a bit
	
	*reg = kTH + kTR;									// make sure we leave with TH & TR hi
		
	if ( *(ULong *) readBuf == kbID )					// found a good Eric Smith Keyboard
		return ( true );
	else
		return ( false );
}


//
//----- Controller Read routines
//

void	ReadEAMulti ( short* results, short flags )
{
	ReadEAorSingles ( results, (flags | kReadEAMulti) );
}


void
ReadEAorSingles ( short* results, short flags )
{
register			UChar*	resultPtr = (UChar*) results;
volatile register	UChar*	reg = (UChar*) kData1;
volatile register	UChar*	reg5 = (UChar*) 0xa10005;
volatile register	UChar*	regb = (UChar*) 0xa1000b;
register			UChar	data;
short				leftToRead = 1;
UChar				EAMultiPort;

	if (flags & kReadEAMulti) {
		leftToRead = 4;							// read all four controls
		*reg5 = EAMultiPort = kEAMultiBase;		// have to set up controller port					
	} 
	do {
		*(reg + kCtrl) = kTH;		// TH is output, others are input
		*reg = kTH + kTR;			// TH is high, TR will be high (maybe)
		nop;
		nop;
		nop;
		*reg = 0;							// lower TH
		nop; nop;							// settle for 2
		nop;								// make that 3...  (for older boxes)
		data = *reg;						// get start, A, down, up
		*reg = kTH;							// raise TH
		data = (data << 2) & kAToStart;		// this burns the usual NOP time....
		data |= *reg & kUpToC;				// insert other 6 buttons
		resultPtr[1] = ~data;				// write the result (NOTE - big endian)
		
		if (flags & kWant6Buttons)
			{
			*reg = 0;							// now cycle through modes
			nop; nop; nop; nop;					// looking for the 6-button ID
			*reg = kTH;
			nop; nop; nop; nop;
			*reg = 0; 							// this is cycle 6
			nop; nop;							// and if it's a 6button pad,
			if ((*reg & 0x0F) == 0)				// we'll see zeros at the bottom
				{
				*reg = kTH;						// get 6button data, if we found one
				nop; nop;
				resultPtr[0] = ~(*reg) & (kZToMode >> 8);	// big endian
				}
			else
				*reg = kTH;						// make sure it's left HI
			}

		resultPtr += 2;							// skip to next short result
		if (flags & kReadEAMulti) {
			EAMultiPort += 0x10;				// next controller (0c,1c,2c,3c) = (1,2,3,4)
			*reg5 = EAMultiPort;				// load address value for next controller to be read
		} else {
			reg += kNextPort;					// just move to next register
		}
	} while (leftToRead--);						// all controllers
}

void
ReadSegaMulti ( short* results, short flags )
{
register			short*		resultPtr = results;
					UChar		readBuf[8];
register			UChar*		readScan = readBuf;
volatile register	UChar*		reg = (UChar*) kData1;
					short		hshkState;
	
	*(reg)			= kTH + kTR;					// both flags hi
	*(reg + kSer)	= 0;							// clear serial modes
	*(reg + kCtrl)	= kTH + kTR;					// both flags are outputs now
	
	nop; nop;
	*readScan++ = *reg & 0x0F;						// get 1st data nibble

	*reg = kTR;										// turn off TH to start hshk seq
	nop; nop;
	*readScan++ = *reg & 0x0F;						// get 2nd data nibble
	
	hshkState = 0;									// start flipping TR
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 3rd data nibble
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 4th data nibble
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 5th data nibble
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 6th data nibble
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 7th data nibble
	*readScan++ = GetHandshakeNibble(&hshkState);		// get 8th data nibble
	
	if ( *(ULong *) readBuf == 0x030f0000 )				// found a good tap
		{
		short	xx;
		short	value;
		
		readScan = readBuf + 4;
		for (xx = 3; xx >= 0; --xx)
			{
			UChar	type = *readScan++;
			if (type == kSega3PadID)
				{
				value = GetHandshakeNibble(&hshkState);
				value |= GetHandshakeNibble(&hshkState) << 4;
				value = value ^ 0xFF;
				}
			else
			if (type == kSega6PadID)
				{
				value = GetHandshakeNibble(&hshkState);
				value |= GetHandshakeNibble(&hshkState) << 4;
				value |= GetHandshakeNibble(&hshkState) << 8;
				value = value ^ 0xFFF;
				}
			else
			if (type == kSegaMouseID)
				{
				GetHandshakeNibble(&hshkState);		// discard 6 nibbles
				GetHandshakeNibble(&hshkState);
				GetHandshakeNibble(&hshkState);
				GetHandshakeNibble(&hshkState);
				GetHandshakeNibble(&hshkState);
				GetHandshakeNibble(&hshkState);
				value = 0;
				}
			else
				value = 0x0000;						// unknown, return "0"

			*results++ = value;
			}
		*results = 1;								// set team tap mode
		}
	*reg = kTH + kTR;			// make sure we're left HI to reset the tap
}


#define kESKeycodeData			0
#define kESControllerEcho		1
#define kESControllerVersion	2

void
ReadESKeyboard ( void )
{
					UChar		readBuf[4];
register			UChar*		readScan = readBuf;
volatile register	UChar*		reg = (UChar*) kData2;		// only support keyboard on PORT 2!
					short		hshkState;
					UChar		len;
					UChar		temp;
register			long		timeout = 100;
register			ULong		kbID = REFGLOBAL( controls, keyboardID );
	
	*(reg)				= kTH + kTR;						// both flags hi
	*(char *)kSerial2	= 0;								// clear serial modes
	*(char *)kCtl2		= kTH + kTR;						// both flags are outputs now
	
	nop; nop;
	*readScan++ = *reg & 0x0F;								// 1st nybble = identifying ID

	*reg = kTR;												// turn off TH to start hshk seq

	do {
		*readScan = *reg & 0x0F;
		}
		while ( (*readScan != ((kbID >> 16) & 0xF)) && --timeout );		// 2nd nybble has no handshake

	if ( !timeout )
	{
		*reg = kTH + kTR;								// make sure we leave with TH & TR hi
		return;
	}
	
	readScan++;							
	
	hshkState = 0;											// start flipping TR
	*readScan++ = GetHandshakeNibblePort2(&hshkState);		// 3rd nybble = local ID
	*readScan++ = GetHandshakeNibblePort2(&hshkState);		// 4th nybble = local ID

	if ( *(ULong *) readBuf == kbID )						// found a good Eric Smith Keyboard
	{
		len = GetHandshakeNibblePort2(&hshkState);			// 5th nybble = BYTE count, 0-15
		
		if (len)
		{
			temp = GetHandshakeNibblePort2(&hshkState);				// get data type
			temp <<= 4;
			temp |= GetHandshakeNibblePort2(&hshkState);	
		
			len--;													// len includes data type byte	
		
			if ( temp == kESKeycodeData )
			{														// Key Codes follow
				readScan = REFGLOBAL( controls, keycodeBuf );
				while ( len )
				{
					REFGLOBAL( controls, keycodeHead )++;						// bump head
					REFGLOBAL( controls, keycodeHead ) &= kKeybdDataFifoMask;	// circular buf
					temp = GetHandshakeNibblePort2(&hshkState);		
					temp <<= 4;
					temp |= GetHandshakeNibblePort2(&hshkState);	
					readScan[REFGLOBAL( controls, keycodeHead )] = temp;
					len--;
				}
			}
			else
			{														// Status Bytes follow
				readScan = REFGLOBAL( controls, statusBuf );
				while ( len )
				{
					REFGLOBAL( controls, statusHead )++;							// bump head
					REFGLOBAL( controls, statusHead ) &= kKeybdCmdStatusFifoMask;	// circular buf
					temp = GetHandshakeNibblePort2(&hshkState);		
					temp <<= 4;
					temp |= GetHandshakeNibblePort2(&hshkState);	
					readScan[REFGLOBAL( controls, statusHead )] = temp;
					len--;
				}
			}
		}
		*reg = kTH + kTR;								// make sure we leave with TH & TR hi
	}
		
}


/*
	
	3	C	6	0	<len>	<hi nyb>   <low nyb> ....
      			^	\____________________________..../
      			|	     Same Format as Dat/Status
 				|
      			Driven to tell Eric I have something for him
				
*/

	
void
WriteESKeyboard ( void )
{
					UChar		readBuf[4];
register			UChar*		readScan = readBuf;
volatile register	UChar*		reg = (UChar*) kData2;			// only support keyboard on PORT 2!
					short		hshkState;
register			long		timeout = 100;
register			ULong		kbID = REFGLOBAL( controls, keyboardID );
					UChar		byteToSend;
					short		byteCount;
	
	if ( REFGLOBAL( controls, keyboardPresent ) &&
			 (REFGLOBAL( controls, cmdTail ) != REFGLOBAL( controls, cmdHead )) )
	{	
		*(reg)				= kTH + kTR;						// both flags hi
		*(char *)kSerial2	= 0;								// clear serial modes
		*(char *)kCtl2		= kTH + kTR;						// both flags are outputs now
	
		nop; nop;
		*readScan++ = *reg & 0x0F;								// 1st nybble = identifying ID

		*reg = kTR;												// turn off TH to start hshk seq

		do {
			*readScan = *reg & 0x0F;
			}
			while ( (*readScan != ((kbID >> 16) & 0xF)) && --timeout );		// 2nd nybble has no handshake

		if ( !timeout )
		{
			*reg = kTH + kTR;									// make sure we leave with TH & TR hi
			return;
		}
	
		readScan++;							
	
		hshkState = 0;											// start flipping TR
		*readScan++ = GetHandshakeNibblePort2(&hshkState);		// 3rd nybble = local ID


		if ( (*(ULong *) readBuf & 0xFFFFFF00) == (kbID & 0xFFFFFF00) )		// found a good Eric Smith Keyboard?
		{
			*reg &= 0xF0;										// ensure data lines are 0
			*(char *)kCtl2 |= kDataLines;						// 4 data lines are outputs now

			REFGLOBAL( controls, cmdTail )++;
			REFGLOBAL( controls, cmdTail ) &= kKeybdCmdStatusFifoMask;
			byteToSend = REFGLOBAL( controls, cmdBuf )[REFGLOBAL( controls, cmdTail )];

			PutHandshakeNibblePort2(&hshkState, 0);				// 4th nybble = 0 ==> I'm talking to him
			PutHandshakeNibblePort2(&hshkState, 2);				// 2 bytes follow; type & data
			
			PutHandshakeNibblePort2(&hshkState, ((kESKeycodeData & 0xF0)>>4));		
			PutHandshakeNibblePort2(&hshkState, (kESKeycodeData & 0x0F));			// 1st byte = type
			
			PutHandshakeNibblePort2(&hshkState, ((byteToSend & 0xF0)>>4));		
			PutHandshakeNibblePort2(&hshkState, (byteToSend & 0x0F));				// 2nd byte = data

			*(char *)kCtl2 &= ~kDataLines;						// 4 data lines are back to being inputs
			*reg = kTH + kTR;									// make sure we leave with TH & TR hi
		}
	}
		
}


//
//----- Misc helper routines
//

short
GetHandshakeNibble( short* hshkState )
{
register 			long		timeout = 100;
volatile register	UChar*		reg = (UChar*) kData1;
	
	if (*hshkState == -1)	// timed out, abort (see below)
		return 0x0F;
	
	if ((*hshkState ^= 1) == 0)
		{
		*reg |= kTR;			// raise TR, wait for TL high
		nop; nop;
		do {}
			while (!(*reg & kTL) && --timeout);
		if (timeout)
			{
			nop; nop;
			return *reg & 0x0F;
			}
		}
	else
		{
		*reg &= ~kTR;			// lower TR, wait for TL low
		nop; nop;
		do {}
			while ((*reg & kTL) && --timeout);
		if (timeout)
			{
			nop; nop;
			return *reg & 0x0F;
			}
		}
		
	// if we got this far, we've timed out.  start returning
	// F's for everything else (because these will be inverted to zeros)
	// so we just abort the entire read sequence
	
	*hshkState = -1;
	return 0x0F;
}


short
GetHandshakeNibblePort2( short* hshkState )
{
register 			long		timeout = 100;
volatile register	UChar*		reg = (UChar*) kData2;
	
	if (*hshkState == -1)	// timed out, abort (see below)
		return 0x0F;
	
	if ((*hshkState ^= 1) == 0)
		{
		*reg |= kTR;			// raise TR, wait for TL high
		nop; nop;
		do {}
			while (!(*reg & kTL) && --timeout);
		if (timeout)
			{
			nop; nop;
			return *reg & 0x0F;
			}
		}
	else
		{
		*reg &= ~kTR;			// lower TR, wait for TL low
		nop; nop;
		do {}
			while ((*reg & kTL) && --timeout);
		if (timeout)
			{
			nop; nop;
			return *reg & 0x0F;
			}
		}
		
	// if we got this far, we've timed out. return 0xFFs to abort.
	
	*hshkState = -1;
	return 0xFF;
}


/* By the time we get here, the 4 data lines have been set up as outputs.
   The caller must ensure that there is nothing in the upper nybble of byteToSend.
 */
 
void
PutHandshakeNibblePort2( short* hshkState, unsigned char byteToSend )
{
register 			long		timeout = 100;
volatile register	UChar*		reg = (UChar*) kData2;
	
	if (*hshkState == -1)	// timed out, abort (see below)
		return;
	
	*reg = (*reg & 0xF0) | byteToSend;	// up to caller to be sure nothing is in hi nybble
	
	if ((*hshkState ^= 1) == 0)
		{
		*reg |= kTR;					// raise TR, wait for TL high
		nop; nop;
		do {}
			while (!(*reg & kTL) && --timeout);
		if ( timeout )
			return;
		}
	else
		{
		*reg &= ~kTR;					// lower TR, wait for TL low
		nop; nop;
		do {}
			while ((*reg & kTL) && --timeout);
		if ( timeout )
			return;
		}
		
	// if we got this far, we've timed out. return 0xFFs to abort.
	*hshkState = -1;
}




void
DeglitchControls ( short* results, short flags )
{
	// post-process controller data

	asm	{
		movem.l		d0-d1/a0-a1,-(a7)
		move.l		results,a0					// pointer to 4 short results
		lea			@glitchTable,a1				// pointer to deglitch table
		
		move.b		1(a0),d0					// get lo byte of first result
		beq.s		@skFix1						// buttons up so ignore it
		andi.w		#0x0F,d0					// just looking at bottom 4
		move.b		(a1,d0.w),d1				// get new table entry
		andi.b		#0xF0,1(a0)					// clear area from old value
		or.b		d1,1(a0)					// insert it into old value
@skFix1
		move.b		3(a0),d0
		beq.s		@skFix2
		andi.w		#0x0F,d0
		move.b		(a1,d0.w),d1
		andi.b		#0xF0,3(a0)
		or.b		d1,3(a0)
@skFix2
		btst      	#1,flags+1					// kWant4Players ?
		beq.s		@skTable					// no, don't waste time on #3 and #4
		
		move.b		5(a0),d0
		beq.s		@skFix3
		andi.w		#0x0F,d0
		move.b		(a1,d0.w),d1
		andi.b		#0xF0,5(a0)
		or.b		d1,5(a0)
@skFix3
		move.b		7(a0),d0
		beq.s		@skFix4
		andi.w		#0x0F,d0
		move.b		(a1,d0.w),d1
		andi.b		#0xF0,7(a0)
		or.b		d1,7(a0)
@skFix4
		bra.s		@skTable
@glitchTable
		dc.b		0x00, 0x01, 0x02, 0x02		// prevents U+D or L+R
		dc.b		0x04, 0x05, 0x06, 0x06
		dc.b		0x08, 0x09, 0x0A, 0x0A
		dc.b		0x08, 0x09, 0x0A, 0x0A
@skTable
		movem.l		(a7)+,d0-d1/a0-a1
	}	// end of ASM
}


#endif			//  SIMULATOR



//
// Eric Smith Hardware Keyboard Routines
//


Boolean _FlushHardwareKeyboardBuffer( void )
{
	REFGLOBAL( controls, sysKeysTail ) = REFGLOBAL( controls, sysKeysHead );
	
	if ( REFGLOBAL( controls, keyboardPresent ) )
		return( true );
	else
		return( false );
}


unsigned char _GetNextESKeyboardRawcode( void )
{
	if ( REFGLOBAL( controls, keyboardPresent ) &&
			(REFGLOBAL( controls, keycodeTail ) != REFGLOBAL( controls, keycodeHead )) )
	{
		REFGLOBAL( controls, keycodeTail )++;
		REFGLOBAL( controls, keycodeTail ) &= kKeybdDataFifoMask;
		return( REFGLOBAL( controls, keycodeBuf )[REFGLOBAL( controls, keycodeTail )] );
	}
	else
		return( 0xFF );
}


unsigned char _GetNextESKeyboardStatus( void )
{
	if ( REFGLOBAL( controls, keyboardPresent ) &&
			(REFGLOBAL( controls, statusTail ) != REFGLOBAL( controls, statusHead )) )
	{
		REFGLOBAL( controls, statusTail )++;
		REFGLOBAL( controls, statusTail ) &= kKeybdCmdStatusFifoMask;
		return( REFGLOBAL( controls, statusBuf )[REFGLOBAL( controls, statusTail )] );
	}
	else
		return( 0xFF );
}


void _SendCmdToESKeyboard( unsigned char *cmdBuf, unsigned char cmdLen )
{
unsigned short oldSR;

	while ( cmdLen )
	{

		asm { 	move.w	sr,oldSR
				move.w	#0x2700,sr
			}
		REFGLOBAL( controls, cmdHead )++;
		REFGLOBAL( controls, cmdHead ) &= kKeybdCmdStatusFifoMask;
		REFGLOBAL( controls, cmdBuf )[REFGLOBAL( controls, cmdHead )] = *cmdBuf;
		asm { 	move.w	oldSR,sr	}

		cmdBuf++;		
		cmdLen--;
	}
}


unsigned char _GetNextHardwareKeyboardChar( void )
{
	if ( REFGLOBAL( controls, keyboardPresent ) &&
			(REFGLOBAL( controls, sysKeysTail ) != REFGLOBAL( controls, sysKeysHead )) )
	{
		REFGLOBAL( controls, sysKeysTail )++;
		REFGLOBAL( controls, sysKeysTail ) &= kSysKeysFifoMask;
		return( REFGLOBAL( controls, sysKeysBuf )[REFGLOBAL( controls, sysKeysTail )] );
	}
	else
		return( kNoKey );
}


long _GetHardwareKeyboardFlags( void )
{
	return( REFGLOBAL( controls, keyboardFlags ) );
}


void _SetHardwareKeyboardFlags( long flags )
{
unsigned char fuck[2];

	REFGLOBAL( controls, keyboardFlags ) = flags;
	fuck[0] = 0xED;						// hit the LED reg
	fuck[1] = flags & 0x7; 				// bits [2:0] are caps/num/scroll lock
	SendCmdToESKeyboard( fuck, 2 );
}




void BackUpKeycodeTail( void )
{
	if ( REFGLOBAL( controls, keycodeTail ) == 0 )				// back up; we'll try again later
		REFGLOBAL( controls, keycodeTail ) = kKeybdDataFifoMask;
	else
		REFGLOBAL( controls, keycodeTail )--;
}



unsigned char _GetNextESKeyboardChar( void )
{
unsigned char raw;
unsigned char map;
unsigned long offset;
unsigned char fuck[2];
Boolean 	  specialPending;
	
	specialPending = false;
	map = kNoKey;
	
	if ( REFGLOBAL( controls, keyMapTable ) )
	{
		raw = GetNextESKeyboardRawcode();			// get next code out of buffer
		
		//
		// BREAK (KEYUP) CODES
		//
		
		if ( raw == 0xF0 )								
		{
			raw = GetNextESKeyboardRawcode();								// try to get the one that's breaking
			
			if ( raw == 0xFF )												// not in buf yet?
			{
				BackUpKeycodeTail();
				return( kNoKey );
			}
				
			if ( raw < (REFGLOBAL( controls, keyMapTable )->tblSectionSize) )		// valid code?
			{
				map = (REFGLOBAL( controls, keyMapTable )->mapTbl)[raw];
				
				if ( map == kShiftKey )				
					REFGLOBAL( controls, keyboardFlags ) &= ~kShiftDown;

				if ( map == kAltKey )				
					REFGLOBAL( controls, keyboardFlags ) &= ~kAltDown;

				if ( map == kControlKey )				
					REFGLOBAL( controls, keyboardFlags ) &= ~kControlDown;

				if ( map == kCapsLockKey )			
					REFGLOBAL( controls, keyboardFlags ) &= ~kCapsLockDown;		// caps lock up
			}
			
			return( kNoKey );
		}
		
		//
		// 101-STYLE CODES
		//

		if ( raw == 0xE0 )													
		{
			raw = GetNextESKeyboardRawcode();						// get key code
			
			if ( raw == 0xFF )										// special key there yet?
			{
				BackUpKeycodeTail();
				return( kNoKey );
			}
			else													// special key *is* there
			{
				if ( raw == 0xF0 )									// just trash breaks
				{
					raw = GetNextESKeyboardRawcode();				// try to get one to break
					if ( raw == 0xFF )								// key to break here?
					{
						BackUpKeycodeTail();
						BackUpKeycodeTail();
					}
					return( kNoKey );
				}
				specialPending = true;				// flag it special
			}
		}
		
		//
		// NORMAL CODES
		//
		
		if ( raw < (REFGLOBAL( controls, keyMapTable )->tblSectionSize) )		// valid code?
		{
			map = (REFGLOBAL( controls, keyMapTable )->mapTbl)[raw];			// map to lower case

			if ( REFGLOBAL( controls, keyboardFlags ) & kShiftDown )			// shift key -> ALL shifted
			{
				if ( (map >= 0x20) && (map <= 0x7E) )
				{
					offset = raw + REFGLOBAL( controls, keyMapTable )->tblSectionSize;	// use upper tbl
					map = (REFGLOBAL( controls, keyMapTable )->mapTbl)[offset];			// map to shifted
				}	
			}
			else
				if ( REFGLOBAL( controls, keyboardFlags ) & kCapsLocked )			// capslock key -> CHARS shifted
				{
					if ( (map >= 'a') && (map <= 'z') )
					{
						offset = raw + REFGLOBAL( controls, keyMapTable )->tblSectionSize;	// use upper tbl
						map = (REFGLOBAL( controls, keyMapTable )->mapTbl)[offset];			// map to shifted
					}
				}
		}
		
		//
		// META KEYS
		//
			
		switch(map)
		{	
			case kShiftKey:
					REFGLOBAL( controls, keyboardFlags ) |= kShiftDown;
					return( kNoKey );
					break;
		
			case kAltKey:
					REFGLOBAL( controls, keyboardFlags ) |= kAltDown;
					return( kNoKey );
					break;
		
			case kControlKey:
					REFGLOBAL( controls, keyboardFlags ) |= kControlDown;
					return( kNoKey );
					break;
		}
		
		if ( (map == kCapsLockKey) && !(REFGLOBAL( controls, keyboardFlags ) & kCapsLockDown) )
		{
			REFGLOBAL( controls, keyboardFlags ) |= kCapsLockDown;
			REFGLOBAL( controls, keyboardFlags ) ^= kCapsLocked;			// flip caps lock state
			fuck[0] = 0xED;													// hit the LED reg
			fuck[1] = REFGLOBAL( controls, keyboardFlags ) & kCapsLocked; 	// bits [2:0] are caps/num/scroll lock
			SendCmdToESKeyboard( fuck, 2 );
			return( kNoKey );
		}

		//
		// Special Keys
		//

		if ( !(REFGLOBAL( controls, keyboardFlags ) & kReturnIsLF) && (map == 0x0A) )
			map = kEnterKey;

		if ( specialPending )
		{
			switch( raw )
			{
				case 0x75:							// up arrow
							map = kUpArrowKey;
							break;
				case 0x72:							// down arrow
							map = kDownArrowKey;
							break;
				case 0x6B:							// left arrow
							map = kLeftArrowKey;
							break;
				case 0x74:							// right arrow
							map = kRightArrowKey;
							break;
				case 0x5A:
							map = kEnterKey;
							break;
				default:
							map = kNoKey;
							break;
			}
		}	

	}
		
	return( map );
}


void EmulateJoypadWithKeyboard( short *results )
{
unsigned char key;
register short pad1 = results[0];

	key = GetNextESKeyboardChar();

	switch( key )
	{
		case kUpArrowKey:
						if ( !(pad1 & kDOWN ) )
							pad1 |= kUP;
						break;
						
		case kDownArrowKey:
						if ( !(pad1 & kUP ) )
							pad1 |= kDOWN;
						break;
						
		case kLeftArrowKey:
						if ( !(pad1 & kRIGHT ) )
							pad1 |= kLEFT;
						break;
						
		case kRightArrowKey:
						if ( !(pad1 & kLEFT ) )
							pad1 |= kRIGHT;
						break;
						
		case kEnterKey:
						pad1 |= kButtonA;
						break;
													
		default:												
						if ( REFGLOBAL( controls, keyboardFlags ) & kAltDown )
						{
							switch( key )
							{
								case 'A':
								case 'a':
											pad1 |= kButtonA;
											break;				
								case 'B':	
								case 'b':	
											pad1 |= kButtonB;
											break;
								case 'C':	
								case 'c':	
											pad1 |= kButtonC;
											break;
								case 'S':	
								case 's':	
											pad1 |= kStart;
											break;
							}
						}
						else
						{
							REFGLOBAL( controls, sysKeysHead )++;		/* not pad-like, stick in key buf */
							REFGLOBAL( controls, sysKeysHead ) &= kSysKeysFifoMask;
							REFGLOBAL( controls, sysKeysBuf )[REFGLOBAL( controls, sysKeysHead )] = key;
						}
						break;
	}
	
	results[0] = pad1;
}



