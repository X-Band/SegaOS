/*
	File:		SegaIn.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<20>	 8/10/94	JBH		Fixed call to InitTextEdit to include useCursor flag.
		<19>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<18>	 7/21/94	ADS		Moved special STIM keyboard stuff to controls
		<17>	  7/4/94	SAH		Stripped most everything and managerized GetNextCommand through
									the events manager.
		<16>	  7/1/94	CMY		Changed modem connection simulator to use tilde instead of
									delete key.
		<15>	 6/16/94	CMY		Now uses event manager rather than ReadHardwareController.
		<14>	 6/16/94	BET		No it doesn't
		<13>	 6/15/94	HEC		SegaOSIdle takes a param now
		<12>	 6/13/94	SAH		Brought in modem misc stuff from controls.c
		<11>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		<10>	 6/10/94	KON		Changed ReadControllers to GetNextCommand.
		 <9>	  6/5/94	KON		Remove references to keyboard graphics so we can compile for
									digital world.
		 <8>	  6/3/94	DJ		updating to revised DrawDBGraphic call
		 <7>	  6/3/94	CMY		Made the threaded keyboard input a bit less bouncy. I guess this
									was trickier than I thought.
		 <6>	  6/3/94	BET		Fix a keyboard bug with threads off. {CTY}
		 <5>	  6/2/94	CMY		Modify keyboard input to use text edit.
		 <4>	 5/31/94	SAH		Killed SegaMisc.h.
		 <3>	 5/30/94	CMY		Remove globals from keyboard input. Use cursor manager.
		 <2>	 5/29/94	KON		Updated to use graphics database.
		 <7>	 5/24/94	SAH		Moved the ReadASegaController crap to contols.c
		 <6>	 5/20/94	SAH		Updated call to LinearizeScreenArea.
		 <5>	 5/19/94	KON		Remove the 5 pages of shit projector added to my last checkin.
		 <4>	 5/18/94	KON		Use events to time user events.
		 <3>	 5/18/94	HEC		long pattern types
		 <2>	 5/17/94	KON		Update to use Get and Set FontColors.

	To Do:
*/


#ifdef SIMULATOR
#include "Main.h"
#include "BigEasy.h"
#endif

#include "SegaOS.h"
#include "SegaIn.h"
#include "Events.h"
#include "ModemMisc.h"
#include "EventsPriv.h"



void _GetNextCommand( short *num1, short *num2 )
{
	if( IsRemoteModemTryingToConnect() )
		{
		*num1 =  kModemTryingToConnect;
		*num2 = 0;
		return;
		}

#ifdef SIMULATOR
//
// Simulate remote modem connect when the deleteKey is pressed
//		
#define tildeKey		0x36
	if( isPressed( tildeKey ) )
		{
		SetRemoteModemTryingToConnectState( true );
		}
#endif

	*num1 = GetNextControllerEvent ( 1 );
	*num2 = GetNextControllerEvent ( 2 );
}


#ifdef SIMULATOR
extern short gTheKeyCode;
short isPressed( unsigned short k )
// k =  any keyboard scan code, 0-127
{
	unsigned char km[16];

	GetKeys( (long *) km);
	if ( ( km[k>>3] >> (k & 7) ) & 1) {
		return 1;
	} else {
		return 0;
	}
}
#endif




/* sorry KON, but I killed all this stuff... */
#ifdef NEVER


//
// Prototypes
//

static void SetupBarCursor( void );

#ifdef SIMULATOR

extern void DrawAWindow( WindowGlobalsPtr wGlobals );

#endif

void DoSpecialKeys( unsigned char *theChar );


void SetupBarCursor( void )
{
#if 0
// SetSprite 0 pattern table to kSpriteBase+24 and disable Sprite 1

	OwnVDP();
	
// sprite 0
	SetVDPIncrement( 2 );			//increment 2 bytes at a time
	SetVDPVRAMAddress( 0xA002 );
	WriteWordToVDP( 0x0101 );			//sprite size (8x16) and link data: 1
	WriteWordToVDP( 0xE000+kSpriteBase+20 );	//priority, color palette, flipping, pattern number
	
	FreeVDP();
#endif
}

static void UpdateKeyboardCursor(KeyboardState *keyboardStuff)
{
	MoveSegaCursor(keyboardStuff->thinCursor, keyboardStuff->cursorX+kKeyboardOffsetX,
		keyboardStuff->cursorY+kKeyboardOffsetY);
	MoveSegaCursor(keyboardStuff->wideCursor, keyboardStuff->cursorX+kKeyboardOffsetX,
		keyboardStuff->cursorY+kKeyboardOffsetY);
	SegaOSIdle();
}

static void MakeCursorWide(KeyboardState *keyboardStuff)
{
	if (keyboardStuff->cursorIsThin) {
		HideSegaCursor(keyboardStuff->thinCursor);
		ShowSegaCursor(keyboardStuff->wideCursor);
		keyboardStuff->cursorIsThin = 0;
	}
}

static void MakeCursorThin(KeyboardState *keybo