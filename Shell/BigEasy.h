//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// types
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct	WindowTemplate	{				/*template to a WIND resource*/
	Rect		boundsRect;
	short		procID;
	Boolean		visible;
	Boolean		filler1;
	Boolean		goAwayFlag;
	Boolean		filler2;
	long		refCon;
	Str255		title;
};
typedef	struct WindowTemplate WindowTemplate;
typedef WindowTemplate *WindowTPtr, **WindowTHndl;

// DitlItem used by PutUpMessage

typedef struct DitlItem
	{
	long			placeholder;
	Rect			displayRect;
	unsigned char	type;
	char			title[1];
	} DitlItem;
typedef DitlItem *		DItemPtr;

typedef	struct
	{
	short			count;
	DitlItem		item[1];
	} Ditl ;
typedef Ditl *			DitlPtr;
typedef	DitlPtr *		DitlHndl;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Method Defintions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


typedef void (*IFP)(void);		/* Integer Function Pointer */
typedef RgnHandle (*RFP)(void);	/* Region Function Pointer */

typedef void (*UpdateProc)(WindowGlobalsPtr globals);
typedef void (*ClickProc)(WindowGlobalsPtr globals, Point p, short mods, long when);
typedef void (*GrowProc)(WindowGlobalsPtr globals, Point p, short mods);
typedef void (*ZoomProc)(WindowGlobalsPtr globals, Point p, short partCode);
typedef void (*KeyProc)(WindowGlobalsPtr globals, long k, short mods);
typedef void (*CloseProc)(WindowGlobalsPtr globals);
typedef void (*ActivateProc)(WindowGlobalsPtr globals);
typedef void (*DeactivateProc)(WindowGlobalsPtr globals);
typedef void (*LoopyProc)(WindowGlobalsPtr globals);

/* Default Methods */

void DefaultUpdate(WindowGlobalsPtr globals);
void DefaultClick(WindowGlobalsPtr globals, Point p, short mods, long when);
void DefaultGrow(WindowGlobalsPtr globals, Point p, short mods);
void DefaultZoom(WindowGlobalsPtr globals, Point p, short partCode);
void DefaultKey(WindowGlobalsPtr globals, long k, short mods);
void DefaultClose(WindowGlobalsPtr globals);
void DefaultActivate(WindowGlobalsPtr globals);
void DefaultDeactivate(WindowGlobalsPtr globals);
void DefaultLoopy(WindowGlobalsPtr globals);
void pnull(void);

WindowGlobalsPtr InstallWindow(Rect *iRect, short iType, Boolean iGoway, unsigned char *iTitle,
              UpdateProc iUpdate, ClickProc iClick, GrowProc iGrow, ZoomProc iZoom, KeyProc iKey,
              CloseProc iClose, ActivateProc iActivate, DeactivateProc iDeactive,
              LoopyProc iLoopy, long iGlobalsSize, long refCon, WindowPtr *thisWindowPtr);
WindowGlobalsPtr GetInstallWindow(short windID,
				  UpdateProc iUpdate, ClickProc iClick, GrowProc iGrow, ZoomProc iZoom, KeyProc iKey,
				  CloseProc iClose, ActivateProc iActivate, DeactivateProc iDeactive,
				  LoopyProc iLoopy, long iGlobalsSize, WindowPtr *thisWindowPtr);
void CloseBEWindow( WindowPtr w );

WindowGlobalsPtr InstallDialog(short iResID, ClickProc iClick, CloseProc iClose, long iGlobalsSize, DialogPtr *thisWindowPtr);
void UninstallDialog(DialogPtr uDialog);

void DoCloseWindow(void);
void DoQuit(void);
WindowGlobalsPtr GetWindowGlobals(WindowPtr w);

// Menu stuff

void InstallMenu(Str255 s, IFP action, short ref);
void InstallItem(Str255 s, IFP action, short ref);
void SetMenuItem(short ref, char enable, char isMarked, char mark,  unsigned char *s);
void InstallEditMenu(IFP Xundo,IFP Xcut,IFP Xcopy,IFP Xpaste,IFP Xclear);
void SetMenuSetupProc( IFP menuSetupProcPtr );
void SetAboutProc( IFP aboutProcPtr );
void SetGetMouseRgnProc( RFP mouseRgnProcPtr );
void SetSleepTime( long sleepTime );

#define	mEnable 1
#define	mNoChange 0
#define mDisable -1

// Cheezy routine to display simple messages
short PutUpMessage( short cancel, unsigned char* text );

//
// Utilities
//
void PStrCpy( unsigned char* s, unsigned char* t );
void EnDisEdits(short Eundo,short Ecut,short Ecopy,short Epaste,short Eclear);



// This will display a message in the debugger
#ifdef DEBUG
#define DebugMessage(s)				DebugStr (s)
#else
#define DebugMessage(s)				((void)	0)
#endif


// This will insert debugging code in the application to check conditions
// and displays a message in the debugger if true
#ifdef DEBUG
#define FailMsg(cond)										\
	if (cond) {												\
		DebugStr((ConstStr255Param)"\p"#cond);				\
	}
#else
#define FailMsg(cond)										\
	((void)	0)
#endif

// check for the exception, and if true call the action and then goto handler
#ifdef DEBUG
#define FailIf(cond, handler)								\
	if (cond) {												\
		DebugStr((ConstStr255Param)"\p"#cond);				\
		goto handler;										\
	}
#else
#define FailIf(cond, handler)								\
	if (cond) {												\
		goto handler;										\
	}
#endif


// check for the exception, and if true call the action and then goto handler
#ifdef DEBUG
#define FailWithAction(cond, action, handler)				\
	if (cond) {												\
		DebugStr((ConstStr255Param)"\p"#cond);				\
		{ action; }											\
		goto handler;										\
	}
#else
#define FailWithAction(cond, action, handler)				\
	if (cond) {												\
		{ action; }											\
		goto handler;										\
	}
#endif


// Assert means to assume.  If you assert something, then you are saying
// that you assume the condition to be true else something special happens.
// Asserts in the code are completely removed when you build a non-debug version

// this one allows you to test for the result of a condition (i.e. CloseComponent)
// and break if it returns an error, otherwise it ignores the result.
#ifdef DEBUG
#define AssertErr(cond)			if (cond) DebugStr("\pAssertErr")
#else
#define AssertErr(cond)			(cond)
#endif

#ifdef DEBUG
#define AssertMsg(cond)			FailMsg(!(cond))
#define Assert(cond, handler)	FailIf(!(cond), handler)
#define AssertWithAction(cond, action, handler)		FailWithAction(!(cond), action, handler)
#else
#define AssertMsg(cond)			((void)0)
#define Assert(cond, handler)	((void)0)
#define AssertWithAction(cond, action, handler)		((void)0)
#endif
