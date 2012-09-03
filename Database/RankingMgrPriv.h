/*
	File:		RankingMgrPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 8/14/94	JOE		secret stats are smaller now
		 <6>	  8/1/94	JOE		hacks in progress
		 <5>	 7/26/94	JOE		update for new ranking screen
		 <4>	 7/25/94	JOE		updated for Laid changes
		 <3>	 7/23/94	JOE		updating for new ranking screen
		 <2>	  7/8/94	SAH		Declare the RankingGlobals type before an instance.
		 <1>	  7/8/94	JOE		first checked in

	To Do:
*/



#ifndef __RankingsPriv__
#define __RankingsPriv__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifndef __DataBase__
#include "DataBase.h"
#endif


#define		kDefaultRankingID	0			// if the user has no Rankings, display this one
#define		kMaxNumRankings		64			// max # of rankings a user can have
#define		kMaxNumSecretStats	16


typedef struct RankSecretSeq				// smaller version embedded in RANK resources
{
	char	maxCount;
	char	pad;
	short	entries[1];
} RankSecretSeq;


typedef struct RankingGlobals
{
	DBID			curUserRanks[kMaxNumRankings];
	unsigned char	numSecretStats;
	RankSecretSeq	*secretStatSeqs[kMaxNumSecretStats];		
	unsigned char	curRanking;								// index into curUserRanks
} RankingGlobals;



#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	RankingGlobals
#else
extern RankingGlobals rankGlobs;
#endif





#endif
