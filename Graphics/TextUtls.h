/*
	File:		TextUtls.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/28/94	CMY		Added kCustomLineHeight flag for SegaBoxText.
		<14>	 8/20/94	KON		Make sane values for just left, right, and center.
		<13>	 8/19/94	KON		Added kReturnPixelHeight flag.
		<12>	 8/17/94	JOE		Add kVerticalJustify
		<11>	 8/10/94	JBH		Added useCursor flag to InitTextEdit, to allow for te fields
									with no cursor (like chat second window).
		<10>	  8/8/94	KON		Add kDontDraw flag to SegaBoxText and now segaBoxText returns
									the number of characters drawn.
		 <9>	  8/5/94	JOE		Added Get/SetJizzleBehavior() routines
		 <8>	 7/21/94	JOE		Added flags param to SegaBoxText()
		 <7>	 7/18/94	CMY		Fancy jizzling boxes
		 <6>	 7/11/94	JBH		Added TextEditPreflightAppend, TextEditGetLineLength.
		 <5>	  7/7/94	CMY		SetTextEditLineHeight - to override the font's height.
		 <4>	  7/2/94	CMY		Added activate/deactivate textEdit.
		 <3>	 6/13/94	SAH		Managerized.
		 <2>	  6/2/94	CMY		Added multiline text edit routines.
		 <2>	 5/23/94	KON		Add support for justify right and center which required new
									routine MeasureSegaTextLength.
		 <1>	 5/23/94	KON		first checked in

	To Do:
*/

#ifndef __TextUtls__
#define __TextUtls__

#define kJustLeft	0
#define kJustCenter	1
#define kJustRight	2

// SegaBoxText flags
#define	kOnlyDrawLastLine	1
#define	kDontDraw			2
#define	kVerticalJustify	4
#define	kReturnPixelHeight	8		/* 0 returns number of characters that fit */
									/* 1 returns pixel height of text that will fit */
#define kCustomLineHeight	16		/* If set, the high byte contains line height to use */


short FindLineBreak( short pixelWidth, char * myCString ) = 
	CallDispatchedFunction( kFindLineBreak );

short SegaBoxText( Rect *myRect, char * myCString, short justType, short flags ) = 
	CallDispatchedFunction( kSegaBoxText );

void DrawSegaStringLength( short x, short y, char *myCString, short length ) = 
	CallDispatchedFunction( kDrawSegaStringLength );

short MeasureSegaTextLength( char *myCString, short length ) = 
	CallDispatchedFunction( kMeasureSegaTextLength );

/*
 * TextBox Jizzling Frames
 */
 
#define kJizzlationEnable	0
 
typedef void *TextBoxReference;

TextBoxReference DrawTextBox(segaCharRect *box, short color, short device) =
	CallDispatchedFunction( kDrawTextBox );

void SetJizzleBehavior(short selector, Boolean flag) =
	CallDispatchedFunction( kSetJizzleBehavior );

Boolean GetJizzleBehavior(short selector) =
	CallDispatchedFunction( kGetJizzleBehavior );
	
void StartTextBoxAnimation(TextBoxReference tbRef, short jizzleDelay) =
	CallDispatchedFunction( kStartTextBoxAnimation );
	
void StopTextBoxAnimation(TextBoxReference tbRef) =
	CallDispatchedFunction( kStopTextBoxAnimation );
	
void DisposeTextBoxReference(TextBoxReference tbRef) =
	CallDispatchedFunction( kDisposeTextBoxReference );

/*
 * Multiline text editing
 */

typedef enum {
	textEditOverflow = 100,
	textEditUnderflow,
	textEditWillBreak			// not really an error, but useful nonetheless for PreflightAppend
} TextEditErrors;

typedef short TextEditError;
typedef void *TextEditReference;

/*
 * Call this to start editing text in the specified rectangle. Buffer is where the C-string
 * of the actual text will be stored. You must have set up a TextGDevice already.
 */
TextEditReference InitTextEdit( Rect *destRect, short bufferSize, char *buffer, Boolean useCursor ) = 
	CallDispatchedFunction( kInitTextEdit );

TextEditError TextEditAppend( TextEditReference toEdit, short count, char *appendText ) = 
	CallDispatchedFunction( kTextEditAppend );

TextEditError TextEditDelete( TextEditReference toEdit, short count ) = 
	CallDispatchedFunction( kTextEditDelete );

TextEditError TextEditActivate( TextEditReference target ) =
	CallDispatchedFunction( kTextEditActivate );
	
TextEditError TextEditDeactivate( TextEditReference target ) =
	CallDispatchedFunction( kTextEditDeactivate );
	
TextEditError DisposeTextEdit(TextEditReference toDispose) = 
	CallDispatchedFunction( kDisposeTextEdit );

void SetTextEditLineHeight(TextEditReference target, short lineHeight) =
	CallDispatchedFunction( kSetTextEditLineHeight );
	
TextEditError TextEditPreflightAppend(TextEditReference target, char characterToAppend) =
	CallDispatchedFunction( kTextEditPreflightAppend );
	
short TextEditGetLineLength(TextEditReference target, short lineNumber) =
	CallDispatchedFunction( kTextEditGetLineLength );

#endif __TextUtls__
