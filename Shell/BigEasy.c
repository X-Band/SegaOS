/*
	File:		BigEasy.c

	Contains:	xxx put contents here xxx

	Written by:	Paul Mercer's Mom

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 8/19/94	HEC		Pass param to DoIntroStuff.
		 <6>	 7/24/94	HEC		StopVBLTask in DoQuit
		 <5>	 7/10/94	SAH		Conditionally compile calls to intro.c out for ROMDB build.
		 <4>	 6/30/94	SAH		Only call ResetDebugger if DEBUG.
		 <3>	 6/28/94	CMY		Fixed the case where you reset with debug windows open in
									non-threaded.
		 <2>	 6/10/94	CMY		Added sega reset code to simulator

	To Do:
*/


#include <Quickdraw.h>
#include <Fonts.h>
#include <Events.h>
#include <OSEvents.h>
#include <Menus.h>
#include <Windows.h>
#include <Dialogs.h>
#include <Desk.h>
#include <Memory.h>
#include <Resources.h>
#include <AppleEvents.h>
#include <Dialogs.h>
#include <SetJmp.h>

#include "Main.h"
#include "BigEasy.h"
#include "Dispatcher.h"
#include "Intro.h"
#include "Exceptions.h"
#ifdef DEBUG
	#include "simDebugger.h"
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Limits
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define WMax 	30		/* Number of windows in list */
#define MMax 	15		/* Number of menus in menubar */
#define MIMax 	30		/* Number of items per menu */

#define	LoMemROM85		(* (unsigned short*) 0x28E)		/* ROM version */
#define	HaveCQD()		(LoMemROM85 <= 0x3FFF)			/* true if color quickdraw is present */

Rect bigRect = {-16000,-16000,16000,16000};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Types
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*** Windows ***/
typedef struct				/* This clever little structure contains */
{							/* pointers to routines for each type of event. */
	UpdateProc		wUpdate;
	ClickProc		wClick;
	GrowProc		wGrow;
	ZoomProc		wZoom;
	KeyProc			wKey;
	CloseProc		wClose;
	ActivateProc	wActivate;
	DeactivateProc	wDeactivate;
	LoopyProc		wLoopy;
	WindowPtr		wWindow;
	WindowGlobalsPtr	wGlobals;
} WindowObject, *WindowObjectPtr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Globals
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WindowObject gWindowObjectList[WMax];	/* Maximum of 30 controlled windows */
short gWindowCount = 0;					/* Number of windows defined */

/*** Menus ***/
MenuHandle gMenuHandleList[MMax];		/* MenuHandles indexed by MenuID's */
short gMenuLengths[MMax];				/* MenuHandles indexed by MenuID's 		*/
IFP gMenuProcs[MMax][MIMax];			/* Array of procedures to call from menus */
short gMenuRefs[MMax][MIMax];			/* Array of menu reference numbers		*/
short gMenuCount;						/* How many menus defined */
short gMenuItemCount;					/* How many items in last menu */
MenuHandle gMenuEdit;					/* Which one is the edit menu? */

/*** Application Edit Commands ***/
IFP AppUndo,AppCut,AppCopy,AppPaste,AppClear;

/*** Global Status ***/
Boolean gQuitApp = false;
long gSleepTime = 60;

/* Global Routines */
IFP	gAboutProc = nil;			/* Do the about box */
IFP	gMenuSetupProc = nil;		/* Do the menu setup */
RFP	gGetMouseRgnProc = nil;

/* On-Demand Reset to test reliability */
jmp_buf	resetJumpBuffer;
Boolean gSimulatorResetRequest;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// prototypes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void DefaultAbout( void );

void InitToolbox(void);
void BEUndo(void);
void BECut(void);
void BECopy(void);
void BEPaste(void);
void BEClear(void);
void BENull(void);
void MenuPoint(long theclick);
void DoKeyPress(long k);
void MenuClick(Point p);
void StartMenus(void);
void EnableMenu(MenuHandle menu, short item, Boolean enable);
void EnDis(MenuHandle m, short i, short f);
void DoMouseClick(EventRecord *event);
WindowObjectPtr ScanWindowObjectList(WindowPtr w, short *n);
GrafPtr	SaveGrafPort(GrafPtr newPort);
void FrontDA(void);
void EventLoop(void);

/*
** Prototypes for externally defined functions
*/
extern void Bootstrap(int resetFlag);
extern void InitVars(void);
extern void CleanUp( void );
extern void StopIntroStuff( void );

/************************************
* Some useful routines
************************************/

void PStrCpy( unsigned char* s, unsigned char* t )
{
short	i, length;

	length = s[0]+1;
	for(i=0;i<length;i++)
	{
		t[i] = s[i];
	}
}	

short PutUpMessage( short cancel, unsigned char* text )
{
DialogPtr	myDialog;
short	itemHit;
Rect		myRect;
GrafPtr	savePort;
DitlHndl	myDitl;
DItemPtr	dPtr;
short	delta;
short	numDitlItems = 1+cancel;
	
	myDitl = (DitlHndl)NewHandle(sizeof(Ditl)+(sizeof(DitlItem)*numDitlItems )+text[0]+10);
	(**myDitl).count = numDitlItems;

	dPtr = (**myDitl).item;
	dPtr->placeholder = 0;
	SetRect( &myRect, 200, 100, 260, 120 );
	dPtr->displayRect = myRect;
	dPtr->type = btnCtrl+ctrlItem;
	PStrCpy("\pOK", (unsigned char*)dPtr->title);
	delta = 2;							/* we need to accumulate the extra data in the ditlItem */
	
	if( cancel ){
		dPtr = (DItemPtr)((char*)(&((**myDitl).item[1]))+delta);
		dPtr->placeholder = 0;
		SetRect( &myRect, 40, 100, 100, 120 );
		dPtr->displayRect = myRect;
		dPtr->type = btnCtrl+ctrlItem;
		PStrCpy("\pCancel", (unsigned char*)dPtr->title);
		delta += 6;						/* we need to accumulate the extra data in the ditlItem */
		}
		
	dPtr = (DItemPtr)((char*)(&((**myDitl).item[numDitlItems]))+delta);
	dPtr->placeholder = 0;
	SetRect( &myRect, 20, 20, 300, 80 );
	dPtr->displayRect = myRect;
	dPtr->type = statText;
	PStrCpy(text, (unsigned char*)dPtr->title);

	GetPort( &savePort );
	SetRect( &myRect, 50, 50, 400, 200 );
	myDialog = NewDialog( 0, &myRect,0,true,dBoxProc,(WindowPtr)-1,false,0,(Handle)myDitl );
	SetPort( (GrafPtr) myDialog );

	do{
		ModalDialog( 0, &itemHit );
	} while( itemHit == 0 );
	
	FlushEvents( everyEvent, 0 );
	DisposDialog( myDialog );
	SetPort( savePort );

	if( itemHit == 1 )
		return( itemHit );
	else
		return( 0 );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void InitToolbox(void)
  {
  MoreMasters();
  MoreMasters();

  InitGraf(&qd.thePort);
  InitFonts();
  FlushEvents(0xffff,0);
  InitWindows();
  InitMenus();
  InitDialogs(0);
  TEInit();
  InitCursor();
  MaxApplZone();
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WindowGlobalsPtr GetWindowGlobals(WindowPtr w)
{
	WindowObjectPtr	wo;
	short			i;

	wo = ScanWindowObjectList(w, &i);
	if( wo )
		return (wo->wGlobals);
	else
		return (nil);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/************************************
* Some inherent methods
************************************/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void BEUndo(void)
  {
  if(!SystemEdit(0))
    (*AppUndo)();
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void BECut(void)
  {
  if(!SystemEdit(2))
    (*AppCut)();
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void BECopy(void)
  {
  if(!SystemEdit(3))
    (*AppCopy)();
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void BEPaste(void)
  {
  if(!SystemEdit(4))
    (*AppPaste)();
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void BEClear(void)
  {
  if(!SystemEdit(5))
    (*AppClear)();
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// default routines

void BENull(void){}
void DefaultUpdate(WindowGlobalsPtr globals) {}
void DefaultClick(WindowGlobalsPtr globals, Point p, short mods, long when) {}
void DefaultGrow(WindowGlobalsPtr globals, Point p, short mods) {}
void DefaultKey(WindowGlobalsPtr globals, long k, short mods) {}
void DefaultClose(WindowGlobalsPtr globals) {}
void DefaultActivate(WindowGlobalsPtr globals) {}
void DefaultDeactivate(WindowGlobalsPtr globals) {}
void DefaultLoopy(WindowGlobalsPtr globals) {}
void pnull(void) {}
static void DefaultAbout( void ){ SysBeep( 0 ); }
static RgnHandle DefaultGetMouseRgn( void ){ return nil; }
void DefaultZoom( WindowGlobalsPtr storage, Point p, short partCode )
{
	
	if (TrackBox(FrontWindow(), p, partCode))
	{
		ZoomWindow(FrontWindow(), partCode, true);
		InvalRect(&qd.thePort->portRect);
	}
}

/************************************
* Menu Action Routines
************************************/

void SetAboutProc( IFP aboutProcPtr )
{
	gAboutProc = aboutProcPtr;
}

void SetGetMouseRgnProc( RFP mouseRgnProcPtr )
{
	gGetMouseRgnProc = mouseRgnProcPtr;
}

void SetMenuSetupProc( IFP menuSetupProcPtr )
{
	gMenuSetupProc = menuSetupProcPtr;
}

void SetSleepTime( long sleepTime )
{
	gSleepTime = sleepTime;
}

void DoKeyPress(long k)
{
	if( gMenuSetupProc )
		(*gMenuSetupProc)();	/* setup the menu state */

	MenuPoint(MenuKey((char)k));
}
	
void MenuClick(Point p)
{
	if( gMenuSetupProc )
		(*gMenuSetupProc)();	/* setup the menu state */
			
	MenuPoint(MenuSelect(p));
}

void MenuPoint(long theclick)
{
short mID,mItem;
Str255 DAname;
short wNum;
WindowObject *thisWindow;
IFP aProc;

	mID = theclick>>16;
	mItem = theclick & 0xffff;

	if(mID == 0) return;

	if(mID == 1)							/* Apple menu */
	{
		if(mItem == 1)						/* Its either the about box */
			(*gAboutProc)();
		else	
		{									/* Or a DA */
			GetItem(gMenuHandleList[1],mItem, DAname);
			OpenDeskAcc(DAname);
		}
	}
	else
	{
		aProc = gMenuProcs[mID][0];
		if(aProc)										/* call the menu's routine, if any		*/
			(*aProc)();
		aProc = gMenuProcs[mID][mItem];
		if(aProc)										/* call the item's routine, if any		*/
			(*aProc)();
	}
	HiliteMenu(0);
}

void StartMenus()
/*
 * Start with just an About box, and some DA's.
 */
{
	MenuHandle applemenu;
	applemenu = NewMenu(1,"\p");				/* Apple menu: ID 1					*/
	AppendMenu(applemenu,"\pAbout...;(-");
	AddResMenu(applemenu,'DRVR');
	InsertMenu(applemenu,0);
	DrawMenuBar();
	gMenuHandleList[1] = applemenu;

	gMenuCount = 1;							/* Next menu added will be ID 2		*/
	gMenuLengths[1] = 0;						/* No items, really, in Apple menu		*/

	SetAboutProc( &DefaultAbout );
	SetGetMouseRgnProc( &DefaultGetMouseRgn );
}

void InstallMenu(Str255 s, IFP action, short ref)
/*
 * Start a new menu with name s
 */
{
	gMenuCount++;								/* new menu						*/
	gMenuItemCount = 0;							/* with no entries					*/

	gMenuLengths[gMenuCount] = 0;
	gMenuHandleList[gMenuCount] = NewMenu(gMenuCount,s);
	InsertMenu(gMenuHandleList[gMenuCount],0);
	gMenuProcs[gMenuCount][gMenuItemCount] = action;
	gMenuRefs[gMenuCount][gMenuItemCount] = ref;
	gMenuItemCount++;
	DrawMenuBar();
}

void InstallItem(Str255 s, IFP action, short ref)
/*
 * Add an item to the last menu, and associate a routine with it.
 */
{
	register MenuHandle mh;

	mh = gMenuHandleList[gMenuCount];

	if (ref < 0)
	{
		DisableItem(mh,gMenuItemCount);
		ref =- ref;
	}

	AppendMenu(mh,s);
	gMenuProcs[gMenuCount][gMenuItemCount] = action;
	gMenuRefs[gMenuCount][gMenuItemCount] = ref;
	gMenuLengths[gMenuCount] = ++gMenuItemCount;
}

void InstallEditMenu(IFP Xundo,IFP Xcut,IFP Xcopy,IFP Xpaste,IFP Xclear)
/*
 * Start an edit menu, and put the first six things in.
 */
{
	AppUndo = Xundo;
	AppCut = Xcut;
	AppCopy = Xcopy;
	AppPaste = Xpaste;
	AppClear = Xclear;

	InstallMenu("\pEdit",nil,0);
	InstallItem("\px/Z", (IFP)BEUndo,-1);
	InstallItem("\p(-", (IFP)BENull,0);
	InstallItem("\px/X", (IFP)BECut,-2);
	InstallItem("\px/C", (IFP)BECopy,-3);
	InstallItem("\px/V", (IFP)BEPaste,-4);
	InstallItem("\px/B", (IFP)BEClear,-5);
	
	gMenuEdit = gMenuHandleList[gMenuCount];

}

void SetMenuItem(short ref, char enable, char isMarked, char mark,  unsigned char *s)
/*
  * Set all items with the reference number to the
  * enable/disable state, with mark, and name s.
  * pass enable + to enable, - to disable, and
  * zero to leave alone.
  */
	{
	short m,i;
	MenuHandle mh;

	for(m=1; m<=gMenuCount; m++)
		{
		mh = gMenuHandleList[m];
		for(i=gMenuLengths[m]; i>=1; i--)
			if(gMenuRefs[m][i] == ref)
				{
				if(enable > 0)
					EnableItem(mh,i);
				else if (enable < 0)
					DisableItem(mh,i);

				if(isMarked)
					if(isMarked > 0)
						SetItemMark(mh,i,mark);
					else
						SetItemMark(mh,i,' ');
				if(s)
					SetItem(mh,i,s);
				}
		}
	}

void EnDisEdits(short Eundo, short Ecut, short Ecopy, short Epaste, short Eclear)
/*
 * For each edit menu entry,
 * -1=disable, 1=enable, 0=leave alone.
 */
	{
	SetMenuItem(1,Eundo,false,0,"\pUndo");
	SetMenuItem(2,Ecut,false,0,"\pCut");
	SetMenuItem(3,Ecopy,false,0,"\pCopy");
	SetMenuItem(4,Epaste,false,0,"\pPaste");
	SetMenuItem(5,Eclear,false,0,"\pClear");
	}


/************************************
* Mouse Action Routines
************************************/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DoMouseClick(EventRecord *event)
  {
  WindowPtr			w;
  WindowObjectPtr	wo;
  short				part;
  short				i;

  part = FindWindow (event->where, &w);

  switch (part)
    {
    case inDesk:		/* click on desk */
      break;

    case inMenuBar:		/* menubar */
      MenuClick(event->where);
      break;

    case inSysWindow:		/* system window */
      SystemClick(event,w);
      break;

    case inContent:		/* content region */
	case inZoomIn:		/* zoom in */
	case inZoomOut:		/* zoom out */
    case inGrow:		/* growregion */

		if (w != FrontWindow())							/* If clicked on a non-front window, */
		{
			SelectWindow(w);							/* bring it to the front. */
			SetPort(w);
		}
		else if (wo = ScanWindowObjectList(w, &i))		/* Click on front window: give click */
	    {												/* to window's click routine. */
	 		if (part == inContent)
	        	(*wo->wClick)(wo->wGlobals, event->where, event->modifiers, event->when);
	        else if (part == inGrow)
	       		(*wo->wGrow)(wo->wGlobals, event->where, event->modifiers);
	       	else
	       		(*wo->wZoom)(wo->wGlobals, event->where, part);
        }
      break;

    case inDrag:		/* drag */
      DragWindow (w,event->where,&bigRect);
      break;

    case inGoAway:		/* goAway */
		if (TrackGoAway(w,event->where))
			CloseBEWindow( w );
		break;
    }
  }

/************************************
* Window Action Routines
************************************/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CloseBEWindow( WindowPtr w )
{
WindowObjectPtr	wo;
short			i;
GrafPtr			savePort;
Boolean			wasFront = false;

	if( w == FrontWindow() )
		wasFront = true;
		
	GetPort( &savePort );
	if (wo = ScanWindowObjectList(w, &i))
	{
		savePort = SaveGrafPort( w );
		(*wo->wClose)(wo->wGlobals);
		SetPort( savePort );
		
		CloseWindow( w );
		DisposePtr((Ptr) w );
		gWindowObjectList[i] = gWindowObjectList[--gWindowCount];
	}
	
	if( wasFront )
		SetPort( FrontWindow() );

}

WindowGlobalsPtr InstallWindow(Rect *iRect, short iType, Boolean iGoway, unsigned char *iTitle,
				  UpdateProc iUpdate, ClickProc iClick, GrowProc iGrow, ZoomProc iZoom, KeyProc iKey,
				  CloseProc iClose, ActivateProc iActivate, DeactivateProc iDeactive,
				  LoopyProc iLoopy, long iGlobalsSize, long refCon, WindowPtr *thisWindowPtr)
/*
 * Add a window to BigEasy's list. If the window is
 * already up somewhere, bring it to the front and
 * visualize it.
 */
{
	WindowObject	*thisWindow;
	Ptr				p;

  	thisWindow = &gWindowObjectList[gWindowCount];

   	p = NewPtrClear(sizeof(CWindowRecord) + iGlobalsSize);		/* make room for window record + globals */
	if (!p)
		return (nil);

	if (HaveCQD())
		thisWindow->wWindow = NewCWindow(p, iRect, iTitle, false, iType, (WindowPtr) -1, iGoway, refCon);
	else
		thisWindow->wWindow = NewWindow(p, iRect, iTitle, false, iType, (WindowPtr) -1, iGoway, refCon);

	if (!thisWindow->wWindow)
	{
		DisposePtr(p);
		return (nil);
	}

	thisWindow->wUpdate = iUpdate;
	thisWindow->wClick = iClick;
	thisWindow->wKey = iKey;
	thisWindow->wClose = iClose;
	thisWindow->wGrow = iGrow;
	thisWindow->wZoom = iZoom;
	thisWindow->wActivate = iActivate;
	thisWindow->wDeactivate = iDeactive;
	thisWindow->wLoopy = iLoopy;
	thisWindow->wGlobals = (WindowGlobalsPtr) ( (char*)p + sizeof(CWindowRecord) );

	gWindowCount++;

	*thisWindowPtr = thisWindow->wWindow;
	return( thisWindow->wGlobals );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WindowGlobalsPtr GetInstallWindow(short windID,
				  UpdateProc iUpdate, ClickProc iClick, GrowProc iGrow, ZoomProc iZoom, KeyProc iKey,
				  CloseProc iClose, ActivateProc iActivate, DeactivateProc iDeactive,
				  LoopyProc iLoopy, long iGlobalsSize, WindowPtr *thisWindowPtr)

{
	WindowGlobalsPtr	result;
	WindowTHndl		windTemplate;

	windTemplate = (WindowTHndl) GetResource('WIND', windID);
	if (windTemplate == nil) {
		Debugger();
		return (nil);
	}

	HLock((Handle)windTemplate);
	result = InstallWindow(&((**windTemplate).boundsRect),
							(**windTemplate).procID,
							(**windTemplate).goAwayFlag,
							(**windTemplate).title,
							iUpdate, iClick, iGrow, iZoom, iKey, iClose, iActivate,
							iDeactive, iLoopy,
							iGlobalsSize, 0, thisWindowPtr);
	HUnlock ((Handle)windTemplate);
	HPurge ((Handle)windTemplate);
	return (result);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WindowGlobalsPtr InstallDialog(short iResID, ClickProc iClick, CloseProc iClose, long iGlobalsSize, DialogPtr *thisWindowPtr)
/*
 * Add a dialog to BigEasy's list. If the window is
 * already up somewhere, bring it to the front and
 * visualize it.
 */
  {
	WindowObject	*thisWindow;
	Ptr				p;

	if (*thisWindowPtr != 0)			/* something already assigned to this window? */
    {
		if(!((WindowPeek) *thisWindowPtr)->visible)	/* If so, just show it, and bring it to the front. */
			ShowWindow((WindowPtr) *thisWindowPtr);
		SelectWindow((WindowPtr) *thisWindowPtr);
		return (nil);
	}


	thisWindow = &gWindowObjectList[gWindowCount];

   	p = NewPtrClear(sizeof(DialogRecord) + iGlobalsSize);		/* make room for window record + globals */

	thisWindow->wWindow = GetNewDialog(iResID, p, (WindowPtr) -1);
	if (!thisWindow->wWindow)
	{
		DisposePtr(p);
		return (nil);
	}

	thisWindow->wUpdate = (UpdateProc) DefaultUpdate;
	thisWindow->wClick = iClick;
	thisWindow->wKey = DefaultKey;
	thisWindow->wClose = iClose;
	thisWindow->wActivate = DefaultActivate;
	thisWindow->wDeactivate = DefaultDeactivate;
	thisWindow->wLoopy = DefaultLoopy;
	thisWindow->wGlobals = (WindowGlobalsPtr)((char*)p + sizeof(DialogRecord));

	gWindowCount++;

	*thisWindowPtr = (DialogPtr) thisWindow->wWindow;
	return (thisWindow->wGlobals);
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UninstallDialog(DialogPtr uDialog)
{
	short i;

	if (ScanWindowObjectList(uDialog, &i))
	{
		CloseDialog(uDialog);
		DisposePtr((Ptr) uDialog);
		gWindowObjectList[i] = gWindowObjectList[--gWindowCount];
		SetPort(FrontWindow());
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DoCloseWindow(void)
{
	WindowPtr		w;
	WindowObjectPtr	wo;
	short			i;

	w = FrontWindow();
	CloseBEWindow( w );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DoQuit(void)
{
	short			i;
	WindowObjectPtr	wo;
	StopVBLTask();
	wo = gWindowObjectList;
	for( i = gWindowCount - 1; i >= 0; --i )
	{
		CloseBEWindow( wo->wWindow );
		wo++;
	}

	gQuitApp = true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WindowObjectPtr ScanWindowObjectList(WindowPtr w, short *n)
/*
 * Set n to the window number of w, if its there. Return TRUE if
 * the window was found, or FALSE if not.
 */
{
	short i;

	for (i = 0; i < gWindowCount; i++)
		if (gWindowObjectList[i].wWindow == w)
		{
			*n = i;
			return (&gWindowObjectList[i]);
		}

	return (nil);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FrontDA(void)
/*
 * Check the front window. If it's a DA,
 * enable all edit menu options.
 */
  {
  WindowPeek 	window;

  window = (WindowPeek)FrontWindow();
  if (window->windowKind < 0)					/* Any click in a DA window: */
    EnDisEdits(1,1,1,1,1);						/* enable edit menu. */
  }

/************************************
* Event Routines
************************************/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GrafPtr	SaveGrafPort(GrafPtr newPort)
{
	GrafPtr	oldPort;

	GetPort(&oldPort);
	SetPort(newPort);
	return (oldPort);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void EventLoop(void)
{
	EventRecord		ER;
	short			i;
	WindowPtr		w;
	WindowObjectPtr	wo;
	short			item;
	GrafPtr			oldPort;
	RgnHandle		mouseRgn;
	
	mouseRgn = (*gGetMouseRgnProc)();
	WaitNextEvent(everyEvent, &ER, gSleepTime, mouseRgn );


	/* this is an event for the dialog */
	if (IsDialogEvent(&ER))
	{
		if (DialogSelect(&ER, &w, &item))
			if (wo = ScanWindowObjectList(w,&i))
			{
 					oldPort = SaveGrafPort(wo->wWindow);
				(*wo->wClick)(wo->wGlobals, ER.where, item, ER.when);
 					SaveGrafPort(oldPort);
 				return;
			}
	}

	switch (ER.what)
	{
		case nullEvent:		/* null event - give everybody some time */
			GetPort(&oldPort);
			wo = gWindowObjectList;
			for (i = gWindowCount - 1; i >= 0; --i)
			{
				SetPort(wo->wWindow);
				(*wo->wLoopy)(wo->wGlobals);
				wo++;
			}
			SetPort(oldPort);
			break;

		case mouseDown:		/* mouse down */
			FrontDA();
  			DoMouseClick(&ER);
			break;

		case keyDown:			/* key event */
		case autoKey:
			FrontDA();
			if (ER.modifiers & cmdKey)
				DoKeyPress(ER.message);
			else if (wo = ScanWindowObjectList(FrontWindow(), &i))
				(*wo->wKey)(wo->wGlobals, ER.message, ER.modifiers);
			break;

		case updateEvt:		/* update */
			if (wo = ScanWindowObjectList((WindowPtr) ER.message, &i))
			{
				oldPort = SaveGrafPort(wo->wWindow);
				BeginUpdate(wo->wWindow);
					(*wo->wUpdate)(wo->wGlobals);
				EndUpdate(wo->wWindow);
				SaveGrafPort(oldPort);
			}
			break;

		case activateEvt:		/* activate */
			FrontDA();
			if (wo = ScanWindowObjectList((WindowPtr) ER.message, &i))
			{
				SetPort(wo->wWindow);
				if (ER.modifiers & activeFlag)
					(*wo->wActivate)(wo->wGlobals);
				else
					(*wo->wDeactivate)(wo->wGlobals);
			}
			break;
		
		//	NEW KWR 9/17/92 - need this to run
		
		case kHighLevelEvent:	/* Apple Event */
			AEProcessAppleEvent (&ER);
			break;
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void main(void)
{
	OSErr err;
	int resetting;
	
	InitToolbox();
	StartMenus();
	
	/*
	 * This setjmp is used for the non-threaded version of the simulator. The entire
	 * simulator is in a single thread, so when we want to simulate a reset, we just
	 * longjmp to here and restart the sega
	 */
	if (resetting = setjmp(resetJumpBuffer)) {
#ifndef ROMDB
		StopIntroStuff();
#endif
		Bootstrap(resetting);
#ifdef DEBUG
		ResetDebugger();						// Restore TimeRequests
#endif
		HiliteMenu(0);
	} else
		Bootstrap(resetting);					// This inits the sega the first time

	/*
	 * gSimulatorResetRequest is how the sega thread asks the Mac thread to reset.
	 * We need to poll here because the Mac thread needs to be in control when we
	 * want to dispose the sega thread.
	 */
	gSimulatorResetRequest = false;

	while (!gQuitApp) {
		if (gSimulatorResetRequest) {
			Rect		myRect;

			SetRect( &myRect, 0, 0, 320, 224 );
#ifndef ROMDB
			StopIntroStuff();						// Kill Sega thread
#endif
			gSimulatorResetRequest = false;			// ...but only once!
			Bootstrap(1);							// Do a reset
#ifdef DEBUG
			ResetDebugger();						// Kill any Mac->Sega TimeRequests
#endif
#ifndef ROMDB
			DoIntroStuff(false, FrontWindow(), &myRect);	// And restart the Sega
#endif
		}
		EventLoop();
	}

	CleanUp();
}
