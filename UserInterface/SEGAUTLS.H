/*
	File:		SEGAUTLS.H

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 5/20/94	KON		Added 2-bit font allocation. Not debugged yet!
		 <2>	 5/18/94	KON		Add get and set of multiple fonts and store them in the
									database.

	To Do:
*/


#ifndef __SegaUtls__
#define __SegaUtls__

#ifndef __SegaText__
#include "SegaText.h"
#endif __SegaText__

SegaFontTablePtr AllocateFont( FontID fontID );
SegaFontTablePtr Allocate2BitFont( FontID fontID );
void DrawSegaKeyboard( short xPos, short yPos );

#endif __SegaUtls__