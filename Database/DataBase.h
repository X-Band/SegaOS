/*
	File:		DataBase.h

	Contains:	xxx put contents here xxx

	Written by:	Dave Jevans, Konstantin Othmer, Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 8/27/94	CMY		Added a constant for RAM DB slop to leave free.
		<25>	 8/19/94	JBH		Added kPurgeCheckPurgeable.
		<24>	 8/17/94	JBH		Added DBAddItemHighPtrSize, CompactPermDatabase.
		<23>	  8/9/94	HEC		Move errors to errors.h
		<22>	 7/18/94	KON		Move DBID type declaration to SegaTypes.h.
		<21>	 7/15/94	SAH		Added a preflight level to DBPreflight.
		<20>	 7/15/94	dwh		unix-ise.
		<19>	 7/10/94	SAH		Added DBPreflight.
		<18>	  7/7/94	HEC		Moved DBGetConstant, DBAddConstant to BoxSer manager.
		<17>	  7/6/94	HEC		Added DBAddItemPtrSize, DBGetConstant, DBAddConstant.
		<16>	  7/3/94	SAH		Fixed up includes some.
		<15>	  7/3/94	SAH		Added DBTypeChanged.
		<14>	  7/1/94	SAH		Added a purge limit to DBPurge.
		<13>	 6/30/94	SAH		Added DBTypeFlags.
		<12>	 6/30/94	SAH		Removed database purge values.
		<11>	 6/18/94	HEC		Added DBGetUniqueIDInRange
		<10>	 6/12/94	SAH		Added DBVerifyDatabase.
		 <9>	  6/4/94	KON		Use kDBIDErrorValue instead of -1.
		 <8>	  6/3/94	SAH		Added kDBReservedItemType.
		 <7>	 5/30/94	SAH		Added InitPermDatabase.
		 <6>	 5/27/94	HEC		Update TranslateROM() proto.
		 <5>	 5/27/94	DJ		Added RESOURCE_DATABASE around DBType (needed if using disk
									version of DB)
		 <4>	 5/27/94	HEC		Added TranslateROM() prototype.
		 <3>	 5/26/94	HEC		Added DBROMSwitch() selector for SIMULATOR
		 <2>	 5/25/94	DJ		integrated into new Sega project
		 <6>	 5/25/94	HEC		Added call dispatch macros for managerization
		 <5>	 5/25/94	HEC		Byte size DBType, DBID, DBPurge and short DBErr.
		 <4>	 5/19/94	KON		Added DBGetItemSize.
		 <3>	 5/17/94	KON		Added error messages to deal with the real (non-Mac resource Mgr
									based) database. For jevans.
		 <2>	 5/12/94	KON		Added DBSetItemPurgeValue which is used by the SendQ.

	To Do:
*/

#ifndef __DataBase__
#define __DataBase__

#ifndef __SegaOS__
#include "SegaOs.h"
#endif


#ifdef RESOURCE_DATABASE
typedef long DBType;				// Mac resource mgr version uses 4-char identifiers, so this is a must.
#else
typedef unsigned char DBType;
#endif RESOURCE_DATABASE

#define kDBIDErrorValue 255

typedef unsigned char DBTypeFlags;
typedef short DBErr;

typedef struct
{
	DBType	theType;
	DBID	theID;
} DBTypeIDPair;

//
// ROM Header
// This is needed so the database manager can know where the
// typenode list begins. This will likely move somewhere else,
// but this is fine for the time being.
//

typedef struct ROMHeader
{
	long	rDBTypeNodeOffset;	// offset from start of rom to first typeNode
} ROMHeader;


//
// kDBReservedItemType is used to flag id 255 which is reserved by the db manager. you cannot
// use this item number.
//
#define	kDBReservedItemType	255


//
//	Preflight levels - currently only light and heavy. light will only see if there's enough room to
//	add the item without deleting an existing one (unless the existing item is bigger or the same size
//	as the new one). heavy will try temporarily deleting an existing item (if same id) if all else fails.
//

enum
{
	kPreflightLight,
	kPreflightHeavy
};

//
//	Slop Factor - For those people who preflight DBAdd's, leave this much slop for everyone else.
//	This is used in Mail and Player List -- UI places where we don't even let the user try to
//	do something if it might not succeed. We can afford to be conservative in such cases.
//
#define kRAMDBSlop 1536L

// Type flags. These are the defines for the 8 flags per type entry in the dbase
//
#define	kPurgeWholeType			0x1		// if purging, purge the whole type (overrides kPurgeCheckPurgeable!)
#define	kPurgeCheckPurgeable	0x2		// if purging, call out on each item to check (to DBPurge.c)
#define	kUndefinedTypeFlag3		0x4
#define	kUndefinedTypeFlag4		0x8
#define	kUndefinedTypeFlag5		0x10
#define	kUndefinedTypeFlag6		0x20
#define	kUndefinedTypeFlag7		0x40
#define	kUndefinedTypeFlag8		0x80

#ifndef	unix

void InitPermDatabase ( void ) =
	CallDispatchedFunction( kInitPermDatabase );
	
/* only call this routine when nobody has pointers into the database!!! Also only call it */
/* at boot, as it makes the DB quite unstable and uses a lot of temp mem! */
void	CompactPermDatabase( void ) =
	CallDispatchedFunction( kCompactPermDatabase );

void *DBGetItem( DBType type, DBID theID ) =
	CallDispatchedFunction( kDBGetItem );

DBErr DBAddItem( DBType type, DBID theID, void *theData ) =
	CallDispatchedFunction( kDBAddItem );

DBErr DBAddItemPtrSize( DBType type, DBID theID, void *theData, long dataSize ) =
	CallDispatchedFunction( kDBAddItemPtrSize );
	
DBErr DBAddItemHighPtrSize( DBType type, DBID theID, void *theData, long dataSize ) =
	CallDispatchedFunction( kDBAddItemHighPtrSize );

DBErr DBDeleteItem( DBType type, DBID theID ) =
	CallDispatchedFunction( kDBDeleteItem );

DBID DBGetUniqueID( DBType type ) =
	CallDispatchedFunction( kDBGetUniqueID );

DBID DBGetUniqueIDInRange( DBType type, DBID low, DBID high ) =
	CallDispatchedFunction( kDBGetUniqueIDInRange );

long DBGetItemSize( DBType type, DBID theID ) =
	CallDispatchedFunction( kDBGetItemSize );

short DBCountItems( DBType type ) =
	CallDispatchedFunction( kDBCountItems );
	
DBID DBGetFirstItemID( DBType type ) =
	CallDispatchedFunction( kDBGetFirstItemID );
	
DBID DBGetNextItemID( DBType type, DBID thePrevID ) =
	CallDispatchedFunction( kDBGetNextItemID );

DBErr DBNewItemType( DBType type, DBTypeFlags flags ) =
	CallDispatchedFunction( kDBNewItemType );

DBErr DBGetTypeFlags( DBType type, DBTypeFlags * flags ) =
	CallDispatchedFunction( kDBGetTypeFlags );

DBErr DBSetTypeFlags( DBType type, DBTypeFlags flags ) =
	CallDispatchedFunction( kDBSetTypeFlags );
	
DBErr DBDeleteItemType( DBType type ) =
	CallDispatchedFunction( kDBDeleteItemType );

DBErr DBVerifyDatabase( short deleteFlag ) =
	CallDispatchedFunction( kDBVerifyDatabase );

DBErr DBPurge( long sizeNeeded, short firstSafeType ) =
	CallDispatchedFunction( kDBPurge );

DBErr DBTypeChanged( DBType type ) =
	CallDispatchedFunction( kDBTypeChanged );

DBErr DBPreflight ( DBType type, DBID id, long dataSize, short level ) =
	CallDispatchedFunction( kDBPreflight );

void TranslateROM(Handle h, long romStart);

#ifdef SIMULATOR
DBErr DBROMSwitch( short enableROM ) =
	CallDispatchedFunction( kDBROMSwitch );
#endif

#endif	/* ! unix */

#endif __DataBase__
