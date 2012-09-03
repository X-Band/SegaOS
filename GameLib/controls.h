/*
	File:		controls.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 8/26/94	JOE		added kReturnIsLF flag
		 <7>	 8/25/94	JOE		mo' hardware keybo'
		 <6>	 8/25/94	JOE		hardware keyboard stuff
		 <5>	  8/6/94	ADS		New interface for types & reading
		 <4>	  8/1/94	MWF		Finalize new controller system
		 <4>	  8/1/94	MWF		add InitControllers
		 <3>	 7/25/94	ADS		Finalize new controller system
		 <2>	 7/21/94	ADS		Lotsa new stuff to support new architecture
		 <1>	 5/24/94	SAH		Added ReadHardwareController.

	To Do:
*/


#ifndef _CONTROLS_H_
#define _CONTROLS_H_


// --------------------------------------
// general OS user-interface code calls this one.
//	whichController = 1..2

short			ReadHardwareController ( short whichController ) =
	CallDispatchedFunction( kReadHardwareController );


// --------------------------------------
// general OS user-interface code calls this one to read
//  a hardware keyboard, if we have one.  Returns 0xFF if no char available.

#define kUpArrowKey			0xFD
#define	kDownArrowKey		0xFC
#define	kRightArrowKey		0xFB
#define	kLeftArrowKey		0xFA
#define	kAltKey				0xF9
#define	kControlKey			0xF8
#define	kEnterKey			0xF7
#define	kNoKey				0x00

#define	kAKey				0x1C
#define	kBKey				0x32
#define	kCKey				0x21
#define	kSKey				0x1B

/* keybd state flags */
#define	kReturnIsLF			0x01000000
#define	kControlDown		0x00000040
#define	kAltDown			0x00000020
#define	kShiftDown			0x00000010
#define	kCapsLockDown		0x00000008
#define	kCapsLocked			0x00000004
#define	kNumLocked			0x00000002
#define	kScrollLocked		0x00000001

Boolean 		FlushHardwareKeyboardBuffer( void ) =
	CallDispatchedFunction( kFlushHardwareKeyboardBuffer );
	
unsigned char 	GetNextHardwareKeyboardChar( void ) =
	CallDispatchedFunction( kGetNextHardwareKeyboardChar );

long 			GetHardwareKeyboardFlags( void ) =
	CallDispatchedFunction( kGetHardwareKeyboardFlags );

void 			SetHardwareKeyboardFlags( long flags ) =
	CallDispatchedFunction( kSetHardwareKeyboardFlags );

// --------------------------------------
// lower level routines for the hardware keyboard

unsigned char 	GetNextESKeyboardRawcode( void ) =
	CallDispatchedFunction( kGetNextESKeyboardRawcode );

unsigned char 	GetNextESKeyboardStatus( void ) =
	CallDispatchedFunction( kGetNextESKeyboardStatus );

void 			SendCmdToESKeyboard( unsigned char *cmdBuf, unsigned char cmdLen ) =
	CallDispatchedFunction( kSendCmdToESKeyboard );

unsigned char 	GetNextESKeyboardChar( void ) =
	CallDispatchedFunction( kGetNextESKeyboardChar );


// --------------------------------------
//	this should only be called by the VBL task!!!!

void	ControllerVBL( void ) =
	CallDispatchedFunction( kControllerVBL );


// --------------------------------------
//	these published ONLY for use by game patches!!!

typedef long ControllerType;		// cookie returned by InitControllers
enum	{							// flags for ReadAllControllers
	kWant6Buttons = 1,
	kWant4Players = 2
};

ControllerType	InitControllers( void ) =
	CallDispatchedFunction( kInitControllers );

void	ReadAllControllers( ControllerType type, short* results, short flags ) =
	CallDispatchedFunction( kReadAllControllers );


#endif		// _CONTROLS_H_




