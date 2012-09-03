/*
	File:		RadioButtonsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 8/15/94	HEC		added choiceString field so I can dynamically change a radio
									button selection string.
		 <2>	 7/16/94	CMY		Changed the blink time
		 <1>	 7/14/94	CMY		first checked in

	To Do:
*/


typedef struct RadioButtonGlobals
{
	short dumi;
} RadioButtonGlobals;

#ifdef SIMULATOR
	#define MANAGERGLOBALTYPE RadioButtonGlobals
#else
	extern RadioButtonGlobals radioGlobs;
#endif


#define kRadioBlinkTime 20

typedef struct {
	segaCharRect	textGDeviceRect;
	short			titleFont;
	char			titleColor0;
	char			titleColor1;
	char			titleColor2;
	char			titleColor3;
	short			titleX;
	short			titleY;
	long			titleString;
	
	short			choiceFont;
	char			choiceColor0;
	char			choiceColor1;
	char			choiceColor2;
	char			choiceColor3;
	char			choiceSelectedColor0;
	char			choiceSelectedColor1;
	char			choiceSelectedColor2;
	char			choiceSelectedColor3;
	short			choiceX;
	short			choiceY;
	
	char *			choiceString;
	short			choicesCount;
	long			choices[1];
} RadioButtonResource;

typedef struct {
	RadioButtonResource	*radioLayout;
	long				patternBase;
	long				blinkTime;
	short				padWidth;
	char				choice;
	Boolean				isActive;
	Boolean				blinkState;
} RadioButtonReference;