/*
	File:		News.c

	Contains:	xxx put contents here xxx

	Written by:	Jevans

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<96>	 9/15/94	KON		Fixed ValidateNews to set the number of pages in the global so
									that DrawNewsPage doesn't get called twice during server
									connects when there is no new news to download. Calling
									DrawNewsPage twice caused two sets of sparks to appear, and one
									set of three stayed forever.
		<95>	 8/28/94	JOE		Trigger the kXBandNewsFlippinASnd for the flippin A
		<94>	 8/28/94	JBH		Changed the sounds played in XBandNews fly-in so the sprite's
									sound is played at END of it's motion. This lets the X get a
									noise of its own.
		<93>	 8/27/94	KON		Handle cases where there is no news. Who ever said no news is
									good news?
		<92>	 8/27/94	CMY		Fix the centering of the progress message.
		<91>	 8/27/94	KON		If there are no kDailyNews pages, don't draw the return icon.
		<90>	 8/26/94	CMY		Timer proc now puts up a message when the count goes below 0.
		<89>	 8/26/94	KON		KillCurNewsPage now returns the current page number.
		<88>	 8/25/94	JBH		Sped up rotating A in XBand News. Draw XBand News before text,
									not after.
		<87>	 8/25/94	KON		Fix sprite flickering problem in XBAND news, and make news more
									robust when there are no news pages available.
		<86>	 8/25/94	KON		Add background graphic priority to form data structure.
		<85>	 8/21/94	JBH		Make the countdown timer display a string when it goes below
									zero.
		<84>	 8/21/94	JBH		EraseGDevice( kScrollA ) in drawnewspage to avoid
									annoying uninitialized pattern mems from showning thru.
									CreateSpriteInFront now takes a hueShift (0 in news)
		<82>	 8/21/94	KON		DrawGraphic now takes a hueShift. If the newspaper bitmap is an
									icon, we use the animationID to indicate the hueShift.
		<81>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<80>	 8/20/94	JBH		Fixed clut owners for news text that uses background graphics'
									clut. Fixed screen erasing between news pages w/ different
									backdrop ID's. Temporarily dispose Kon's button caches when
									LinearizeScreenArea fails due to pattern mems fragmentation.
		<79>	 8/20/94	KON		Background music track is now a sound effect.
		<78>	 8/19/94	KON		Move headline centering to new field which replaces color3 in
									the news page. Removed color3 from body1 and body2 and use those
									for format flags now.
		<77>	 8/15/94	KON		If the headlne char rect is nil and the headline flag is set,
									skip the headline. Only disable the left arrow if we are on the
									first page of news.
		<76>	 8/15/94	KON		Deconservatise pixel bounds calculation for headline textbox
									rectangle.
		<75>	 8/15/94	JBH		Added flipping A in XBand News animation
		<74>	 8/15/94	JBH		Avoid redraw when changing between different forms, but not
									different background graphic ID
		<73>	 8/15/94	KON		Headline uses headline char rect (rather than body1rect) to
									figure boxtext bounds.
		<72>	 8/15/94	KON		Make headline draw as box text so it can become the third news
									body if needed.
		<71>	 8/14/94	JBH		Added sounds to XBand News fly-ib
		<70>	 8/13/94	JBH		Added XBand News letter flyin for xband news. Fixed bug where
									news has multiple backdrop IDs and we were leaking aux graphics.
		<69>	 8/12/94	JOE		fix kludge, put back in Kon's GetIconBitMap() call
		<68>	 8/12/94	JOE		kludge for new icons
		<67>	 8/11/94	KON		Look at flag to determine whether or not headline is centered.
		<66>	 8/11/94	JBH		Made news countdown timer use a news global to track when server
									resets timeBase. Added routine to reset it.
		<65>	 8/11/94	JBH		Kill pattern memory at teardownnewssparks so the time number
									gets erased when the server is done.
		<64>	 8/10/94	KON		Make page arrows and return icon sprites, make high byte of
									color flags determine whether bitmap is an icon or a graphic.
		<63>	  8/9/94	KON		Clearbackdrop when news gets killed so we don't leave treats
									laying around for non-aux background guys.
		<62>	  8/9/94	KON		Roll Josh's changes from 60 back in. Projector is great!
		<61>	  8/9/94	KON		Cache control positions to eliminate flicker.
		<59>	  8/9/94	KON		Rolled background ID and aux background graphic into one call.
		<58>	  8/8/94	KON		Erase scroll A on KillNewsPage to remove flicker. Remove extra
									call that sets cur device to scrollB.
		<57>	  8/8/94	KON		Added aux background support.
		<56>	  8/5/94	JBH		Draw news text in clut of the news background graphic if news
									page tells us to do so (in colorFlags)
		<55>	  8/4/94	JBH		Fixed bugs with countdown timer.
		<54>	  8/4/94	JBH		De-seeded RandomShort calls.
		<53>	 7/31/94	KON		Uncomment the commented out countdown proc update because now
									the counter is always showing zero after a page turn.
		<52>	 7/31/94	KON		Make news countdown time not flash the start value on every new
									page.
		<51>	 7/31/94	KON		Make first page of news fade in.
		<50>	 7/30/94	KON		Make news a type2 background + a news graphic. Draw both inFront
									of sprites.
		<49>	 7/30/94	KON		Remove bizarre time reference assert as the server now sets the
									countdown rate.
		<48>	 7/29/94	KON		Move everything to the front so that animations can come up from
									behind the newspaper.
		<47>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		<46>	 7/28/94	KON		Allocate the mems for the countdown timer ref here rather than
									in the progress proc. This fixes the bug where dialogs where
									trashing parts of the screen because the progress proc was
									killed and the patterm mem was still around.
		<45>	 7/28/94	JBH		Added news countdown timer so player can tell how much longer
									the server connect will last.
		<44>	 7/27/94	KON		Fix server connect news problems related to drawing the return
									icon and validating news.
		<43>	 7/26/94	HEC		kDrawNewsPage was explicitly set to 0, causing DrawNewsPage() to
									be installed in the wrong dispatcher table entry!  Changed
									kDrawNewsPage to kDoDrawNewsPage...
		<42>	 7/26/94	DJ		fixing AddPageToNewsBox
		<41>	 7/26/94	KON		Add draw parameter to ValidateNews().
		<40>	 7/26/94	KON		When adding a page to the news box, clear the last page flag.
		<39>	 7/25/94	DJ		moved pagenum and date to end of list of strings.  this helps
									server out.
		<38>	 7/25/94	KON		Made all text fields selected by flags field.
		<37>	 7/25/94	KON		Add page control graphics and change API's to reflect new model.
									Someday we should fix news so it's all controlled from one
									place, rather than NewsScreen and FindOpponent. Nasty. Yeah.
		<36>	 7/25/94	KON		Add return icon and rip out old new page animation stuff.
		<35>	 7/24/94	KON		Changed news form to move font ID's to the news and add a second
									body of text.
		<34>	 7/21/94	JOE		Added flags param to SegaBoxText()
		<33>	 7/21/94	KON		Support new date format.
		<32>	 7/13/94	ADS		Vectorize EVERYTHING
		<31>	 7/10/94	KON		Added GetNewsGraphicsID.
		<30>	 7/10/94	JOE		play FX indirectly
		<29>	  7/9/94	KON		Fix page number problem when two newspapers are installed.
		<28>	  7/6/94	KON		Add background sound track in place of animation2.
		<27>	  7/5/94	KON		Added animation fields to news.
		<26>	  7/4/94	KON		Make validate news deal better with the case where the news
									doesn't get replaced.
		<25>	 6/30/94	KON		Add left and right page arrows.
		<24>	 6/30/94	SAH		Removed database purge values.
		<23>	 6/30/94	KON		Make news pages one based everywhere.
		<22>	 6/30/94	KON		Fixed mem leaks.
		<21>	 6/30/94	KON		Make page counts 1 based, add killNews call.
		<20>	 6/29/94	KON		Major restructuring.
		<19>	 6/28/94	CMY		Changed sprite calls to remove palette param.
		<18>	 6/28/94	KON		Added more stuff to handle news during server connect.
		<17>	 6/27/94	KON		Add validate and invalidate news calls.
		<16>	 6/21/94	SAH		Fixed fucked up patterns after news screen leaves.
		<15>	 6/20/94	KON		Make connect sequence time based, and wait for button press on
									news.
		<14>	 6/16/94	BET		Revert Ted's changes.
		<13>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		<12>	 6/15/94	HEC		SegaOSIdle now takes a param
		<11>	 6/13/94	SAH		Managerized.
		<10>	 6/12/94	DJ		removed WaitForButtonPress
		 <9>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		 <8>	  6/6/94	DJ		added new cheapo animproc
		 <7>	  6/5/94	DJ		tweaks of icon (draw after text)
		 <6>	  6/5/94	DJ		turned animation off
		 <5>	  6/5/94	DJ		turned animation back on
		 <4>	  6/3/94	DJ		added DisposeAnimation to stop the wigging
		 <3>	  6/3/94	DJ		updating to revised DrawDBGraphic call
		 <2>	  6/3/94	DJ		dancing news
		 <1>	  6/2/94	DJ		first checked in

	To Do:
*/


#include "SegaOS.h"
#include "SegaBM.h"
#include "SegaVDP.h"
#include "vdp.h"
#include "sega.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "UsrConfg.h"
#include "DataBase.h"
#include "News.h"
#include "NewsUtilsPriv.h"
#include "Errors.h"
#include "DBTypes.h"
#include "Heaps.h"
#include "Reliability.h"
#include "SendQ.h"
#include "GraphicsDB.h"
#include "TextUtls.h"
#include "Paths.h"
#include "PathProcs.h"
#include "Animation.h"
#include "Sprites.h"
#include "Time.h"
#include "Events.h"
#include "Backdrops.h"
#include "Animators.h"
#include "SegaSound.h"
#include "SegaIn.h"
#include "StringDB.h"
#include "MailView.h"
#include "Utils.h"
#include "Progress.h"
#include "DBConstants.h"
#include "BoxSer.h"
#include "Challnge.h"
#include "SegaCluts.h"

void 		_AddPageToNewsBox(NewsPage *page, DBType whichNews );
NewsPage 	*_GetPageFromNewsBox( char pageNum, DBType whichNews );
NewsForm 	*_GetNewsForm( DBID newsFormID );
short		_GetNumNewsPages( DBType whichNews );
void		_EmptyNewsBox( DBType whichNews );

void _DrawNewsPage( char pageNum, DBType whichNews, Boolean fade );
void _ValidateNews( Boolean drawNews );
void _InvalidateNews(void);

void _SetupNewsForServerConnect( void );
void _ServerConnectNewsDone( void );
short _DoNewsControlIdle( void );
short _KillCurNewsPage( Boolean killControls );
DBID _GetNewsGraphicsID( char whichNews );
void _SetNewsCountdownTimeConst( long newTimeConst );

//
// internal routines
//
void _ShowLeftRightPageControls( short flags );
void _DrawNewsReturnIcon( void );
void _DrawXBandNews( Boolean shouldAnimate );
void _DisposeXBandNews( void );

#ifndef SIMULATOR
NewsGlobals news;
#endif	


long
_NewsControl ( short command, long data )
{
long offset;
short error;
NewsGlobals	* globals;
short iii;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:

			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET( news, offset );
			
			error = AllocateGlobalSpace ( kNewsManager, offset,
				sizeof(NewsGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create news globals" );
			
			globals->generalNewsRead = 0;			//mark general news unread
			globals->catapultNewsRead = 0;			//mark catapult news unread

			// install our selectors

			SetDispatchedFunction ( kFindNextNewsString,	kNewsManager,	_FindNextNewsString );
			SetDispatchedFunction ( kAddPageToNewsBox,		kNewsManager,	_AddPageToNewsBox );
			SetDispatchedFunction ( kGetNewsForm,			kNewsManager,	_GetNewsForm );
			SetDispatchedFunction ( kGetPageFromNewsBox,	kNewsManager,	_GetPageFromNewsBox );
			SetDispatchedFunction ( kGetNumNewsPages,		kNewsManager,	_GetNumNewsPages );
			SetDispatchedFunction ( kEmptyNewsBox,			kNewsManager,	_EmptyNewsBox );
			SetDispatchedFunction ( kDrawNewsPage,			kNewsManager,	_DrawNewsPage );
			SetDispatchedFunction ( kValidateNews,			kNewsManager,	_ValidateNews );
			SetDispatchedFunction ( kInvalidateNews,		kNewsManager,	_InvalidateNews );
			SetDispatchedFunction ( kSetupNewsForServerConnect, kNewsManager,	_SetupNewsForServerConnect );
			SetDispatchedFunction ( kServerConnectNewsDone,	kNewsManager,	_ServerConnectNewsDone );
			SetDispatchedFunction ( kDoNewsControlIdle,		kNewsManager,	_DoNewsControlIdle );
			SetDispatchedFunction ( kKillCurNewsPage,		kNewsManager,	_KillCurNewsPage );
			SetDispatchedFun