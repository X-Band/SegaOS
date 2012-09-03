/*
	File:		SegaScrnPriv.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	  7/3/94	JBH		Added backgroundColor to globals.
		 <4>	 6/29/94	CMY		Added 24 bytes of luminance tables to the screen globals. Used
									by GenerateGrayMap.
		 <3>	 6/28/94	CMY		Palette manager.
		 <2>	 6/21/94	HEC		Added FadeInScreen() which uses screenIsBlack and clutStorage
									globals.
		 <1>	 5/26/94	SAH		first checked in

	To Do:
*/



typedef
struct ScreenGlobals
{
	short			gCurDevice;
	SegaGDevice 	gDevices[3];
	short			clutSemaphore[3];
	short			screenIsBlack;
	short			clutStorage[4][16];
	short			backgroundColor;
	unsigned char	redLuminance[8];		/* used to calculate luminance of sega pixels */
	unsigned char	greenLuminance[8];
	unsigned char	blueLuminance[8];
} ScreenGlobals;



#ifdef SIMULATOR
#define MANAGERGLOBALTYPE ScreenGlobals

#else
extern ScreenGlobals screen;
#endif
