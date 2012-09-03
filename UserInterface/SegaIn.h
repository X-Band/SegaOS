/*
	File:		SegaIn.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 8/23/94	CMY		Added a definition for when any button (but no direction) is
									pressed on the controller.
		<12>	 7/27/94	BET		dunno
		<12>	 7/27/94	BET		#ifndef wrapper on __SERVER__
		<11>	 7/21/94	ADS		move STIM-only  keyboard stuff to controlsPriv.h
		<10>	 7/13/94	ADS		Add controller-2 keyboard equiv's
		 <9>	 7/13/94	ADS		Fixed controller constants (maybe not final)
		 <8>	  7/5/94	ADS		Added new simulator keycodes
		 <7>	  7/4/94	SAH		Killed a bunch of unused stuff and managerized GetNextCommand.
		 <6>	  7/1/94	CMY		Changed modem-connect key to be tilde instead of delete.
		 <5>	 6/10/94	KON		Changed ReadControllers to GetNextCommand..
		 <4>	  6/9/94	SAH		Added some sega buttons.
		 <3>	  6/2/94	CMY		Add support for multiline text edit.
		 <2>	 5/30/94	CMY		Remove globals from keyboard input. Use cursor manager.
		 <2>	 5/24/94	SAH		Added a prototype for isPressed.

	To Do:
*/

#ifndef __SegaIn__
#define __SegaIn__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif


#ifndef	__SERVER__
void GetNextCommand( short *num1, short *num2 ) = 
	CallDispatchedFunction ( kGetNextCommand );
#endif

#ifdef SIMULATOR
extern short isPressed( unsigned short k );
#endif




/********************	Taken out **********************/
#ifdef NEVER

#ifndef __Cursor__
#include "Cursor.h"
#endif

#ifndef __TextUtls__
#include "TextUtls.h"
#endif

typedef struct {
	char				shiftKeyState;
	char				cursorIsThin;
	short				cursorX;
	short				cursorY;
	TextEditReference	textEdit;
	CursorPtr			thinCursor;
	CursorPtr			wideCursor;
} KeyboardState;


unsigned char InputSegaChar( KeyboardState *keyboardStuff );
unsigned char GetCurrentKeyboardChar( KeyboardState *keyboardStuff );
void MoveSelectionSprite( short num1, KeyboardState *keyboardStuff );
unsigned char InputSegaString( char *theString, short length,
		short keyboardX, short keyboardY, Rect *textRect );
void SetupKeyboard( void );
unsigned char AnswerQuestion( char *theQuestion, char *theAnswer, short length );
void UpdateSprite( void );

#endif
/******************************************************/


// controller reads return a short containing all possible
// key values.  This format is intended to be really close
// to that used by most games, so we can use our controller
// routines from them and keep the game patches smaller.


//	15	14	13	12	11	10	 9	 8	 7	 6	 5	 4	 3	 2	 1	 0
//	-unused, zero-	Md	 X	 Y	 Z	St	 A	 C	 B	Rt	Lf	Dn	Up


#define kUP						0x01
#define kDOWN					0x02
#define kLEFT					0x04
#define kRIGHT					0x08
#define	kButtonB				0x10		// note B & C are reversed
#define	kButtonC				0x20
#define	kButtonA				0x40
#define	kStart					0x80
#define kButtonZ				0x100
#define kButtonY				0x200
#define kButtonX				0x400
#define kMode					0x800

#define kModemTryingToConnect	0x0f	//Magic return value to indicate
										//modem is trying to connect

#define kButtonPressed			0x07f0
#define kABCXYZButtonPressed 	(kButtonA | kButtonB | kButtonC | kButtonX | kButtonY | kButtonZ)

//
// Constants
//

#define kKeyboardYBump	23
#define kKeyboardXBump	23

#define kCANCEL			0xFC
#define kSHIFT			0xFD
#define kRUB			0xFE
#define kEND			0xFF

#define kCANCELLocation	(kKeyboardXBump*('G'-'A'+1))
#define kSHIFTLocation	(kKeyboardXBump*('G'-'A'+1))
#define kRUBLocation	(kKeyboardXBump*('G'-'A'+1))
#define kENDLocation	(kKeyboardXBump*('G'-'A'+1))

#define kSHIFTPressed	1

#define kKeyboardOffsetX	64

#define kKeyboardOffsetY	128



#endif __SegaIn__
