/*
	File:		NewsUtils.c

	Contains:	xxx put contents here xxx

	Written by:	Jevans

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 6/29/94	KON		Major update.
		 <3>	 6/13/94	SAH		Managerized.
		 <2>	  6/9/94	HEC		Fixed include bug
		 <1>	  6/2/94	DJ		first checked in

	To Do:
*/


#include "News.h"
#include "NewsUtilsPriv.h"
#include "Errors.h"

//
// extract a string from the packed text.
//
char *_FindNextNewsString(char *text, long lineNum)
{
char	*s, *t;
long	line;

	ASSERT(text);
	if(!text)
		return(NULL);
	
	s = text;
	for(line = 0; line < lineNum; line++)
	{
		// find the end of the string.
		for(t = s; *t; )
			t++;
		
		s = t+1;
	}
	
	return(s);
}
