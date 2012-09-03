/*
	File:		DialogMgr.c

	Contains:	Pop-up modal dialogs

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

	   <100>	 8/28/94	CMY		Changed the color of the green text in the PlayAgainDialog to be
									less obnoxious.
		<99>	 8/28/94	JOE		When a dialog comes up, tell the hardware keyboard to return
									Button A, regardless of what someone like software keyboard told
									it.  Undo that on the way out.
		<98>	 8/27/94	KON		Exploding dialogs went away because the adjustment to the
									template height on the trimming code was hosing us. The height
									is now adjusted as a character, rather than a pixel, and the
									exploding height value is the original value so the correct
									explosions can be used.
		<97>	 8/27/94	HEC		Fixed PlayAgainDialog timeout.
		<96>	 8/26/94	HEC		Made DoPlayAgainDialog time out.
		<95>	 8/25/94	JBH		Removed hue-cycling password box, as there are times when we
									have no free CLUTs.
		<94>	 8/25/94	ADS		Allow start button on all dialogs
		<93>	 8/25/94	CMY		Do a little better on the antialiasing of red and green text
									against the gray dialog backgrounds.
		<92>	 8/24/94	SAH		Use 19 bit packet format.
		<91>	 8/24/94	CMY		Fixed code that was testing against kModemTryingToConnect.
									Should have been using "=" and not "&"
		<90>	 8/23/94	JBH		Made frame for passwords thinner., as per Joey's request.
									Centered it on screen as well.
		<89>	 8/22/94	JBH		Remove annoying flicker in password dialog caused by shiners.
		<88>	 8/22/94	ADS		tweak
		<87>	 8/22/94	JBH		Removed Andy's beloved sparkles. Made password dialog box have a
									box around the password X's.
		<86>	 8/21/94	JOE		Add sound FX for dialog choosin'
		<85>	 8/21/94	HEC		DoAnyResponse take news template param. Return hibit set in
									dialog result when countdown timer elapses.
		<84>	 8/21/94	KON		DrawSpriteInFront now takes a hueShift.
		<83>	 8/20/94	HEC		Auto-position countdown timer in adjustable dialogs.
		<82>	 8/19/94	ADS		Sprite init problem when shiners fired before sparkles
		<81>	 8/19/94	ADS		Shinermania
		<80>	 8/19/94	KON		Shrink dialogs vertically if there's too much extra space.
		<79>	 8/17/94	JOE		Let X, Y, or Z also dismiss dialogs
		<78>	 8/15/94	HEC		timeout data should be long, not long *.
		<77>	 8/15/94	HEC		Can now pass in an autodismiss time in DoDialogItem's data param
									for progress time.
		<76>	 8/10/94	KON		Remove dismissal animation.
		<75>	 8/10/94	JBH		Save / restore text state around dialog manager (for deferred
									dialogs). Also added a "debug" way to get a large deferred
									dialog.
		<74>	  8/9/94	CMY		Added a flag to show/hide backdrop.
		<73>	  8/9/94	JOE		Data Driven Dialogs now have attached sound FX IDs
		<72>	  8/8/94	HEC		Removed READCONTROLS_ONVBL #ifdef around ControllerVBL().
		<71>	  8/8/94	JBH		Pennance for the nasty shiner code. Use graphics now, and moved
									shiner code outside of DoDialogItem (vectorized, too!)
		<70>	  8/8/94	KON		Fix merge nightmare.
		<69>	  8/8/94	KON		Use hide and show backdrop procs for removing background in low
									pattern mem situations. Flush events after dismissing dialog
									box.
		<68>	  8/7/94	JBH		Added shiny border animations. Also made text draw only when the
									blink state changed (or selection changed) in the response
									procedures.
		<67>	  8/6/94	JBH		Make play again dialog use GTLooseSession stuff
		<66>	  8/6/94	HEC		In DoAnyResponse, return kDialogYes if modem trying to connect
									during call waiting in peer connect.
		<65>	  8/5/94	SAH		Made the play again dialog quit much more cleanly.
		<64>	  8/4/94	JBH		De-seeded RandomShort calls.
		<63>	  8/4/94	CMY		Don't make palettes gray when disposing background. It's not
									visually pleasing on a TV.
		<62>	  8/4/94	JBH		Modified Play Again dialog font colors. Needs testing on a color
									monitor...
		<61>	  8/3/94	CMY		The dialog frame was drawing the colors in an incorrect order.
		<60>	 7/31/94	SAH		Modified some gametalk timeouts. The chat SegaOSIdle is really a
									network code idle.
		<59>	 7/30/94	CMY		When we kill the background to make room for a dialog, make the
									bg color black, not gray.
		<58>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<57>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<56>	 7/26/94	CMY		Updated to pass VBL flag to SetupClut.
		<55>	 7/25/94	HEC		Call FlushUserEvents after dialog setup in DoDialogItem().
		<54>	 7/23/94	SAH		Make sure to shutdown our gametalk session when we're done.
		<53>	 7/22/94	SAH		New gametalk.
		<52>	 7/22/94	JBH		Changed background color when pattern mems is blown away. Fixed
									a bug in DataDrivenDismissal where dialog pieces would be strewn
									over the screen on the Sega (but not the simulator!!).
		<51>	 7/21/94	JOE		Added flags param to SegaBoxText()
		<50>	 7/21/94	ADS		Fixed hilite blinks for PW entry too
		<49>	 7/21/94	JOE		trigger Password FX
		<48>	 7/20/94	JOE		use new sound FX constants
		<47>	 7/20/94	CMY		Fixed blinking hilight colors
		<46>	 7/17/94	JBH		Tweaked logic in DoPlayAgainDialog.
		<45>	 7/17/94	KON		Update for new fonts.
		<44>	 7/17/94	JBH		Added secret keys to dialogs. Also added a GameTalk-controlled
									"Play Again" dialog (special-case).
		<43>	 7/16/94	JBH		Made cluts 1-3 fade to gray values when background pattern
									memory is released.
		<42>	 7/13/94	ADS		Vectorize EVERYTHING
		<41>	 7/13/94	CMY		Fixed ParamText to support the case where a paramtext string is
									nil, but is referenced in a Dialog.
		<40>	 7/11/94	SAH		Fixed bug where DoPassword was trashing random memory while
									drawing the password on the screen.
		<39>	 7/10/94	ADS		Call progress indicators' IdleProc so they can animate
		<38>	 7/10/94	ADS		Add template-controlled progress indicators
		<37>	 7/10/94	JOE		play FX indirectly
		<36>	  7/8/94	ADS		Added semi-universal dialogs
		<35>	  7/8/94	ADS		(temp) show password input chars
		<34>	  7/6/94	ADS		Add anonymous feedback to password dialog
		<33>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<32>	  7/6/94	KON		Refix merge hell.
		<31>	  7/6/94	KON		Fix merge hell.
		<30>	  7/6/94	KON		Make minimum dialog time 2 seconds.
		<29>	  7/6/94	ADS		Added "password" dialogs
		<28>	  7/3/94	JBH		Allow dialogs to temporarily dispose of backdrop patterns if
									pattern memory is full.
		<27>	  7/3/94	JBH		Initialized direction, in order to make Chris happy.
		<26>	  7/2/94	JBH		Allow dialog dismissals to be played backwards (for bring-ins),
									also slowed them down on the real Sega.
		<25>	  7/2/94	JBH		Added data-driven dialog dismissals.
		<24>	  7/1/94	KON		Added minimum and maximum times a dialog can stay up. User
									interaction can always override the minimum.
		<23>	  7/1/94	KON		Wait a minimum amount of time before allowing remote modem to
									interrupt a dialog. Time set in header file (3 seconds
									currently).
		<22>	 6/27/94	SAH		Make SoftInit return noErr.
		<21>	 6/22/94	CMY		Used default pitch and duration for the sounds.
		<20>	 6/21/94	CMY		Randomize animations (CMY) and fix jizzled checkin from last rev
									(Kon).
		<19>	 6/21/94	SAH		(Kon) Fixed dialog delay on peer connect
		<18>	 6/21/94	KON		Add SetDialogTimeOutDelay.
		<17>	 6/21/94	CMY		Exploding Dialogs.
		<16>	 6/20/94	KON		Added DoDialogItem call so DeferredDialogMgr can create dialogs
									directly.
		<15>	 6/20/94	CMY		Animated dialogs. Fixed the Yes/No trashed text on Sega.
		<14>	 6/18/94	CMY		Suspend background animations when a dialog is up.
		<13>	 6/18/94	BET		(Really CMY) fix the fucker.
		<12>	 6/17/94	CMY		Fix the textBox flowing logic for better text placement.
		<11>	 6/17/94	CMY		Fix the bottom left corner of the dialog frame.
		<10>	 6/16/94	CMY		Set the VDP increment *before* setting the VRAM address.
		 <9>	 6/16/94	BET		Revert Ted's changes.
		 <8>	 6/15/94	SAH		Fixed the DismissalTimeProc some.
		 <7>	 6/15/94	CMY		ParameterText!
		 <6>	 6/15/94	CMY		Default items for dialogs.
		 <5>	 6/15/94	HEC		SegaOSIdle now takes a param
		 <4>	 6/15/94	CMY		Support for Yes/No dialogs.
		 <3>	 6/15/94	KON		Cheesy fix to use the right dialog DBID rather than zero.
		 <2>	 6/14/94	CMY		Make dialogs database-driven.
		 <1>	 6/14/94	CMY		first checked in

	To Do:
*/

#include "SegaOS.h"
#include "SegaScrn.h"
#include "OSManagers.h"
#include "Errors.h"
#include "Events.h"
#include "Heaps.h"
#include "Pattern.h"
#include "GraphicsDB.h"
#include "DBTypes.h"
#include "SegaVDP.h"
#include "Time.h"
#include "SegaText.h"
#include "Decompress.h"
#include "Sprites.h"
#include "TextUtls.h"
#include "DialogMgr.h"
#include "DialogMgrPriv.h"
#include "SegaIn.h"
#include "StringDB.h"
#include "Animation.h"
#include "SegaSound.h"
#include "Utils.h"
#include "Backdrops.h"
#include "ModemMisc.h"
#include "UsrConfg.h"
#include "News.h"
#include "Secrets.h"
#include "SegaCluts.h"
#include "BoxSer.h"
#include "NetErrors.h"
#include "VBLManager.h"
#include "GTSendData.h"
#include "Controls.h"		// so we read controllers at DialogVBL
#include "DebugSelectors.h"
#include "controls.h"

#if TESTNASTYDIALOG
#include "DeferredDialogMgr.h"
#endif

#ifdef SIMULATOR 
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif


#define Pixel8(a,b,c,d,e,f,g,h)	(a<<28|b<<24|c<<20|d<<16|e<<12|f<<8|g<<4|h)
#define Pixel8Reverse(a,b,c,d,e,f,g,h)	(h<<28|g<<24|f<<20|e<<16|d<<12|c<<8|b<<4|a)

/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
DialogGlobals dialog;
#endif

void _SetDialogColors(short background, short frame1, short frame2, short frame3,
	short frame4, short frame5);
short _DoDialog(DBID which);
short _DialogParameterText(char *p1, char *p2, char *p3, char *p4);
short _DoDialogItem( CPDialog *dbDialog, void* data );
short _DoDialogParam(DBID whichDialog, void* data);


char *	_CopyString(const char *src, char *dst);
short 	_DoAnyResponse( Rect *textBox, char defaultItem, long minTime, long maxTime, CPDialog *dbDialog, CPDialogTemplate *template );
short 	_DoDataDrivenDismissal(SpriteRef sprite, short width, short height, short left, short top, short forward);
short	_DoPassword( Rect *textBox, char defaultItem, long minTime, long maxTime, short bgColor, void* param );
short	_DoPlayAgainDialog( Rect *textBox, char defaultItem, long minTime, long maxTime, CPDialog *dbDialog, void *param );
void	_DrawDialogFrame(segaCharRect *frameBounds);
void	_FillTextRectangle(segaCharRect *bounds, short bgColor);
void	_HorizontalLine(short x1, short x2, short y, short color);
void	_KillProgressTimer(ProgressTimer* theTimer);
void	_ReplaceParameters(char *message, char *buffer);
void	_SetupProgressTimer(ProgressTimer* theTimer, CPDialogTemplate* dialog, long endTime);
void	_VerticalLine(short y1, short y2, short x, long colorPattern);
Shiners *_CreateShiners( segaCharRect *dialogCharRect, CPDialogTemplate *template );
void	_DisposeShiners( Shiners *theShiners );

//
// Static protos
//
static long RunDialogProgress ( long time, long data );
static void HorizontalCharacterLine(short x1, short x2, short y, short color);

#if	TESTNASTYDIALOG
static long PutUpNastyDialog( long time, long ref );
#endif

long
_DialogControl ( short command, long data )
{
long			offset;
short			error;
DialogGlobals	*globals;
short			ii;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(dialog,offset);
			
			error = AllocateGlobalSpace ( kDialogManager, offset, sizeof(DialogGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create dialog globals" );
			if ( error != noErr )
			{
				return error;
			}
				
			/* install our selectors */
			SetDispatchedFunction ( kSetDialogColors,		kDialogManager,		_SetDialogColors );
			SetDispatchedFunction ( kDoDialog,				kDialogManager,		_DoDialog );
			SetDispatchedFunction ( kDoDialogParam,			kDialogManager,		_DoDialogParam );
			SetDispatchedFunction ( kDialogParameterText,	kDialogManager,		_DialogParameterText );
			SetDispatchedFunction ( kDoDialogItem,			kDialogManager,		_DoDialogItem );

			SetDispatchedFunction ( kCopyString,			kDialogManager,		_CopyString );
			SetDispatchedFunction ( kDoAnyResponse,			kDialogManager,		_DoAnyResponse );
			SetDispatchedFunction ( kDoDataDrivenDismissal,	kDialogManager,		_DoDataDrivenDismissal );
			SetDispatchedFunction ( kDoPassword,			kDialogManager,		_DoPassword );
			SetDispatchedFunction ( kDoPlayAgainDialog,		kDialogManager,		_DoPlayAgainDialog );
			SetDispatchedFunction ( kDrawDialogFrame,		kDialogManager,		_DrawDialogFrame );
			SetDispatchedFunction ( kFillTextRectangle,		kDialogManager,		_FillTextRectangle );
			SetDispatchedFunction ( kHorizontalLine,		kDialogManager,		_HorizontalLine );
			SetDispatchedFunction ( kKillProgressTimer,		kDialogManager,		_KillProgressTimer );
			SetDispatchedFunction ( kReplaceParameters,		kDialogManager,		_ReplaceParameters );
			SetDispatchedFunction ( kSetupProgressTimer,	kDialogManager,		_SetupProgressTimer );
			SetDispatchedFunction ( kVerticalLine,			kDialogManager,		_VerticalLine );
			SetDispatchedFunction ( kCreateShiners,			kDialogManager,		_CreateShiners );
			SetDispatchedFunction ( kDisposeShiners,		kDialogManager,		_DisposeShiners );

			break;
		
		case kSoftInialize:
			/* init our globals here */
			{
				short *color;
				char **params;
				
				params = REFGLOBAL(dialog,paramText);
				*params++ = nil;
				*params++ = nil;
				*params++ = nil;
				*params++ = nil;
				
				REFGLOBAL(dialog,bgColor) = 13;
				color = REFGLOBAL(dialog,frameColors);
				*color++ = 15;
				*color++ = 14;
				*color++ = 13;
				*color++ = 14;
				*color++ = 15;

	#if TESTNASTYDIALOG		/* debugging purposes, allows us to put up a large dialog anywhere */
				AddTimeRequest( PutUpNastyDialog, 0 );
	#endif
			}
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}

/* Draw a horizontal line in the text gdevice in the given color. */
void _HorizontalLine(short x1, short x2, short y, short color)
{
Ptr patMemory;
short hBump;
short unused;
Ptr end;
	
	color = color<<12 | color<<8 | color<<4 | color;
	patMemory = GetTextPatternAddress(x1, y, &unused);
	hBump = GetTextPatternAddress(x1+8, y, &unused) - patMemory;
	end = GetTextPatternAddress(x2, y, &unused);
	
	OwnVDP();
	
	SetVDPIncrement(2);
	
	while (patMemory < end) 
	{
		SetVDPVRAMAddress((long)patMemory);
		WriteWordToVDP(color);
		WriteWordToVDP(color);
		patMemory += hBump;
	}
	FreeVDP();
}

/* Draw an 8 pixel high horizontal line in the text gdevice in the given color. */
void HorizontalCharacterLine(short x1, short x2, short y, short color)
{
Ptr patMemory;
short hBump;
short unused;
register unsigned long fillData;
short	count;
short	iii;

	color = color<<12 | color<<8 | color<<4 | color;		// Replicate color into all 4 pixels in the word
	fillData = ((unsigned long)color << 16) | (unsigned long)color;
	
	patMemory = GetTextPatternAddress(x1, y, &unused);
	hBump = GetTextPatternAddress(x1+8, y, &unused) - patMemory;
	count = (x2 - x1);
	OwnVDP();
	
	for( iii = count; iii > 0; iii-- ) 
	{
		FillVRAM(fillData, patMemory, 8);
		patMemory += hBump;
	}
	FreeVDP();
}

/* Draw a 8-pixel wide vertical line in the given color pattern */
void _VerticalLine(short y1, short y2, short x, long colorPattern)
{
Ptr patMemory;
short scanlinesLeft;
short scanlines = y2-y1;
short y = y1;
	
	OwnVDP();
	SetVDPIncrement(2);
	
	while (scanlines) 
	{
		patMemory = GetTextPatternAddress(x, y, &scanlinesLeft);
		SetVDPVRAMAddress((long)patMemory);
		if (scanlinesLeft > scanlines)
			scanlinesLeft = scanlines;
		scanlines -= scanlinesLeft;
		y += scanlinesLeft;
		
		while (scanlinesLeft--) 
		{
			WriteWordToVDP(colorPattern>>16);
			WriteWordToVDP(colorPattern);
		}
	}
	
	FreeVDP();
}

/* Draw the frame of the dialog. The charRect is local to the dialog's text GDevice.
 * Thus: 0,0 is top,left of dialog char cells.
 */
void _DrawDialogFrame(segaCharRect *frameBounds)
{

	{	// First d