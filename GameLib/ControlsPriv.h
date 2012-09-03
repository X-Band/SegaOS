/*
	File:		ControlsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/25/94	JOE		hardware keyboard stuff
		 <8>	 8/17/94	JOE		Add Button X, Y, & Z support to stimulator
		 <7>	  8/8/94	ADS		Read 15x per second instead of 60x
		 <6>	  8/6/94	ADS		New interface for types & reading
		 <5>	  8/1/94	MWF		Finalize new controller system
		 <5>	 8/01/94	MWF		support for different multi-types
		 <4>	 7/25/94	ADS		Finalize new controller system
		 <3>	 7/23/94	ADS		more changes to data structures
		 <2>	 7/21/94	ADS		Lotsa new stuff to support new architecture

	To Do:
*/






#ifndef __CONTROLS_PRIV_H__
#define __CONTROLS_PRIV_H__

#include "Controls.h"


#define	kKeybdDataFifoMask		0xF
#define	kSysKeysFifoMask		0xF
#define	kKeybdCmdStatusFifoMask	0x7

/* special keymap vals */
#define	kShiftKey			0xFF
#define	kCapsLockKey		0xFE

typedef
struct KeyMappingTable
{
	unsigned short	tblSectionSize;
	char			mapTbl[1];
} KeyMappingTable;

typedef
struct ControlGlobals
{
	short			controlValues[5];		// 4 controllers, isMulti
	unsigned char	sysKeysBuf[16];
	unsigned char	sysKeysHead,sysKeysTail;
	unsigned char	keycodeBuf[16];
	unsigned char	keycodeHead,keycodeTail;
	unsigned char	statusBuf[8];
	unsigned char	statusHead,statusTail;
	unsigned char	cmdBuf[8];
	unsigned char	cmdHead,cmdTail;
	Boolean			keyboardPresent;
	char			pad;
	KeyMappingTable	*keyMapTable;
	unsigned long	keyboardID;
	unsigned long	keyboardFlags;
	long			samplePhase;			// not everything each VBL
	ControllerType	curType;				// routine for reading appropriate multiType
} ControlGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE ControlGlobals
#else
extern ControlGlobals controls;
#endif



#ifdef SIMULATOR
			// these keycodes are for mac keyboards only!
#define	leftArrow		0x7B
#define rightArrow		0x7C
#define upArrow			0x7E
#define downArrow		0x7D
#define returnKey		0x24
#define deleteKey		0x33
#define letterAKey		0x00
#define letterBKey		0x0B
#define letterCKey		0x08
#define	letterXKey		0x07
#define	letterYKey		0x10
#define	letterZKey		0x06
#define letterSKey		0x01
#define optionKey		0x3A

#define	kUp2			91			// numeric pad, pseudo joypad for player 2
#define kDown2			84
#define	kLeft2			86
#define kRight2			88
#define kA2				66		// *, -, +
#define kB2				77
#define kC2				72
#define	kX2				0x47	// clear,=,/
#define	kY2				0x51
#define	kZ2				0x4B
#define kStart2			76		// enter

#endif

#endif 			// __CONTROLS_PRIV_H__

