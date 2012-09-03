/*
	File:		DataBase_priv.h

	Contains:	xxx put contents here xxx

	Written by:	Dave Jevans, Konstantin Othmer, Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<14>	 8/26/94	JBH		Made ComputeTypeCheckSum take a delta, so it can checksum blocks
									whose database pointers are already offset for a pending move
									(see CompactPermDatabase)
		<13>	 8/17/94	JBH		Added AddItemToDB, which can add either high or low in db
									memory.
		<12>	 7/14/94	ADS		VectorMania
		<11>	  7/3/94	SAH		Added private dispatched ComputeTypeCheckSum.
		<10>	 6/30/94	SAH		Added DBTypeFlags.
		 <9>	 6/30/94	SAH		Removed database purge values. Moved the checksum to a per type
									thing. Removed other unused fields.
		 <8>	 6/12/94	SAH		Moved the typelist global to a low mem. Added a dummy member to
									the sega global structure to shut the compiler up. We probably
									want to fix this by only allocating globals on the simulator.
		 <7>	  6/9/94	HEC		Added CRC checking for database heap.
		 <6>	  6/2/94	HEC		Added purgeMap to DBTypeNode
		 <5>	 5/27/94	HEC		Make size a long
		 <4>	 5/27/94	HEC		Added dataSize field to DBListNode
		 <3>	 5/26/94	HEC		Added rom switch global
		 <2>	 5/25/94	DJ		integrated into new Sega project
		 <3>	 5/25/94	HEC		Compress DBListNode
		 <2>	 5/25/94	HEC		reduced size of structs (byte size type and id, e.g.)
		 <1>	 5/17/94	KON		first checked in

	To Do:
*/

#ifndef __DataBase_priv__
#define __DataBase_priv__

#ifndef __DataBase__
#include "DataBase.h"
#endif

#define kNoFlag		0
#define kROMFlag	1		/* Node is ROM */
#define	kRes1Flag	2
#define kRes2Flag	4
#define kRes4Flag	8

#define DBMaxPurgeValue	15

//
// Node
//
typedef struct DBListNode {
	struct DBListNode		*next;
	DBID					id;
	unsigned char			pad;
	unsigned char			data[1];
} DBListNode;

//
// TypeNode
//
typedef struct DBTypeNode {
	struct DBTypeNode		*next;
	DBListNode				*list;
	unsigned short			crc;
	DBType					type;
	DBTypeFlags				flags;
} DBTypeNode;

//
// database
//
typedef struct DBGlobals {
#ifdef SIMULATOR
	short		romswitch;
#else
	short		dummy;
#endif
} DBGlobals;


//
// private routines
//

//
// Private routines that are dispatched
//
unsigned short ComputeTypeCheckSum ( DBTypeNode * tn, long delta ) =
	CallDispatchedFunction( kComputeTypeCheckSum );

long				GetItemSize ( DBListNode * n ) =
	CallDispatchedFunction( kGetItemSize );

DBTypeNode *		DBGetTypeNode( DBType type ) =
	CallDispatchedFunction( kDBGetTypeNode );

DBTypeNode *		DBGetPrevTypeNode( DBType type ) =
	CallDispatchedFunction( kDBGetPrevTypeNode );

DBListNode *		DBTNGetItem( DBTypeNode *tn, DBID id ) =
	CallDispatchedFunction( kDBTNGetItem );

DBListNode *		DBTNGetPrevItem( DBTypeNode *tn, DBID id ) =
	CallDispatchedFunction( kDBTNGetPrevItem );

void				DBTNDisposeList( DBTypeNode *node ) =
	CallDispatchedFunction( kDBTNDisposeList );

DBErr			DeleteItem(DBTypeNode *tn, DBListNode *n, DBListNode *p) =
	CallDispatchedFunction( kDeleteItem );

DBErr 			AddItemToDB( DBType type, DBID theID, void *theData, long dataSize, Boolean high ) =
	CallDispatchedFunction( kAddItemToDB );

#ifdef SIMULATOR
#define MANAGERGLOBALTYPE DBGlobals
#else
extern DBGlobals db;
#endif


#endif __DataBase_priv__
