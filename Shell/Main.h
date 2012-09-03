/*
	File:		Main.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 8/19/94	HEC		Added param to DoSegaDemo. Added DoSegaDemoStart.
		 <2>	  6/5/94	HEC		Added void DoSegaDemo( void )
		 <2>	 5/18/94	KON		Change type to MacDITLItem to get out of the way of the sega
									DITLItem.

	To Do:
*/

#include <QDOffscreen.h>

extern short gDrawGrid;

typedef void (*exitToShellProcPtr)(void);				//opcodes take a void and return a short

typedef struct WindowGlobals 
{
	Rect		gWorldRect;
	GWorldPtr	wGWorld;		//1-bit guy
	GWorldPtr	bufferGWorld;	//4-bit
	GWorldPtr	buffer1Bit;		//1-bit
	GWorldPtr	segaGWorld;		//4-bit pattern table
	GWorldPtr	expandGWorld;	//4-bit expanded from 1-bit
	GWorldPtr	theScreen;		//4-bit expanded from 1-bit
//
// screen elements
//
	Rect		patternRect;
	Rect		foreColorRect;
	Rect		backColorRect;
	short		boxHeight;
	short		boxWidth;

} WindowGlobals, *WindowGlobalsPtr;

typedef struct AreaDescriptor 
{
	RgnHandle	theRegion;
	short		patIndex;
	long		foreIndex;	// index of color in offscreen color table
	long		backIndex;

} AreaDescriptor, *AreaDescriptorPtr;

typedef struct UndoRecord 
{
	short		areaIndex;
	short		patIndex;
	long		foreIndex;	// index of color in offscreen color table
	long		backIndex;

} UndoRecord, *UndoRecordPtr;

void TileVertical( short windowCount, Rect *tileRect );
void TileWindows( void );

typedef struct {
	void			*dataPtr;			/* this is the definition of each item in the DITL */
	Rect			displayRect;
	char			itemType;
	char			itemLength;
	char			itemData[2];
} MacDITLItem, *MacDITLItemPtr;

#define fieldOffset(type, field) ((short) &((type *) 0)->field)

void DoSegaDemo( Boolean justPlayedGame );
void DoSegaDemoStart( void );
