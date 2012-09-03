/*
	File:		TextButt.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 7/30/94	CMY		Added code to control caching of the decompressor dictionary.
		 <6>	 7/27/94	CMY		Cache the master dictionary when animating.
		 <5>	 7/22/94	JOE		Animated icons can now have attached sound effects.
		 <4>	 7/21/94	CMY		Draw the text in a sprite so it don't get squarshed by the bean.
		 <3>	 7/20/94	CMY		Added animation for the selected bean.
		 <2>	 6/28/94	CMY		Updated to use palette manager.
		 <1>	 6/20/94	KON		first checked in
		 <3>	 5/30/94	KON		Fix up header includes.
		 <2>	 5/27/94	KON		Removed duplicate include of DitlMgr.h.
		 <5>	 5/23/94	SAH		Added text x and y offsets. Made TextButtonDesc fields bytes.
		 <4>	 5/21/94	SAH		Added animation data in place of bitmap ptr.
		 <3>	 5/20/94	SAH		Added the font and palette parameters. Also replaced foreColor
									and backColor in favour of color0..3.
		 <2>	 5/19/94	KON		Fix the TextButtonDesc that projector crapped on.
		 <1>	 5/18/94	KON		first checked in

	To Do:
*/

#ifndef __TextButtGraphics__
#define __TextButtGraphics__

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

#ifndef	__TextButt__
#include "TextButt.h"
#endif __TextButt__

#ifndef __Sprites__
#include "Sprites.h"
#endif __Sprites__

#ifndef __Decompress__
#include "Decompress.h"
#endif

typedef struct TextButtGraphicsPriv
{
	SpriteRef			textSprite;
	GraphicReference	buttReference;
	TimeProcRef			animatorProc;
	char				buttonID;
	char				currentFrame;
	short				x;
	short				y;
	short				frameCount;
	void				*toKeepDictCached;
	char				*frameSequence;
	FXSequence			*fxSequence;
} TextButtGraphicsPriv;



doDITLItemProc GetTextButtonGraphicsPtr( void );


#endif __TextButtGraphics__
