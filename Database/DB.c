/*
	File:		DB.c

	Contains:	xxx put contents here xxx

	Written by:	Ted Cohn, Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<38>	 8/29/94	ADS		Josh is a fucker
		<37>	 8/26/94	JBH		Changed CompactPermDatabase to make box unstable for a minimum
									of time. Changed ComputeTypeCheckSum to not destabilize the box
									during checksumming the typenode itself.
		<36>	 8/21/94	JBH		Made missing db constants not spew an "id not in database"
									warning, as the db constant will likely be in the ROM db
									constant table (see BoxSer.c)
		<35>	 8/19/94	JBH		Made db-purge manager a part of db manager, but kept purge
									helper function in DBPurge.c
		<34>	 8/19/94	JBH		Added callouts to DBPurge manager for types that request it, in
									PurgeDB. Currently, only game patches and RAM icons use this
									mechanism.
		<33>	 8/19/94	JBH		Added a debugging option to scramble the database heap on reboot
									by pressing B and C on the controller.
		<32>	 8/17/94	JBH		Added CompactPermDatabase to defragment during boot. Added
									DBAddItemHighPtrSize to allow adding of DB items high in DB
									memory (like game-patches)
		<31>	 8/15/94	ADS		DBGetNextItemID would inf. loop if you had RAM overrides
		<30>	 8/12/94	HEC		Renamed errors.
		<29>	 7/21/94	JBH		Fixed sizes of DB items.
		<28>	 7/15/94	SAH		Added a preflight level for DBPreflight.
		<27>	 7/15/94	dwh		Fix case for unix build
		<26>	 7/14/94	ADS		Vectormania
		<25>	 7/10/94	SAH		Added DBPreflight.
		<24>	  7/7/94	HEC		Moved DBGetConstant and DBAddConstant to BoxSer.c
		<23>	  7/6/94	HEC		Added DBGetConstant, DBAddConstant, and DBAddItemPtrSize.
		<22>	  7/3/94	SAH		Added DBTypeChanged.
		<21>	  7/2/94	SAH		Converted over to SetDBUnstable.
		<20>	  7/1/94	SAH		Only compute checksum for RAM entries.
		<19>	  7/1/94	SAH		Added a purge limit to DBPurge.
		<18>	  7/1/94	SAH		Moved Translate DB to it's own file.
		<17>	 6/30/94	SAH		Added DBTypeFlags with Get and Set calls. Made DBPurge work for
									real.
		<16>	 6/30/94	SAH		Removed database purge values. crc is computed on a per type
									basis. Fixed the DBGetUniqueID bug.
		<15>	 6/28/94	SAH		Updated to new globals.
		<14>	 6/18/94	HEC		Added DBGetUniqueIDInRange.
		<13>	 6/12/94	SAH		Update to new database heap and typelist globals.
		<12>	  6/9/94	HEC		Added crc checking.
		<11>	  6/4/94	KON		Use kDBIDErrorValue instead of -1 as DBID error value.
		<10>	  6/2/94	HEC		Purge all type's purgeable items above specified purgevalue.
		 <9>	  6/1/94	HEC		Add purging in DBAddItem when memory low.
		 <8>	 5/31/94	HEC		Changed bytecopy to SegaBlockMove.
		 <7>	 5/30/94	SAH		Added InitPermDatabase so we can init the perm database at a
									safer time than kHardInit time.
		 <6>	 5/28/94	SAH		Include OSGlobals.h for RAM/ROM/SRAM location stuff.
		 <5>	 5/27/94	HEC		Fixed Rom translation and added size to database structure.
		 <4>	 5/26/94	HEC		Added ROM heap capability.
		 <3>	 5/26/94	DJ		removed a Debugger() statement
		 <2>	 5/25/94	DJ		integrated with new Sega project
		 <5>	 5/25/94	HEC		Managerize and globalize.
		 <4>	 5/25/94	HEC		Now uses database heap and new memory manager. Smaller
									structures too.
		 <3>	 5/19/94	KON		Make DBAddItem delete the old one if the type and ID already
									exist.

	To Do:
*/

#include "OSManagers.h"
#include "Dispatcher.h"
#include "SegaOs.h"
#include "DataBase_priv.h"
#include "heaps.h"
#include "Errors.h"
#include "utils.h"
#include "Reliability.h"
#include "OSGlobals.h"
#include "SegaTraps.h"
#include "ccitt.h"
#include "DBTypes.h"
#include "DBPurge.h"

#include "DebugSelectors.h"

#if STRESS_DBCOMPRESS
#include "SegaIn.h"			/* for kButtonB, for stress testing only */
#include "Controls.h"		/* for control poo, for stress testing only */
#include "StringDB.h"		/* for kAboutBox0, for UI for stress test only */
#include "DialogMgr.h"		/* for kMediumDialog, for UI for stress test only */
#include "DeferredDialogMgr.h"		/* for QDefDialog, to give some U