/*
	File:		Keyboard.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 8/24/94	CMY		Allow the client to disable the cancel key.
		<12>	 8/16/94	CMY		Added SetKeyLayoutMeasureProc.
		<11>	 8/10/94	JBH		Added useCursor field to editableField, for chat's second field.
		<10>	 7/24/94	CMY		Keyboard entry manager can draw jizzlers for your text fields.
		 <9>	 7/13/94	CMY		GetKeyLayoutFieldCount and GetKeyLayoutFieldSize.
		 <8>	 7/11/94	JBH		Added chat keyboard.
		 <7>	 7/11/94	CMY		Added SelectKeyboardField.
		 <6>	 7/10/94	CMY		Added Keyboard layouts for the AKA screens
		 <5>	  7/8/94	CMY		Added StuffCurrentKeyboardField.
		 <4>	  7/7/94	CMY		Added a lineheight field to the EditableField struct.
		 <3>	  7/5/94	CMY		FinishKeyboard and RefreshKeyboard to support error dialogs
									during keyboard entry.
		 <2>	  7/1/94	CMY		Rearranged keyboard layout structure to map better to resource.
		 <1>	 6/30/94	CMY		first checked in

	To Do:
*/


#ifndef __Keyboard__
#define __Keyboard__


/* Keyboard Entry Layout DB IDs */
enum
{
	kMailEditKeyboardEntry,
	kAddressEditKeyboardEntry,
	kChooseHandleKeyboardEntry,
	kChooseTauntKeyboardEntry,
	kChooseInfoTextKeyboardEntry,
	kChoosePasswordKeyboardEntry,
	kChatKeyboardEntry
};


/* Return values from SendCommandToKeyboard() */
#define kKeyboardCommandOK	0
#define kKeyboardDone		1
#define kKeyboardCancel		-1

/* EditableField
 *
 * An EditableField describes the appearance and behavior of a text-edit field on the screen.
 */
typedef struct {	
	segaCharRect		textGDeviceRect;	/* The bounds of the field in character cells */
	Rect				textRect;			/* The bounds of the field in pixels within that rect */
	long				patternBase;		/* The pattern memory for the field's text gDevice */
	char				lineHeight;			/* The line spacing for the field */
	char				fontID;				/* The font and it's colors */
	short				fontColor0;
	short				fontColor1;
	short				fontColor2;
	short				fontColor3;
	short				bufferLength;		/* How big is the buffer for the text */
	short				useCursor;			/* 0 means don't use a te caret in this field */
	char				*buffer;			/* The actual buffer goes here */
} EditableField;


/* KeyboardEntryLayout
 *
 * A KeyboardEntryLayout describes a screen of 1 or more editable text boxes with a software
 * keyboard.
 */
typedef struct {
	short				keyboardX;			/* Left, Top of keyboard picture */
	short				keyboardY;
	char				activeField;		/* The currently active field */
	Boolean				disableCancel;		/* Set if the user can't cancel out */
	short				fieldCount;			/* How many edit fields are there? */
	EditableField		fields[1];			/* An array of the fields */
} KeyboardEntryLayout;

typedef void *KeyboardEntryRef;			/* Public keyboard entry session reference type */

/* A MeasureProc takes a string of text and returns true if the text will fit. */
typedef Boolean (*MeasureProc)(char *text);

// Create a KeyboardEntryLayout from a DB entry
KeyboardEntryLayout *SetupKeyboardEntryLayout( DBID keyLayoutID ) =
	CallDispatchedFunction( kSetupKeyboardEntryLayout );

// Dispose a KeyboardEntryLayout created from above call
void DisposeKeyboardEntryLayout( KeyboardEntryLayout *keyLayout ) =
	CallDispatchedFunction( kDisposeKeyboardEntryLayout );

short GetKeyLayoutFieldCount( DBID keyLayoutID ) =
	CallDispatchedFunction( kGetKeyLayoutFieldCount );
	
short GetKeyLayoutFieldSize( DBID keyLayoutID, short fieldNumber ) =
	CallDispatchedFunction( kGetKeyLayoutFieldSize );
	
/* Process a screen described by a KeyboardEntryLayout. This may be created using the
 * above calls, or may be user-allocated. If user-allocated, the buffers in the EditableFields
 * must be allocated and filled with the default text or the null string
 */
void DoKeyboardEntry(KeyboardEntryLayout *keyLayout) =
	CallDispatchedFunction( kDoKeyboardEntry );

/* === Screen-Driven Keyboards ===
 *
 * The following calls are designed to be used with screens. Call Init to get a reference
 * to a keyboard entry session. Send commands to the keyboard until it returns true. At this
 * point you can read the text from the buffers in the EditableFields. Then call FinishKeyboardEntry
 * to allow the keyboard entry manager to dispose of its state. Finally, dispose of your
 * KeyboardEntryLayout via DisposeKeyboardEntryLayout if it was generated from a DB entry,
 * or dispose it yourself if you allocated it directly.
 */
KeyboardEntryRef InitKeyboardEntry(KeyboardEntryLayout *keyLayout, short jizzleColor) =
	CallDispatchedFunction( kInitKeyboardEntry );
	
short SendCommandToKeyboard(KeyboardEntryRef ref, short command) =
	CallDispatchedFunction( kSendCommandToKeyboard );

void SelectKeyboardField(KeyboardEntryRef ref, short field ) =
	CallDispatchedFunction( kSelectKeyboardField );

void StuffCurrentKeyboardField(KeyboardEntryRef ref, char *string) =
	CallDispatchedFunction( kStuffCurrentKeyboardField );
	
void RefreshKeyboard(KeyboardEntryRef ref) =
	CallDispatchedFunction( kRefreshKeyboard );
		
void FinishKeyboardEntry(KeyboardEntryRef ref)
	= CallDispatchedFunction( kFinishKeyboardEntry );
	
void SetKeyboardEntryMeasureProc(KeyboardEntryRef ref, MeasureProc proc) =
	CallDispatchedFunction( kSetKeyboardEntryMeasureProc );

/* === Chat Keyboard ===
 *
 * The "chat" keyboard is meant for the live peer-to-peer communication between the players
 * and is somewhat specialized right now. It expects a keyboard layout with 2 fields. The
 * first field will get unmapped inputs (ie controller reads), and will return the mapped
 * character to send to the other player. A special value are sent to indicate completion
 * by the user selecting "done" or "cancel". Unlike the normal keyboards, the chat keyboards
 * have scrolling fields.
 */
 
short SendCommandToChatKeyboard(KeyboardEntryRef ref, short command, short commandIsMapped,
	short linesInField) =
	CallDispatchedFunction( kSendCommandToChatKeyboard );

#endif __Keyboard__