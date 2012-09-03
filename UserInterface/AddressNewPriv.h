/*
	File:		AddressNewPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  8/9/94	CMY		Added stick figure guys to the backgrounds.
		 <5>	  8/4/94	CMY		Added another field.
		 <4>	  8/3/94	CMY		Added a title.
		 <3>	 7/31/94	KON		Add textPatterns and handleField.
		 <2>	 7/24/94	CMY		Added some jizzlin' to this here skrene.
		 <1>	  7/2/94	CMY		first checked in

	To Do:
*/

#ifndef __Keyboard__
#include "Keyboard.h"
#endif


/* Fields for the New Address Keyboard entry screen */
enum
{
	kNewAddressAddressField
};

typedef struct {
	KeyboardEntryRef	keyRef;			// Keyboard entry session reference
	KeyboardEntryLayout	*keyLayout;		// Keyboard entry screen layout data
	TextBoxReference	jizzle;
	long				descriptionPatterns;
	long				titlePatterns;
	long				donePatterns;
	GraphicReference	handleField;	// The box to type into
	GraphicReference	playersRef;		// Them cute little stick figures
} NewAddressGlobals;

