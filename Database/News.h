/*
	File:		News.h

	Contains:	xxx put contents here xxx

	Written by:	Jevans

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<33>	 8/26/94	KON		KillCurNewsPage now returns the current page number.
		<32>	 8/25/94	KON		Add background graphic priority to form data structure.
		<31>	 8/20/94	KON		Background music track is now a sound effect.
		<30>	 8/19/94	KON		Move headline centering to new field which replaces color3 in
									the news page. Removed color3 from body1 and body2 and use those
									for format flags now.
		<29>	 8/13/94	JBH		Added bits to control xband news animation.
		<28>	 8/27/56	JBH		Add flag to control whether XBand News graphics get drawn on top
									of news page.
		<27>	 8/11/94	KON		Add flag to control whether the headline gets centered or not.
		<26>	 8/11/94	JBH		Added SetNewsCountdownTimeConst
		<25>	 8/10/94	KON		Add isBitMapRefAnIcon flag to the color flags.
		<24>	  8/9/94	KON		Added parameter to KillNewsPage to indicate we are leaving news.
									Cache state of right, left, and return controls to eliminate
									flicker.
		<23>	  8/5/94	JBH		Added color flags to news page to allow text to be drawn in the
									default clut (bit set) or the news background graphic clut (bit
									clear).
		<22>	 7/31/94	KON		Add fade parameter to draw news page.
		<21>	 7/28/94	KON		Add the location of the countdown area to the form.
		<20>	 7/28/94	JBH		Added progressBounds so we know where to draw countdown timer &
									sparks.
		<19>	 7/27/94	KON		Change #defines so names don't overlap dispatch table names.
									Yuk!
		<18>	 7/26/94	HEC		Changed kDoDrawNewsPage to kDoDrawNewsPage.
		<17>	 7/26/94	KON		Add draw parameter to ValidateNews().
		<16>	 7/25/94	KON		Made all the strings selected by the flags field.
		<15>	 7/25/94	KON		Change form structure to deal w/ new UI.
		<14>	 7/24/94	KON		Changed news form to move font ID's to the news and add a second
									body of text.
		<13>	 7/16/94	BET		unix-ise include file capitalization.
		<12>	 7/10/94	KON		Added GetNewsGraphicsID.
		<11>	  7/6/94	KON		Added background music sound track in place of animation2.
		<10>	  7/5/94	KON		Added animation fields to news.
		 <9>	 6/30/94	KON		Update news form for left and right arrow positions.
		 <8>	 6/30/94	KON		Add killCurNewsCall.
		 <7>	 6/29/94	KON		Major update.
		 <6>	 6/28/94	KON		Added for stuff for news during server connect.
		 <5>	 6/27/94	KON		Added validate and invalidate news calls.
		 <4>	 6/13/94	SAH		Managerized.
		 <3>	  6/9/94	HEC		Added #include "Database.h"
		 <2>	  6/3/94	DJ		treatly dancing news
		 <1>	  6/2/94	DJ		first checked in

	To Do:
*/


#ifndef __News__
#define __News__

#ifndef __Database__
#include "DataBase.h"
#endif

#ifndef __SegaGraphics__
#include "SegaGraphics.h"
#endif

#ifndef __SegaText__
#include "SegaText.h"
#endif

//
// Server news flags
//
#define kNewDaysNewsFlag	0x01
#define kLastNewsPageMask	0x80

#define kDrawNewsPageFlag 		0
#define kDontDrawNewsPageFlag 	1

typedef struct NewsForm
{
	segaCharRect	headlineCharRect;
	segaCharRect	taglineCharRect;
	short			headlineYpos;
	short			taglineYpos;
	segaCharRect	pageNumCharRect;
	short			pageNumXpos;
	short			pageNumYpos;
	segaCharRect	dateCharRect;
	short			dateYpos;
	segaCharRect	body1CharRect;
	Rect			body1Rect;
	segaCharRect	body2CharRect;
	Rect			body2Rect;
	segaCharRect	countDownCharRect;
	short			rightArrowXpos;
	short			rightArrowYpos;
	short			leftArrowXpos;
	short			leftArrowYpos;
	short			returnIconXpos;
	short			returnIconYpos;
	DBID			backgroundID;
	char			countDownTimerYOffset;
	Rect			progressBounds;
	unsigned char	backgroundGraphicDecompOptions;
} NewsForm;

//
// News is stored in the database as a series of pages.  Each page is a NewsPage
// followed by the actual form.
//

#define	kPageFlag_Headline	1
#define kPageFlag_Tag		2
#define	kPageFlag_Date		4
#define kPageFlag_PageNum	8
#define kPageFlag_Body1		0x10
#define kPageFlag_Body2		0x20
#define	kPageFlag_DisableXBandAnimation		0x40

//Freed one up for you, Josh!

#define	kColorFlag_Headline	1
#define kColorFlag_Tag		2
#define	kColorFlag_Date		4
#define kColorFlag_PageNum	8
#define kColorFlag_Body1	0x10
#define kColorFlag_Body2	0x20
#define	kColorFlag_DisableXBandHeadline	0x40
#define kColorFlag_IsBitMapRefAnIcon	0x80

//
// if the pageFlag has the Headline bit set, then the first string
// of the text is the headline.
// if the pageFlag has the Tag bit set, then the next string is
// for the tag field.
// then the last string is the paragraph for the page.
//

typedef struct NewsPage 
{
	DBID	newsFormID;
	DBID	animation1;
	DBID	backgroundSoundFX;
	DBID	bitmapID;
	FontID	headlineFontID;
	FontID	taglineFontID;
	FontID	dateFontID;
	FontID	pageNumFontID;
	FontID	body1FontID;
	FontID	body2FontID;
	short	bitmapXpos;
	short	bitmapYpos;
	char	pageNum;
	char	pageFlags;
	char	colorFlags;
	char	headlineColor0;
	char	headlineColor1;
	char	headlineColor2;
	char	headlineDrawFlags;
	char	taglineColor0;
	char	taglineColor1;
	char	taglineColor2;
	char	taglineNotUsed;
	char	pageNumColor0;
	char	pageNumColor1;
	char	pageNumColor2;
	char	pageNumNotUsed1;
	char	dateColor0;
	char	dateColor1;
	char	dateColor2;
	char	dateNotUsed;
	char	body1Color0;
	char	body1Color1;
	char	body1Color2;
	char	body1DrawFlags;
	char	body2Color0;
	char	body2Color1;
	char	body2Color2;
	char	body2DrawFlags;
	char	text[1];
} NewsPage;


#ifdef __SERVER__


char *_FindNextNewsString(char *text, long lineNum);

#define	FindNextNewsString(t,l)	_FindNextNewsString(t,l)


#else

char *FindNextNewsString(char *text, long lineNum) = 
	CallDispatchedFunction( kFindNextNewsString );


//
// add news to the news queue

//
// Store each page as a separate DB entry for DBType kNews.
// When the first page comes in we draw it to keep the cheezeball player interested (adman: nudge nudge).
// We don't store pages if not enuf mems avail.
//
// BRILLIANT!
// I can assign all news a common purge level.  and purger blows away everything of that level.
// provide a dispose method for each DBType (so they can remove themselves from lists, etc).
// the method calls dispose
//
// Without that functionality, the mems manager might have deleted *some* of the news pages
// instead of all of them.  And that would make missing news pages (jumbled newspaper).
//

void AddPageToNewsBox(NewsPage *page, DBType whichNews) = 
	CallDispatchedFunction( kAddPageToNewsBox );

NewsPage *GetPageFromNewsBox(char pageNum, DBType whichNews ) = 
	CallDispatchedFunction( kGetPageFromNewsBox );

NewsForm *GetNewsForm( DBID newsFormID ) = 
	CallDispatchedFunction( kGetNewsForm );

short GetNumNewsPages( DBType whichNews ) = 
	CallDispatchedFunction( kGetNumNewsPages );

void EmptyNewsBox( DBType whichNews ) = 
	CallDispatchedFunction( kEmptyNewsBox );

void DrawNewsPage( char pageNum, DBType whichNews, Boolean fade ) = 
	CallDispatchedFunction( kDrawNewsPage );

void ValidateNews( Boolean drawNews ) = 
	CallDispatchedFunction( kValidateNews );

void InvalidateNews( void ) = 
	CallDispatchedFunction( kInvalidateNews );

void SetupNewsForServerConnect( void ) = 
	CallDispatchedFunction( kSetupNewsForServerConnect );

void ServerConnectNewsDone( void ) = 
	CallDispatchedFunction( kServerConnectNewsDone );

short DoNewsControlIdle( void ) = 
	CallDispatchedFunction( kDoNewsControlIdle );

short KillCurNewsPage( Boolean killControls ) = 
	CallDispatchedFunction( kKillCurNewsPage );

DBID GetNewsGraphicsID( char whichNews ) = 
	CallDispatchedFunction( kGetNewsGraphicsID );

void SetNewsCountdownTimeConst( long newTimeConst ) =
	CallDispatchedFunction( kSetNewsCountdownTimeConst );

#endif __SERVER__

#endif __News__
