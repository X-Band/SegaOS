/*
	File:		DialogMgrPriv.h

	Contains:	Modal dialogs internals

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<28>	 8/29/94	CMY		Bump size of ParamText buffer to 300 bytes to allow for larger
									server-downloaded dialogs.
		<27>	 8/22/94	ADS		tweak timing
		<26>	 8/22/94	JBH		Added bgColor to DoPassword
		<25>	 8/21/94	HEC		Added param to DoAnyResponse.
		<24>	 8/19/94	ADS		Add sparkly sparks
		<23>	 8/10/94	KON		Remove stale dismissal proc.
		<22>	  8/9/94	JOE		Data Driven dismissals now have associated sound FX IDs
		<21>	  8/8/94	JBH		Vectorized shiner code.
		<20>	 7/17/94	JBH		Fixed prototype for DoPlayAgainDialog.
		<19>	 7/17/94	JBH		Added play again dialog.
		<18>	 7/14/94	ADS		Remove placeholder
		<17>	 7/13/94	ADS		Vectorize EVERYTHING
		<16>	 7/10/94	ADS		Added progress timers
		<15>	  7/8/94	ADS		Added semi-universal dialogs
		<14>	  7/6/94	ADS		changed dialog params
		<13>	  7/2/94	JBH		Allow dialog dismissals to be run backwards (for bring-ins).
		<12>	  7/2/94	JBH		Added data-driven dialog dismissals.
		<11>	  7/1/94	KON		Removed dialog time out global and rolled it into the dialog
									data structure directly.
		<10>	  7/1/94	KON		Added kTimeBeforeAutoDismiss.
		 <9>	 6/21/94	KON		Add dialogTimeOut global.
		 <8>	 6/20/94	KON		Moved CPDialog to public interface so the DeferredDialogMgr can
									create dialogs directly.
		 <7>	 6/20/94	CMY		Added textColors to the dialog template DB item.
		 <6>	 6/15/94	CMY		Support for ParamText
		 <5>	 6/15/94	CMY		Add default items to dialog.
		 <4>	 6/15/94	CMY		SUpport for Yes/No dialogs
		 <3>	 6/15/94	KON		Make template and variation chars (to match DBID size) and avoid
									packing issues.
		 <2>	 6/14/94	CMY		Make dialogs database driven.
		 <1>	 6/14/94	CMY		first checked in

	To Do:
*/


#ifndef __DialogMgrPriv__
#define __DialogMgrPriv__


#ifndef __Sprites
	#include "Sprites.h"
#endif
#ifndef __Progress__
	#include "Progress.h"
#endif

#ifndef __GT__
	#include "GT.h"
#endif

#define kParamTextBufferSize 300
#define kTimeBeforeAutoDismiss 180

typedef struct DialogGlobals
{
	short			bgColor;
	short			frameColors[5];
	char			*paramText[4];
	char			paramBuffer[kParamTextBufferSize];
} DialogGlobals;

#ifdef SIMULATOR
	#define MANAGERGLOBALTYPE DialogGlobals
#else
	extern DialogGlobals dialog;
#endif

enum {
	kInformDialog = 0,
	kYesNoDialog = 1,
	kPasswordEntryDialog = 2,
	kAnyStringsDialog = 3,
	kPlayAgainDialogType = 4
};

/*
 * This defines the overall appearance of a dialog: size, location, and color. The actual text is
 * kept in a separate database type so we don't have to repeat this stuff over and over again.
 */
typedef struct {
	short		width;			/* Width in char cells, must be a multiple of 4 */
	short		height;			/* Height in char cells, must be a multiple of 4 */
	short		x;				/* Left edge of dialog in screen coordinates */
	short		y;				/* Top edge of dialog in screen coordinates */
	Rect		textBox;		/* The box to wrap text into, in dialog local coordinates */
	short		bgColor;		/* The background color of the dialog */
	short		frameColors[5];	/* The 5 frame colors... */
	short		textColor1;
	short		textColor2;
	short		frameBitmap;	/* Dialog frame bitmap picture or 0 if no picture */
	
	short		progressID;		/* for progress mgr.  0=no progress for this dialog */
	long		progressData;	/* more data, specific to each progress type	*/
	Rect		progressRect;	/* rect for progress area	*/
} CPDialogTemplate;


/*
 * State for our flying dialog timeProc
 */
typedef struct {
	SpriteRef	dialogSprite;
	short		timeStamp;
	short		velocityX;
	short		velocityY;
	short		left;
	short		top;
} DismissalState;

/*
 * This defines the data-driven dialog dismissal. Essentially, the data is acceleration
 * vectors for each cell in the dialog.
 */
 typedef struct DataDismissalRecord {
 	DBID  forwardSoundFX;
 	DBID  backwardSoundFX;
	short width;				/* width in 32x32 pixel cells */
	short height;
	short frames;				/* total frames in animation */
	short data[1];
} DataDismissalRecord;

/*
 * State for the data-driven dialog dismissal timeProc
 */
typedef struct DataDismissalState {
	SpriteRef sprite;
	short frameNumber;
	short totalFrames;
	short direction;
	short cells;
	long timeStamp;
	short *positions;
} DataDismissalState;


/*
 *	Controller for time manager driven "progress" displays
 */
typedef struct ProgressTimer	{
	TimeProcRef		timer;
	ProgressRef		progress;
	ProgressRec		progressData;
	long			startTime;
	Boolean			timedOut;
	short			fc0, fc1, fc2, fc3;
	FontID			font;
} ProgressTimer;


//
//	Data used by the shiny things around dialog boxes
//

#define kShinerBlankTime	(3*60)		// delay between shines
#define kShinerOffscreen	(100)		// non-displayed location
#define kSparkLife			(140)		// ticks of single spark run

enum {
	kBetweenEffects = 0,
	kRunShiner,
	kRunSparkle
};

typedef struct Shiner {
	SpriteRef	shinerSprite;
	short		x;
	short		y;
	short		initialX;
	short		initialY;
	short		deltaX;
	short		deltaY;
} Shiner;

typedef struct Shiners {
	Shiner			theShiners[4];
	short			frame;
	short			transition1;
	short			transition2;
	short			totalFrames;
	TimeProcRef		shinerTimeProc;
	SparkRef		spark;
	SpriteRef		sparkSprite;
	short			sparkX[3];
	short			sparkY[3];
	short			mode;
} Shiners;

// internal functions, vectorized for patching


char *	CopyString(const char *src, char *dst) =
	CallDispatchedFunction( kCopyString );

short 	DoAnyResponse( Rect *textBox, char defaultItem, long minTime, long maxTime, CPDialog *dbDialog, CPDialogTemplate *template ) =
	CallDispatchedFunction( kDoAnyResponse );

short 	DoDataDrivenDismissal(SpriteRef sprite, short width, short height, short left, short top, short forward) =
	CallDispatchedFunction( kDoDataDrivenDismissal );

short	DoPassword( Rect *textBox, char defaultItem, long minTime, long maxTime, short bgColor, void* param ) =
	CallDispatchedFunction( kDoPassword );

short	DoPlayAgainDialog( Rect *textBox, char defaultItem, long minTime, long maxTime, CPDialog *dbDialog, void* param ) =
	CallDispatchedFunction( kDoPlayAgainDialog );

void	DrawDialogFrame(segaCharRect *frameBounds) =
	CallDispatchedFunction( kDrawDialogFrame );

void	FillTextRectangle(segaCharRect *bounds, short bgColor) =
	CallDispatchedFunction( kFillTextRectangle );

void	HorizontalLine(short x1, short x2, short y, short color) =
	CallDispatchedFunction( kHorizontalLine );

void	KillProgressTimer(ProgressTimer* theTimer) =
	CallDispatchedFunction( kKillProgressTimer );

void	ReplaceParameters(char *message, char *buffer) =
	CallDispatchedFunction( kReplaceParameters );

void	SetupProgressTimer(ProgressTimer* theTimer, CPDialogTemplate* dialog, long endTime) =
	CallDispatchedFunction( kSetupProgressTimer );

void	VerticalLine(short y1, short y2, short x, long colorPattern) =
	CallDispatchedFunction( kVerticalLine );

Shiners	*CreateShiners( segaCharRect *dialogCharRect, CPDialogTemplate *template ) =
	CallDispatchedFunction (kCreateShiners );
	
void	DisposeShiners( Shiners *shiners ) =
	CallDispatchedFunction( kDisposeShiners );

#endif __DialogMgrPriv__
