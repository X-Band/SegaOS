/*
	File:		CreateDatabases.c

	Contains:	Database init code

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

	   <147>	 8/29/94	ADS		Don't add bad patches to real ROMS
	   <146>	 8/28/94	SAH		Added kDefaultSyncOTronDelayConst.
	   <145>	 8/28/94	HEC		Added kAnswerDelayConst
	   <144>	 8/27/94	SAH		Added the Stinko timeout DBConstant.
	   <143>	 8/27/94	SAH		Don't put the test game patch in the sega ROM.
	   <142>	 8/27/94	HEC		Install game patch flags const.
	   <141>	 8/26/94	HEC		Install default fredstatus register const.
	   <140>	 8/25/94	JOE		keyboard id const now in keymap
	   <139>	 8/25/94	JOE		added keyboard id const
	   <138>	 8/24/94	HEC		added dtmf consts.
	   <137>	 8/24/94	BET		Add kChatDebugFifoSize.
	   <136>	 8/21/94	HEC		Fixed fuckup.
	   <135>	 8/21/94	HEC		Added call waiting timeout consts.
	   <134>	 8/21/94	JBH		Moved ROM DB constants into a single table, saving about 1K of
									ROM and time on the lookup.
	   <133>	 8/20/94	HEC		Install kAnswerTimeoutConst, kAnswerToneTimeoutConst, and
									kAnswerCarrierTimeoutConst.
	   <132>	 8/19/94	JBH		Changed purgability of ram icons, game patches, and daily news.
	   <131>	 8/18/94	ADS		Hide gBoxID globals
	   <130>	 8/18/94	CMY		Add chortle procs and chortle secrets to ROM DB.
	   <129>	 8/17/94	CMY		Setup chortle constants.
	   <128>	 8/15/94	HEC		Added kListenCallWaitingTime
	   <127>	 8/13/94	HEC		Added kGPM896SecretCodeValueConst.
	   <126>	 8/12/94	JBH		Add kXBandNewsDataType
	   <125>	 8/12/94	JOE		nuke IconRef stuff, init RAIC (RAM Icon) stuff
	   <124>	 8/10/94	BET		Added scriptID brokering vi phoneNumber structure.
	   <123>	  8/8/94	JOE		Init Codelets
	   <122>	  8/7/94	JOE		Added init for kSoundEffectRefString
	   <121>	  8/6/94	JBH		Added DBConstant for server-connect messages read.
	   <120>	  8/6/94	HEC		Added kMaxLostPeerConnectionsConst.
	   <119>	  8/5/94	JOE		Setup kJizzlerOptionsConst
	   <118>	  8/4/94	JOE		Install kMathDataType
	   <117>	  8/4/94	HEC		Added kMaxLostPeerConnections. Removed peer tlisten timeout
									const.
	   <116>	  8/3/94	HEC		Added kTriesBeforeDisableCW.
	   <115>	  8/1/94	CMY		Don't try to compress DIC1 (Intro Animation) because it's not
									there.
	   <114>	  8/1/94	HEC		Adding more DBConstants for PeerConnect and Chat
	   <113>	 7/30/94	CMY		Split buttons into separate dictionaries to lessen our temp mems
									requirements.
	   <112>	 7/30/94	KON		Create install screen layout type to get rid of warning message.
	   <111>	 7/29/94	CMY		Add screen layout resources to ROM build.
	   <110>	 7/29/94	KON		Add sample news and mail to the ROM build.
	   <109>	 7/28/94	KON		Add some mail to ROM db build for the marketing spuds.
	   <108>	 7/28/94	JBH		Added constant for news countdown timer.
	   <107>	 7/27/94	SAH		Moved all the test user crap to BoxSer for the Box ID init.
	   <106>	 7/27/94	JOE		fuck kcuf
	   <105>	 7/27/94	JOE		SetCurUserID() *after* setting up the test users & box hometown
									(fucker!)
	   <104>	 7/26/94	SAH		Don't add test game patch to the ROMDB.
	   <103>	 7/26/94	CMY		Add button animator db types.
	   <102>	 7/26/94	JOE		Temporarily turn ranking installation back on
	   <101>	 7/26/94	JBH		Added mazeDataType to database
	   <100>	 7/25/94	JBH		Added a ROM dumper, ifdef-ed out.
		<99>	 7/25/94	JOE		Set Box Hometown to something real to make the Psuq build happy.
		<98>	 7/25/94	SAH		Make sure the test game patch has a unique id.
		<97>	 7/24/94	KON		Update the res file so crashes don't kill us.
		<96>	 7/24/94	KON		Add code that Ted deleted in <93>.  Thanks yet again.
		<95>	 7/24/94	KON		Add compressed bitmap caching.
		<94>	 7/23/94	SAH		Added some gametalk constants.
		<93>	 7/22/94	HEC		Added kGameCallWaitingTimeoutConst for period durng game play
									when call waiting occurs, but before we quit waiting for
									opponent to call back.
		<92>	 7/22/94	BET		Added kConnectSequenceType for server connect scripts.
		<91>	 7/22/94	JOE		Add animation sound effect stuff
		<90>	 7/21/94	KON		Add name table compression to dictionary compressor. If def'd
									out until decompressor is written.
		<89>	 7/21/94	CMY		Add keyboard graphics as the type 8 dictionary.
		<88>	 7/20/94	CMY		There are now 7 graphics dictionaries. Show compression progress
									at start.
		<87>	 7/19/94	CMY		Install DIC4 and DIC5 resources.
		<86>	 7/19/94	CMY		Modified InstallAndCompressGraphics to install graphics into any
									DB type.
		<85>	 7/19/94	JOE		kGameNameType is kaput
		<84>	 7/18/94	CMY		Add and RLE compress the JIZL resource for the jizzling text
									boxes of doom.
		<83>	 7/18/94	DJ		not loading in rankings
		<82>	 7/18/94	HEC		New kXBANDOptions1Const added to constants db.
		<81>	 7/18/94	HEC		Added kXBANDOptions1 to constants db.
		<81>	 7/18/94	CMY		Fixed a bug in the code that adds local nametable references to
									dictionary compressed bitmaps.
		<80>	 7/17/94	KON		Tag each graphic with its dictionary.
		<79>	 7/17/94	KON		Do SEGA resources same as it ever was, and build dictionaries
									out of DIC1, DIC2, and DIC3 resources.
		<78>	 7/17/94	ADS		Added about-strings
		<77>	 7/17/94	JBH		Removed thank-you strings, fixed a warning wrt radio button data
									type.
		<76>	 7/17/94	KON		Add color table dictionaries to the database.
		<75>	 7/16/94	KON		Add run length compression to bitmap name tables.
		<74>	 7/16/94	HEC		Add constants for call waiting.
		<73>	 7/16/94	KON		Remove debugStr and added unique cells to dictionary bitmap.
		<72>	 7/16/94	KON		Add dictionary compression.
		<71>	 7/16/94	JBH		Add thank-you string to database.
		<70>	 7/15/94	JBH		Add CLUT animation data to db.
		<69>	 7/14/94	CMY		Added XBand setup screen
		<68>	 7/13/94	JOE		pre/post game DB type initialization
		<67>	 7/12/94	SAH		Fix test patch fuckup for dave-o.
		<66>	 7/12/94	HEC		Added installation of new pmodem constants for call waiting.
		<65>	 7/11/94	SAH		Install the test game patch into the ROM. Don't set the box
									phone number here anymore.
		<64>	 7/10/94	KON		Add vector path data to DB.
		<63>	 7/10/94	JOE		InstallResourceList() SSNDs and SBGMs
		<62>	 7/10/94	JOE		added inits for BGM refs and FX refs
		<61>	  7/8/94	JBH		Added progress procs
		<60>	  7/8/94	ADS		Add const's for connect-disable-password
		<59>	  7/7/94	KON		Add server and peer-to-peer timeouts and retry constants to the
									DBConstants database.
		<58>	  7/7/94	JOE		don't DBNewItemType twice
		<57>	  7/7/94	JOE		setup for opponent personification & writeable strings
		<56>	  7/6/94	BET		Add transport timeout constants
		<55>	  7/6/94	KON		Add constant for net register time out.
		<54>	  7/6/94	BET		Add kDBFEOffListenThresholdConst and
									kDBFEOffOriginateThresholdConst
		<53>	  7/6/94	HEC		Added DBAddConstant calls to  InitRomDatabase() to exercise new
									kConstantsDBType.
		<52>	 8/28/56	SAH		Chnaged SetBasePhoneNumber to SetBoxPhoneNumber.
		<51>	  7/5/94	KON		Kill some warnings.
		<50>	  7/5/94	JOE		personification update
		<49>	  7/5/94	ADS		Added secret keys
		<48>	  7/4/94	SAH		Added the test game patch for the stimulator.
		<47>	  7/3/94	SAH		dded kAnimation.
		<46>	  7/3/94	JOE		DBNewItemType() & InstallResourceList() 'IDSC', 'ISEG'
		<45>	  7/2/94	KON		Add some sample news.
		<44>	  7/2/94	JBH		Added dialog dismissal data type.
		<43>	  7/2/94	JOE		DBNewItemType() & InstallResourceList() 'IREF'
		<42>	  7/2/94	CMY		Added keyboard layouts
		<41>	  7/1/94	JOE		init RANKing shit
		<40>	  7/1/94	KON		Add CATAPULT to the ROM for all four players.
		<39>	  7/1/94	KON		Move news to RAM and make it for simulator only.
		<38>	 6/30/94	SAH		Added flags for DBNewItemType. The 800 number is no longer a
									resource.
		<37>	 6/30/94	SAH		Removed database purge values.
		<36>	 6/29/94	KON		update for latest news stuff.
		<35>	 6/28/94	SAH		Killed Results.h
		<34>	 6/28/94	SAH		Updated to new box serial id and local user stuff.
		<33>	 6/21/94	SAH		Add address book entries to ROMDB for CES.
		<32>	 6/20/94	SGR		added LEDAnimations
		<31>	 6/19/94	KON		Make TexT items add by resID rather than sequentially.
		<30>	 6/18/94	GOD		Fixed a bogus perm memory allocation.
		<29>	 6/18/94	KON		Install NGP list.
		<28>	 6/16/94	KON		Move test addresses into RAM database rather than ROM database.
		<27>	 6/15/94	DJ		not setting boxSerialNumber in here anymore.  it is done in
									BoxSer.c
		<26>	 6/14/94	CMY		Added dialog types.
		<25>	 6/14/94	DJ		800 number in the ROM db
		<24>	 6/10/94	KON		Add Screen State Table.
		<23>	  6/6/94	DJ		Date macro needs 1994L
		<22>	  6/6/94	DJ		LoadSomeMail loads demo mails into 2 inboxes
		<21>	  6/5/94	CMY		Added credits and restrictions entries to database.
		<20>	  6/4/94	SAH		Only include InstallResourceList for the simulator.
		<19>	  6/4/94	HEC		Added InstallResourceList() to allow loading of sparse resource
									ID's.
		<18>	  6/4/94	KON		Add personification.
		<17>	  6/3/94	SAH		Moved the address book stuff to the rom database so we can see
									it on the sega. Fixed the game patch database id.
		<16>	  6/2/94	KON		Added address book entries to RAM database rather than ROM so
									now they can be deleted.
		<15>	  6/1/94	KON		Add address book for user 0 and init from resource.
		<13>	 5/31/94	SAH		Killed SegaMisc.h. Only compile InitRomDatabse for the
									simulator.
		<12>	 5/31/94	DJ		added BOXSERIALNUMBER #define
		<11>	 5/31/94	SAH		Consolidated cursors. Added game patch type.
		<10>	 5/31/94	KON		Add control strings, test strings, and test user IDs.
		 <9>	 5/30/94	SAH		Add sounds to rom database.
		 <8>	 5/30/94	CMY		Added new cursors for keyboard input
		 <7>	 5/30/94	KON		Added coordinates (for no particular reason) and layouts.
		 <6>	 5/30/94	SAH		Add some cluts.
		 <5>	 5/29/94	DJ		changed name of mail db typedef
		 <4>	 5/28/94	KON		Add some outgoing mail and change date from a string to a long.
		 <3>	 5/28/94	KON		Add mail and text strings.
		 <2>	 5/27/94	SAH		Oops. Fixed some checkin integration problems.
		 <1>	 5/27/94	SAH		first checked in

	To Do:
*/

#include "SegaOS.h"
#include "Decompress.h"
#include "Paths.h"
#include "Animation.h"
#include "Mail.h"
#include "Reliability.h"
#include "Heaps.h"
#include "DBTypes.h"
#include "DITLMgr.h"
#include "UsrConfg.h"
#include "Challnge.h"
#include "GameDB.h"
#include "GameID.h"
#include "BoxSer.h"
#include "SegaText.h"
#include "Errors.h"
#include "StringDB.h"
#include "SegaSound.h"
#include "PatchDB.h"
#include "Screens.h"
#include "DBConstants.h"
#include "Progress.h"
#include "Decompressors.h"
#include "CompressDictionary.h"
#include "TextButt.h"
#include "Secrets.h"

#ifdef SIMULATOR
	#include <StdIO.h>
#endif


/*
*	This file contains all the specific init code for the rom and perm database. Be careful with
*	what you add to the ROM database. The perm database init function is intended to add test data
*	only for the simulator. if there is anything you need to add to the perm SRAM database on
*	initial sega boot, add it to the end of BuildDataHeap in SegaOS.c
*/

void	InitROMDatabase ( void );
void	InitTestRamDatabase ( void );
Ptr		GetGamePatchPtr ( void );
static short InstallResourceList( OSType restype, short dbtype );
static short InstallAndCompressDictionaryGraphics( OSType restype, DBID dictionaryID, DBType resultType );
static void AddTextBoxJizzlers( void );

#ifdef SIMULATOR

static void DumpDBContents(void);

static long *BeginROMDBConstants( void );

static void AddROMDBConstant( DBID constID, long data, long *table );

static void EndROMDBConstants( long *table );

long *BeginROMDBConstants( void )
{
long *table;
short iii;

	table = (long *) NewMemory( kTemp, sizeof( long ) * kROMDBConstantsCount );
	
	for (iii=0;iii<kROMDBConstantsCount;iii++)
		table[iii] = kROMDBConstantsSentinel;
		
	return (table);
}

void AddROMDBConstant( DBID constID, long data, long *table )
{
	ASSERT(data != kROMDBConstantsSentinel);		/* If this fires, we need a new sentinel value! */
	ASSERT(constID < kROMDBConstantsCount);			/* If this fires, we need a bigger table! */

	table[ constID ] = data;
}

void EndROMDBConstants( long *table )
{
	DBAddItem( kROMDBConstantsType, 0, table );
	
	DisposeMemory( (Ptr) table );
}

/*
*	Everything in here is added to the ROM database. be very careful!
*	This is called by BuildOSHeap in SegaOS.c for the SIMULATOR only.
*/
void
InitROMDatabase ( void )
{
Handle				h;
Ptr					p;
short				ii;
short				theCount;
short				theSize;
phoneNumber			*serverPhoneNumber, *temp;
short			rID;
OSType			rType;
Str255 			rName;
long			*constantsRef;

	/* PREALLOCATE RESERVED MEMORY IN THE DATABASE HEAP FOR GAME PATCHES HERE */
	
	
	// TURN ON THE ROM
	DBROMSwitch(true);

	DBNewItemType( kConstantsDBType, 0 );
	DBNewItemType( kNGPType, 0 );
	DBNewItemType( kGamePatchType, kPurgeCheckPurgeable );
	DBNewItemType( kDecompressorType, 0 );
	DBNewItemType( kSendQType, 0 );
	DBNewItemType( kPathType, 0 );
	DBNewItemType( kCursorType, 0 );
	DBNewItemType( kBitMapType, 0 );
	DBNewItemType( kFontType, 0 );
	DBNewItemType( kDITLProcType, 0 );
	DBNewItemType( kAnimatorType, 0 );
	DBNewItemType( kSessionType, 0 );
	DBNewItemType( kStreamType, 0 );
	DBNewItemType( kMailInBoxType0, 0 );
	DBNewItemType( kMailInBoxType1, 0 );
	DBNewItemType( kMailInBoxType2, 0 );
	DBNewItemType( kMailInBoxType3, 0 );
	DBNewItemType( kStringType, 0 );
	DBNewItemType( kClutType, 0 );
	DBNewItemType( kObjectDesc, 0 );
	DBNewItemType( kScreenLayout, 0 );
	DBNewItemType( kCoordinateType, 0 );
	DBNewItemType( kSoundFXType, 0 );
	DBNewItemType( kSoundBGMType, 0 );
	DBNewItemType( kControlLayout, 0 );
	DBNewItemType( kTestUserID, 0 );
	DBNewItemType( kScreenLEDAnimation, 0 );
	DBNewItemType( kDialogDismissalType, 0 );
	DBNewItemType( kAnimation, 0 );
	DBNewItemType( kSecretListType, 0 );
	DBNewItemType( kSecretSequenceType, 0 );
	DBNewItemType( kScreenGetDispatchType, 0 );
	DBNewItemType( kDeferredDialogType, 0 );
	DBNewItemType( kVectorType, 0 );
	DBNewItemType( kCLUTDataType, 0 );
	DBNewItemType( kRadioButtonLayout, 0 );
	DBNewItemType( kAboutString, 0 );

//
// Maybe these should always just live in RAM
//
	DBNewItemType( kAddressBookType0, 0 );
	DBNewItemType( kAddressBookType1, 0 );
	DBNewItemType( kAddressBookType2, 0 );
	DBNewItemType( kAddressBookType3, 0 );
	
//
// Personifications for all users
//
	DBNewItemType( kPersonificationType, 0 );
	DBNewItemType( kUserRankings, 0 );
	
	DBNewItemType( kCreditsDBType, 0 );
	DBNewItemType( kRestrictionsDBType, 0 );
	DBNewItemType( kScreenStateTableType, 0 );
	DBNewItemType( kDialogTemplateType, 0 );
	DBNewItemType( kDialogType, 0 );
	DBNewItemType( kNewsForm, 0 );
	DBNewItemType( kDailyNews, 0 );
	DBNewItemType( kOtherNews, kPurgeWholeType );
	DBNewItemType( kKeyboardEntryLayout, 0 );
	DBNewItemType( kIconDescType, 0 );
	DBNewItemType( kIconBitMapType, 0 );
	DBNewItemType( kWriteableString, 0 );
	DBNewItemType( kProgressType, 0 );
	DBNewItemType( kBackgroundMusicRef, 0 );
	DBNewItemType( kSoundEffectRef, 0 );
	DBNewItemType( kTauntGameOverLayout, 0 );
	DBNewItemType( kPatternDictionaryType, 0 );
	DBNewItemType( kTextBoxJizzlerType, 0 );
	DBNewItemType( kButtonGraphicType, 0 );
	DBNewItemType( kButtonSequenceType, 0 );
	DBNewItemType( kButtonSequenceFXType, 0 );
	DBNewItemType( kConnectSequenceType, 0 );
	DBNewItemType( kMazeDataType, 0 );
	DBNewItemType( kButtonAnimationType, 0 );
	DBNewItemType( kScreenLayoutType, 0 );
	DBNewItemType( kMathDataType, 0 );
	DBNewItemType( kSoundEffectRefString, 0 );
	DBNewItemType( kCodeletsType, 0 );
	DBNewItemType( kPhoneNumberType, 0 );
	DBNewItemType( kRAMIconType, kPurgeCheckPurgeable );
	DBNewItemType( kXBandNewsDataType, 0 );
	DBNewItemType( kSecretProcType, 0 );
	DBNewItemType( kROMDBConstantsType, 0 );
	
	// Add Credits and Restrictions for Options screen
	InstallResourceList('CRED', kCreditsDBType);
	InstallResourceList('REST', kRestrictionsDBType);
	
	// add personifications
	InstallResourceList('PERS', kPersonificationType);

	// install rankings
	InstallResourceList('RANK', kUserRankings);

	// add default icon cursors
	InstallResourceList('SCUR', kCursorType);

	// add bitmaps
	InstallResourceList('SEGA', kBitMapType);

	//add graphics that get dictionary compressed
	InstallAndCompressDictionaryGraphics( 'DIC2', 2, kBitMapType );
	InstallAndCo