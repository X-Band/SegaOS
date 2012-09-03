/*
	File:		AddressBook.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<10>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<10>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		 <9>	 7/29/94	JOE		made clut cycle faster 'cuz it's purtier
		 <8>	 7/29/94	JOE		added constant for clut cycling rate
		 <7>	 7/19/94	KON		Twiddle positions.
		 <6>	 6/20/94	KON		Set MaxAddressBookEntries to 9
		 <5>	 6/20/94	KON		Twiggle the jigger for CES look.
		 <4>	 6/10/94	KON		Update to use screen dispatcher.
		 <3>	  6/5/94	KON		Reposition stuff for new background screens. These positions
									should probably be moved to the database...
		 <2>	  6/2/94	KON		Positions moved to here from AddressCntl.h.
		 <1>	  6/1/94	KON		first checked in

	To Do:
*/

#ifndef __AddressBook__
#define __AddressBook__

#ifndef __PlayerDB__
#include "PlayerDB.h"
#endif __PlayerDB__

#define	kAddrBookClutCycleRate	3

#define kMaxAddressBookEntries	10

// ==BRAIN DAMAGE==
// Positions in main address dialog - this is shit: read from DB
//

#define kAddressNamePos			2
#define kAddressNameLength		110
#define kDateLastPlayedPos		104
#define kDateLastPlayedLength	86
#define kAddressStatsPos		192
#define kAddressStatsLength		78

enum	
{									// values for serverUniqueID.
	kUncorrelatedEntry = 0xFF		//  Others are assigned by the server
};



#endif __AddressBook__