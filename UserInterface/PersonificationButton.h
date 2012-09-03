/*
	File:		PersonificationButton.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 8/12/94	JOE		new icon stuff
		 <6>	  8/7/94	CMY		Change text spacing
		 <5>	  8/3/94	JOE		update for new pers mgr
		 <4>	  8/3/94	CMY		Added text state.
		 <3>	 7/21/94	CMY		The text is now a sprite
		 <2>	 7/20/94	JOE		updated icon width for new thin icons
		 <1>	  7/9/94	CMY		first checked in

	To Do:
*/


#ifndef __PersonificationButton__
#define __PersonificationButton__

#ifndef __SegaBM__
#include "SegaBM.h"
#endif __SegaBM__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__

#ifndef	__Animation__
#include "Animation.h"
#endif __Animation__

#ifndef	__Animators__
#include "Animators.h"
#endif __Animators__

#ifndef __Sprites__
#include "Sprites.h"
#endif __Sprites__


#define kPersonificationIconWidth 35
#define kPersonificationIconHeight 57

typedef struct PersonButtonPriv
{
	SpriteRef		textSprite;
	IconReference	iconReference;
	void			*textSpriteState;
} PersonButtonPriv;

typedef struct PersonButtonDesc
{
	unsigned char	userID;
	char			*handle;
} PersonButtonDesc;


doDITLItemProc GetPersonButtonPtr( void );


#endif __TextButt__
