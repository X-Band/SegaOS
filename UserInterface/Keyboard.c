/*
	File:		Keyboard.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):
	
		<46>	 8/29/94	JBH		Bumped buffer size for chat keyboard to 500 chars.
		<45>	 8/28/94	JBH		Ignore cancel in chat
		<44>	 8/28/94	JOE		Josh says:  Check "commandIsMapped" so we only read the hardware
									keyboard for the local user
		<43>	 8/27/94	JOE		Now the chat keyboard no longer has the "feature" that if the
									cursor is over the right hand side "special" keys, A, B, and C
									are all like A (just like the normal keyboard)
		<42>	 8/27/94	CMY		Add a return key to the chat keyboard.
		<41>	 8/27/94	CMY		Don't read hardware flags if there's no hardware keyboard.
		<40>	 8/26/94	CMY		Always draw the lower case keyboard before drawing the upper
									case keyboard due to graphic weirdness.
		<39>	 8/26/94	CMY		Only draw the upper-case keyboard if we need to.
		<38>	 8/26/94	JOE		add support for kReturnIsLF flag, map Escape -> Cancel (thanks
									Josh!)
		<37>	 8/25/94	JOE		hardware keyboard tweaks
		<36>	 8/25/94	JOE		Add support for the hardware keyboard
		<35>	 8/24/94	HEC		Made keyboard call GetQwertyOption for current user.
		<34>	 8/24/94	CMY		Don't leak a sprite. Don't leak pattern mems.
		<33>	 8/24/94	CMY		Allow client to disable the cancel key.
		<32>	 8/23/94	CMY		The keyboard now defaults to upper-case.
		<31>	 8/18/94	CMY		Default keyboard now has dimmed switch key, and we enable it
									with a sprite if there are multiple fields.
		<30>	 8/16/94	CMY		Added SetKeyLayoutMeasureProc.
		<29>	 8/13/94	CMY		Dim the switch key when there's only one field.
		<28>	 8/10/94	JBH		Fixed chat keyboard handling to be like normal keyboard.
		<27>	  8/5/94	CMY		Made the upper-case graphics smaller. yerga, stocklein, and
									britt.
		<26>	  8/3/94	HEC		Changed button A to type, B to space by popular demand:
									perlman, topping, froelich, horvich, holland, jevans, and cohn.
		<25>	  8/3/94	CMY		Remapped the A and C buttons. Redid Ted's shift key mod.
		<24>	  8/2/94	CMY		Changed keyboard caching strategy for better screen updates and
									greater free contiguous pattern space.
		<23>	 7/31/94	CMY		Don't dispose of nil graphicsReference.
		<22>	 7/31/94	CMY		Cache the damn keyboard! But don't leak graphicsReferences and
									flush the cache when returning control to the abuser.
		<21>	 7/31/94	KON		No longer cache the keyboard, and fix the text box jizzlers so
									they don't dispose nil jizzle procs.
		<20>	 7/31/94	CMY		Made caps and lower draw into same pattern mems to prevent VDP
									freak-outs.
		<19>	 7/30/94	CMY		Cache the decompressor dictionary to make switching caps faster.
		<18>	 7/28/94	HEC		kStart performs Shift.
		<17>	 7/26/94	CMY		Respect the QWERTY/ABCD flag.
		<16>	 7/24/94	CMY		Keyboards can draw jizzlers for your fields.
		<15>	 7/21/94	JOE		trigger Sound FX
		<14>	 7/13/94	ADS		Vectorize EVERYTHING
		<13>	 7/13/94	CMY		Made the CAPS key work. Added GetKeyLayoutFieldCount and
									GetKeyLayoutFieldSize.
		<12>	 7/12/94	CMY		Mr. Bigbux Consultant sez there's no Delete or Space key on
									keyboard. Make it so.
		<11>	 7/11/94	JBH		Added the dreaded "chat" keyboard.
		<10>	 7/11/94	CMY		Fix SelectKeyboardField so it doesn't generate phantom cursors.
		 <9>	 7/11/94	CMY		Added SelectKeyboardField so external clients can "Tab" between
									fields.
		 <8>	  7/8/94	CMY		Added StuffCurrentKeyboardField.
		 <7>	  7/8/94	CMY		Added large cursor for special keys. Added upper-case keyboard
									map.
		 <6>	  7/7/94	CMY		Added lineHeight field.
		 <5>	  7/5/94	CMY		FinishKeyboard and RefreshKeyboard to support error dialogs
									during keyboard entry.
		 <4>	  7/4/94	SAH		Include Cursor.h
		 <3>	  7/2/94	CMY		Fix memory leaks and support insertion cursors.
		 <2>	  7/1/94	CMY		Support for resource-driven and screen-driven keyboards.
		 <1>	 6/30/94	CMY		first checked in

	To Do:
*/


#include "SegaOS.h"
#include "SegaScrn.h"
#include "OSManagers.h"
#include "Errors.h"
#include "Events.h"
#include "Heaps.h"
#include "Pattern.h"
#include "GraphicsDB.h"
#include "DBTypes.h"
#include "SegaVDP.h"
#include "Time.h"
#include "SegaText.h"
#include "Decompress.h"
#include "Sprites.h"
#include "TextUtls.h"
#include "DialogMgr.h"
#include "SegaIn.h"
#include "Utils.h"
#include "Cursor.h"
#include "Keyboard.h"
#include "KeyboardPriv.h"
#include "StringDB.h"
#include "SegaSound.h"
#include "BoxSer.h"
#include "DBConstants.h"
#include "UsrConfg.h"
#include "controls.h"


/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
KeyboardEntryGlobals keyEntry;
#endif

/* Our dispatched functions */
KeyboardEntryLayout *_SetupKeyboardEntryLayout( DBID keyLayoutID );
void				_DisposeKeyboardEntryLayout( KeyboardEntryLayout *keyLayout );
short				_DoKeyboardEntry( KeyboardEntryLayout *keyLayout );
KeyboardEntryRef	_InitKeyboardEntry( KeyboardEntryLayout *keyLayout, short jizzleColor );
short				_SendCommandToKeyboard( KeyboardEntryRef ref, short command );
short 				_SendCommandToChatKeyboard(KeyboardEntryRef ref, short command, short commandIsMapped, 
							short linesInField);
void				_FinishKeyboardEntry( KeyboardEntryRef ref );
void				_RefreshKeyboard( KeyboardEntryRef ref );	
void				_StuffCurrentKeyboardField( KeyboardEntryRef ref, char *string );
void				_SelectKeyboardField( KeyboardEntryRef ref, short field );
short				_GetKeyLayoutFieldCount( DBID keyLayoutID );
short				_GetKeyLayoutFieldSize( DBID keyLayoutID, short fieldNumber );
void				_SetKeyboardEntryMeasureProc( KeyboardEntryRef ref, MeasureProc proc );

// internal only dispatched functions

void	_SetFocusField(KeyboardEntryLayout *keyLayout, short fieldIndex);
void	_DrawKeyboard(KeyEntryState *state);
short	_ComputeCursorLineNumber( TextEditReference ref, char *buffer );
void	_CacheKeyboardGraphics( KeyEntryState *state );
void	_ReleaseKeyboardGraphicsCache( KeyEntryState *state );



long
_KeyboardEntryControl ( short command, long data )
{
long					offset;
short					error;
KeyboardEntryGlobals	*globals;
short					ii;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(keyEntry,offset);
			
			error = AllocateGlobalSpace ( kKeyboardEntryManager, offset, sizeof(KeyboardEntryGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create keyboard entry globals" );
			if ( error != noErr )
			{
				return error;
			}
				
			/* install our selectors */
			SetDispatchedFunction ( kSetupKeyboardEntryLayout,	kKeyboardEntryManager,	_SetupKeyboardEntryLayout );
			SetDispatchedFunction ( kDisposeKeyboardEntryLayout,kKeyboardEntryManager,	_DisposeKeyboardEntryLayout );
			SetDispatchedFunction ( kDoKeyboardEntry,			kKeyboardEntryManager,	_DoKeyboardEntry );
			SetDispatchedFunction ( kInitKeyboardEntry,			kKeyboardEntryManager,	_InitKeyboardEntry );
			SetDispatchedFunction ( kSendCommandToKeyboard,		kKeyboardEntryManager,	_SendCommandToKeyboard );
			SetDispatchedFunction ( kFinishKeyboardEntry,		kKeyboardEntryManager,	_FinishKeyboardEntry );
			SetDispatchedFunction ( kRefreshKeyboard,			kKeyboardEntryManager,	_RefreshKeyboard );
			SetDispatchedFunction ( kStuffCurrentKeyboardField,	kKeyboardEntryManager,	_StuffCurrentKeyboardField );
			SetDispatchedFunction ( kSelectKeyboardField,		kKeyboardEntryManager,	_SelectKeyboardField );
			SetDispatchedFunction ( kSendCommandToChatKeyboard,	kKeyboardEntryManager,	_SendCommandToChatKeyboard );
			SetDispatchedFunction ( kGetKeyLayoutFieldCount,	kKeyboardEntryManager,	_GetKeyLayoutFieldCount );
			SetDispatchedFunction ( kGetKeyLayoutFieldSize,		kKeyboardEntryManager,	_GetKeyLayoutFieldSize );
			SetDispatchedFunction ( kSetKeyboardEntryMeasureProc,kKeyboardEntryManager,	_SetKeyboardEntryMeasureProc );

			SetDispatchedFunction ( kSetFocusField,				kKeyboardEntryManager,	_SetFocusField );
			SetDispatchedFunction ( kDrawKeyboard,				kKeyboardEntryManager,	_DrawKeyboard );
			SetDispatchedFunction ( kComputeCursorLineNumber,	kKeyboardEntryManager,	_ComputeCursorLineNumber );
			SetDispatchedFunction ( kCacheKeyboardGraphics,		kKeyboardEntryManager,	_CacheKeyboardGraphics );
			SetDispatchedFunction ( kReleaseKeyboardGraphicsCache,kKeyboardEntryManager,_ReleaseKeyboardGraphicsCache );
			break;
		
		case kSoftInialize:
			/* init our globals here */
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}


short _GetKeyLayoutFieldCount( DBID keyLayoutID )
{
	KeyboardEntryLayout *dbLayout = (KeyboardEntryLayout *) DBGetItem(kKeyboardEntryLayout, keyLayoutID);
	
	return dbLayout->fieldCount;
}


short _GetKeyLayoutFieldSize( DBID keyLayoutID, short fieldNumber )
{
	KeyboardEntryLayout *dbLayout = (KeyboardEntryLayout *) DBGetItem(kKeyboardEntryLayout, keyLayoutID);

	if (fieldNumber < dbLayout->fieldCount)
		return dbLayout->fields[fieldNumber].bufferLength;
	else
		return -1;
}

KeyboardEntryLayout *_SetupKeyboardEntryLayout(DBID keyLayoutID)
{
	KeyboardEntryLayout *keyLayout, *dbLayout;
	short extraFields;
	short index = 0;
	EditableField *field;
	
	// Get the layout from DB. If not there, just bail like a happy-fun routine should
	dbLayout = (KeyboardEntryLayout *) DBGetItem(kKeyboardEntryLayout, keyLayoutID);
	if (dbLayout == nil)
		return nil;
	
	// Allocate space for the in-mems layout
	extraFields = dbLayout->fieldCount - 1;
	if (extraFields < 0)
		extraFields = 0;
	keyLayout = NewMemory( kTemp, sizeof(KeyboardEntryLayout) + extraFields*sizeof(EditableField));
	if (keyLayout == nil)
	{
		DisposeMemory(dbLayout);
		return nil;
	}
	
	// Copy the DB fields to the in-mems version
	ByteCopy((Ptr)keyLayout, (Ptr)dbLayout, GetMemorySize(keyLayout));
	
	// Allocate the field buffers and null them out
	field = keyLayout->fields;
	while (index < keyLayout->fieldCount)
	{
		field->buffer = NewMemory( kTemp, field->bufferLength );
		field->buffer[0] = '\0';
		++index;
		++field;
	}
	
	return keyLayout;
}

void _DisposeKeyboardEntryLayout( KeyboardEntryLayout *keyLayout )
{
	short index = 0;
	EditableField *field = keyLayout->fields;
	
	while (index < keyLayout->fieldCount)
	{
		DisposeMemory(field->buffer);
		++index;
		++field;
	}
			
	DisposeMemory((Ptr)keyLayout);
}

void _CacheKeyboardGraphics( KeyEntryState *state )
{
	Decompressor dec = CreateDecompressor( GetDBGraphics(state->graphicID), 0, 0 );
	
	state->decompressorCache = GetDecompressorCache( dec );
	DisposeDecompressor( dec );
}

void _ReleaseKeyboardGraphicsCache( KeyEntryState *state )
{
	Decompressor dec;
	GraphicReference *graphic;
	
	/* 
	 * If we're using the upper-case keyboard, switch to lower and toss the upper
	 * case keyboard, because we allocate the upper keyboard high in pat mems
	 */
	if (state->shiftState)
	{
		state->shiftState = false;
		DrawKeyboard(state);
	}
	
	if (state->upperGraphic)
	{
		DisposeGraphicReference( state->upperGraphic );
		state->upperGraphic = nil;
	}

	/* Get rid of the decompressor cache if it's around. We must do this after switching
	 * keyboards, because DrawKeyboard() will re-cache it!
	 */
	if ( state->decompressorCache )
	{
		dec = CreateDecompressor( GetDBGraphics(state->graphicID), 0, 0 );
		
		ReleaseDecompressorCache( dec, state->decompressorCache );
		DisposeDecompressor( dec );
		state->decompressorCache = nil;
	}
}

void _SetFocusField(KeyboardEntryLayout *keyLayout, short fieldIndex)
{
	EditableField *field = &keyLayout->fields[fieldIndex];
	
	if (fieldIndex < 0 || fieldIndex >= keyLayout->fieldCount)
	{
		WARNING_MESG("SetFocusField: invalid field index");
		return;
	}
	
	/* Active the text device for this field and set the font and colors */
	SetupTextGDevice( &field->textGDeviceRect, field->patternBase );
	SetCurFont( field->fontID );
	SetFontColors( field->fontColor0, field->fontColor1, field->fontColor2, field->fontColor3 );
	
	/* Update the keyLayout state to point to this field */
	keyLayout->activeField = fieldIndex;
}

void _Dr