/*
	File:		SegaCluts.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	  8/1/94	CMY		Added enums for the blinky cluts
		 <2>	 7/18/94	CMY		Added enums for the default palette colors
		 <1>	 5/30/94	SAH		first checked in

	To Do:
*/



/*
*	Some standard clut id's until we come up with a better solution
*/

/* Colors in the default palette */
enum
{
	kTransparentColor,
	kWhiteColor,
	kGray5Color,
	kGray4Color,
	kGray3Color,
	kGray2Color,
	kGray1Color,
	kBlackColor,
	
	kGreenColor,
	kDarkGreenColor,
	
	kPurpleColor,
	kDarkPurpleColor,
	
	kYellowColor,
	kDarkYellowColor,
	
	kRedColor,
	kDarkRedColor
};


enum
{
	kZeusClut,
	kFirstBlinkyClut,
	kLastBlinkyClut = kFirstBlinkyClut+5,
	
	
	/* always keep this one last */
	kLastClut
};

#if kLastClut > 255
#error "Too many cluts for database"
#endif 
