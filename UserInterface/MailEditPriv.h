/*
	File:		MailEditPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	  8/3/94	CMY		Added pattern mems references so we don't leak.
		 <4>	  8/3/94	CMY		Added a jizzle rect?
		 <3>	 7/11/94	CMY		Added a "To" field to the mail edit globals to support the Reply
									button.
		 <2>	  7/8/94	CMY		Added addressee and userHasTyped to MailEditGlobals to support
									auto-entry of address book names.
		 <1>	  7/2/94	CMY		first checked in

	To Do:
*/


#ifndef __Keyboard__
#include "Keyboard.h"
#endif


/* Fields for the MailEdit Keyboard entry screen */
enum
{
	kMailEditToField,
	kMailEditTitleField,
	kMailEditMessageField
};

typedef struct {
	KeyboardEntryRef	keyRef;			// Keyboard entry session reference
	KeyboardEntryLayout	*keyLayout;		// Keyboard entry screen layout data
	userIdentification	*to;			// If this is a Reply, who is the letter to?
	TextBoxReference	jizzle;			// Jizzle rect
	GraphicReference	fieldsGraphic;	// Pictures of the fields
	long				toPatterns;		// Text GDevice patterns for "To:"
	long				titlePatterns;
	char				addressee;		// Current addressee from address book
	char				userHasTyped;	// Has user entered chars yet?
} MailEditGlobals;

