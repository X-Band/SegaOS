/*
	File:		TimerProgress.c

	Contains:	A countdown timer progress proc

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/21/94	JBH		Add option to display a string, instead of countdown number
									(used by news when we hit zero)
		 <8>	 8/20/94	HEC		Hack: Subtract one from MeasureText result so timer text isn't
									clipped on right edge.
		 <7>	  8/5/94	JBH		Use right-justified, padded text so we don't leave garbage on
									the screen.
		 <6>	  8/4/94	JBH		Added a flag to allow timer progress to not draw the timer on
									spawn, so that news can set it to the appropriate value first.
		 <5>	 7/28/94	KON		Make the progress flags a bit field rather than an enum.
		 <4>	 7/28/94	JBH		Added option to not use colon.
		 <3>	 7/19/94	ADS		Vector Mania
		 <2>	 7/10/94	ADS		Cleaned up "existing GWorld" case
		 <1>	  7/9/94	JBH		first checked in

	To Do:
*/


/* if we're building the ROM database, do a real sega build */
#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "Errors.h"
#include "Heaps.h"
#include "MungeCode.h"
#include "Pattern.h"
#include "Progress.h"
#include "ProgressPriv.h"
#include "SegaText.h"
#include "StringDB.h"
#include "vdp.h"

/*
*
* This is meant to be a simple "progress proc" used to tell the user how far in an operation
* they have waited, and how much is left (ie when loading news). It will do a countdown,
* assuming that the totalTime is the total # of seconds in the operation, and the current time
* is how many seconds have passed so far (ie 0 is timer start, totalTime-1 is 1 second left)
*
* moreData is the pattern memory to draw the text into. If non-zero, we will assume that
* the textGDevice is set up and the LinearizeScreenArea has been called. If 0, we will
* set it up ourselves, and throw it away during disposal. Note that no matter what,
* THE CALLER MUST CLEAR THE NAME TABLE after the LinearizeScreenArea is cleared, before
* a new sprite is allocated, otherwise the user will see "features" (ie poo) when pattern
* memory is allocated to someone else
*
*/

typedef (*TimerUtilProc)	(char *buffer, short thisTime, short totalTime, Boolean useColon);

typedef struct TimerProgressState {
	short deltaX;			/* 0..7, how far over the text gets drawn */
	short deltaY;			/* 0..7, how far down the text gets drawn */
	short totalTime;
	short lastTime;
	long  patternMemory;
	void *ourTextState;			/* the text state we want to use */
	void *userTextState;		/* memory used to preserve text state across calls to us */
	short	patternMemoryOwnedByTimer;
	short	padWidth;
	Boolean	useColon;
	
	TimerUtilProc	fillTimeBuffer;
	
} TimerProgressState;

static	long	TimerProgressProc ( short command, ProgressRec *progData, long moreData, long * ref );

//
// local, psuedo-vectorized functions
//
static void		_FillTimeBuffer(char *buffer, short thisTime, short totalTime, Boolean useColon );

#define FillTimeBuffer(buffer, thisTime, totalTime, useColon)		((state->fillTimeBuffer) (buffer, thisTime, totalTime, useColon))



static
long TimerProgressProc ( short command, ProgressRec *progData, long moreData, long * ref )
{
long				returnVal;
TimerProgressState	*state = (TimerProgressState *) *ref;
char				buffer[6];
char				zeroBuffer[6];
short x,y;

	returnVal = kProgressOK;
	
	switch ( command )
		{
		case kInitProgressProcCmd:
			state = NewMemory ( kTemp, sizeof(TimerProgressState) );
			*ref = (long) state;
			
			if ( state )
			{
				state->fillTimeBuffer = (TimerUtilProc) _FillTimeBuffer;
				
				state->totalTime = progData->totalTime;
				state->lastTime = 0;
				if (moreData & kDrawWithAColonProgress)
					state->useColon = true;
				else
					state->useColon = false;
				
				FillTimeBuffer(buffer, 0, state->totalTime, state->useColon);
				
				/* nasty code to set up a string of 5 0's */
				*(long *)zeroBuffer = '0000';
				zeroBuffer[4] = '0';
				zeroBuffer[5] = 0;
				
				if( moreData & kNoTextGWorldProgress ) 
				{
					/* compute the SegaCharRect to hold the characters. */

					short pixelLeft;	/* leftmost pixel, rounded down to nearest 8 */
					short pixelTop;
					short pixelRight;	/* rounded up to nearest 8 */
					short pixelBottom;
					segaCharRect charRect;
					FontID	curFont;
					
					curFont = GetCurFont();	/* swap out the font to get metrics */					
					SetCurFont( kMainFont );
					
					pixelLeft = progData->boundsRect.left & (~7);	/* round down to multiple of 8 */
					pixelTop = progData->boundsRect.top & (~7);
					pixelBottom = progData->boundsRect.top + GetCurFontHeight();
					
					pixelBottom += 7;
					pixelBottom &= ~7;

					pixelRight = progData->boundsRect.left + GetCharWidth('0')*5 + 5;
					pixelRight += 7;
					pixelRight &= ~7;
					
					state->deltaX = progData->boundsRect.left - pixelLeft;
					state->deltaY = progData->boundsRect.top - pixelTop;
					
					charRect.left = pixelLeft >> 3;		/* convert pixels to char rects by */
					charRect.top = pixelTop >> 3;		/* dividing by 8 */
					charRect.right = pixelRight >> 3;
					charRect.bottom = pixelBottom >> 3;
					
					state->padWidth = MeasureSegaText( zeroBuffer );
					
					SetCurFont( curFont );
					
					state->patternMemory = LinearizeScreenArea(&charRect, 0);
					
					ASSERT_MESG(state->patternMemory, "couldn't allocate pattern memory for text");
					if (state->patternMemory) 
					{
						/* setup the TextGDevice */

						state->patternMemoryOwnedByTimer = true;
						state->userTextState = GetTextStateReference();
						
						ASSERT_MESG( state->userTextState, "couldn't get text state buffer" );

						if ( state->userTextState ) 
						{
							state->ourTextState = GetTextStateReference();
							
							ASSERT_MESG( state->ourTextState, "couldn't get text state buffer" );
	
							if ( state->ourTextState ) 
							{
							
								/* setup default drawing characteristics */
								SetupTextGDevice(&charRect, state->patternMemory);
								
								SetCurFont( kMainFont );
								SetFontColors( 0, 13, 12, 0 );
								
								SaveTextState( state->ourTextState );

								if( ( moreData & kDontDrawOnSpawn ) == 0 )
								{
									/* draw the sucker */
									x = state->deltaX;
									y = state->deltaY;
									
									DrawPaddedClippedSegaText( x, y, state->padWidth, kJustifyRight, buffer );
								}
								
								/* restore to state before call */
								RestoreTextState( state->userTextState );
							} 
							else 
							{
								/* couldn't allocate our text state buffer, so bail */
								returnVal = kProgressInitError;
								DisposeTextStateReference( state->userTextState );
								FreePatternBlock( state->patternMemory );
								DisposeMemory(state);
								*ref = 0;
							}

						} 
						else 
						{
							/* couldn't allocate text state buffer, so bail */
							returnVal = kProgressInitError;
							FreePatternBlock( state->patternMemory );
							DisposeMemory(state);
							*ref = 0;
						}

					} 
					else 
					{
						/* couldn't allocate pattern memory, so 0 and bail */

						returnVal = kProgressInitError;
						DisposeMemory(state);
						*ref = 0;
					}
					
				} 
				else 
				{		/* use the existing text GWorld */
				
					state->userTextState = GetTextStateReference();

					ASSERT_MESG( state->userTextState, "couldn't get text state buffer" );

					if ( state->userTextState ) 
					{
						state->ourTextState = GetTextStateReference();
						ASSERT_MESG( state->ourTextState, "couldn't get text state buffer" );
						
						if (state->ourTextState) 
						{
							state->patternMemory = moreData;
							state->patternMemoryOwnedByTimer = false;
							x = state->deltaX = progData->boundsRect.left;
							y = state->deltaY = progData->boundsRect.top;
							state->padWidth = MeasureSegaText( zeroBuffer ) - 1;
							if( ( moreData & kDontDrawOnSpawn ) == 0 )
								DrawPaddedClippedSegaText( x, y, state->padWidth, kJustifyRight, buffer );
							VDPIdle();
						} 
						else 
						{
							returnVal = kProgressInitError;
							DisposeTextStateReference( state->userTextState );
							DisposeMemory( state );
							*ref = 0;
						}
					} 
					else 
					{
							returnVal = kProgressInitError;
							DisposeMemory( state );
							*ref = 0;
					}
				}
			}
			else
			{
				returnVal = kProgressInitError;
			}
			break;
		
		case kDisposeProgressProcCmd:
			if ( state )
				{
				
				DisposeTextStateReference( state->userTextState );
				DisposeTextStateReference( state->ourTextState );
				
				/* if we own the pattern memory for the text, chuck it. */
				if (state->patternMemoryOwnedByTimer)
					{
					FreePatternBlock ( state->patternMemory );
					}
				DisposeMemory ( state );
				}
			
			*ref = 0L;
			break;
		
		case kSetProgressPositionCmd:
			if ( state )
				{
				if (state->lastTime != moreData)
					{
					SaveTextState( state->userTextState );
					RestoreTextState( state->ourTextState );
					state->lastTime = moreData;
					
					if ( ( moreData & kTimerProgressUseString ) == kTimerProgressUseString )
					{
						char *str;
					
						str = GetSegaString( moreData & ~kTimerProgressUseString );
						DrawPaddedClippedSegaText( state->deltaX, state->deltaY, state->padWidth, kJustifyRight, str );
					}
					else
					{
						FillTimeBuffer(buffer, moreData, state->totalTime, state->useColon);
						DrawPaddedClippedSegaText( state->deltaX, state->deltaY, state->padWidth, kJustifyRight, buffer );
					}
					
					RestoreTextState( state->userTextState );
					}
				}
			break;
		
		case kIdleProgressCmd:		/* a big no-op */
			break;
		
		}
	
	return returnVal;
}

/* utility function - takes current time, total time, and makes a text string suitable for */
/* drawing. Note that buffer should point to at least 6 characters of memory. */
static void _FillTimeBuffer(char *buffer, short thisTime, short totalTime, Boolean useColon)
{
	short time;
	short minutes;
	short seconds;

	time = totalTime - thisTime;
	if (useColon) {
		minutes = time / 60;
		seconds = time % 60;
		
		ASSERT_MESG(minutes < 100, "too many minutes in countdown timer!");
		
		/* handle first digit of minutes */
		if (minutes > 9) {
			*buffer++ = (minutes / 10) + '0';
			*buffer++ = (minutes % 10) + '0';
		} else {
			*buffer++ = ' ';
			/* handle 2nd digit of minutes, which could be a space if minutes is 0 */
			minutes %= 10;
			if (minutes) {
				*buffer++ = minutes + '0';
			} else {
				*buffer++ = ' ';
			}
		}
		
		
		*buffer++ = ':';
		
		*buffer++ = (seconds / 10) + '0';
		*buffer++ = (seconds % 10) + '0';
		*buffer = 0;
	} else {
		short iii;
		
		if ( time < 10 ) {
			iii=0;
			buffer[1] = 0;
		} else if ( time < 100 ) {
			iii = 1;
			buffer[2] = 0;
		} else if ( time < 1000 ) {
			iii = 2;
			buffer[3] = 0;
		} else if ( time < 10000) {
			iii = 3;
			buffer[4] = 0;
		} else {
			iii = 4;
			buffer[5] = 0;
		}
		
		do {
			buffer[iii] = (time % 10) + '0';
			time /= 10;
		} while (time && --iii>=0);
	}
}


Ptr
GetTimerProgressProc ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) TimerProgressProc + 2));

	size = (*(long *) ((Ptr) GetTimerProgressProc + 2)) - srcFunction;
	data = NewMemory ( kTemp,  size );
	ASSERT_MESG( data, "Cannot allocate memory for timer progress proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) TimerProgressProc;
#else
		CheckCodeForJTRefs((void *)TimerProgressProc, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
