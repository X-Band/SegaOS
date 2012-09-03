/*
	File:		RankingMgr.h

	Contains:	xxx put contents here xxx

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	  8/2/94	JOE		added GetRankingSize(), added size param to AddRanking()
		 <8>	  8/1/94	JOE		added secret stats support
		 <7>	  8/1/94	JOE		hacks in progess
		 <6>	 7/26/94	JOE		changes for new ranking screen
		 <5>	 7/21/94	BET		Add padding to structs for UNIX matching
		 <4>	 7/17/94	DJ		#ifdef __SERVER__
		 <3>	 7/15/94	dwh		unix-ise.
		 <2>	  7/8/94	JOE		CountRankings() returns a short now
		 <1>	  7/7/94	JOE		first checked in

	To Do:
*/



#ifndef __Rankings__
#define __Rankings__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#define		kRankStringLen		20			// max # chars in a string, minus 1

/* In the following struct, the rankData consists of a bunch of C strings, laid out like so:

			- Game Name
			- Current Rank
			- Current Points
			- Next Rank
			- Next Points
			- Hidden Stats, if any exist
			
	Hidden stats, if they exist, each consist of a Kseq (variable length) followed by
	a C string.
*/
											
typedef struct RankingType
{
	unsigned long	gameID;
	unsigned char	userID;					// each user has his own rankings
	unsigned char	numHiddenStats;
	char			rankData[1];			// multiple C strings, stats shit
} RankingType;


typedef struct RankUpdate
{
	char			*gameNameString;
	char			*curLevelString;
	char			*curPointsString;
	char			*nextLevelString;
	char			*nextPointsString;
	unsigned char	hiddenStatsCount;
} RankUpdate;


#ifndef __SERVER__
#ifndef	unix

void	GetFirstRanking ( RankUpdate *theRank ) =
			CallDispatchedFunction( kGetFirstRanking );
			
void	GetNextRanking ( RankUpdate *theRank ) =
			CallDispatchedFunction( kGetNextRanking );
			
void	GetPrevRanking ( RankUpdate *theRank ) =
			CallDispatchedFunction( kGetPrevRanking );
			
char	*GetHiddenStat( long command ) =
			CallDispatchedFunction( kGetHiddenStat );
			
Boolean	NextRankingExists ( void ) =
			CallDispatchedFunction( kNextRankingExists );
			
Boolean	PrevRankingExists ( void ) =
			CallDispatchedFunction( kPrevRankingExists );
			
short	CountRankings ( void ) = 
			CallDispatchedFunction( kCountRankings ); 

DBID	GetFirstRankingID ( void ) =
			CallDispatchedFunction( kGetFirstRankingID );
			
DBID	GetNextRankingID ( DBID prevID ) =
			CallDispatchedFunction( kGetNextRankingID );
			
DBID	GetUniqueRankingID ( void ) =
			CallDispatchedFunction( kGetUniqueRankingID );
			
long	GetRankingSize( DBID theID ) =
			CallDispatchedFunction( kGetRankingSize );
			
DBErr	DeleteRanking ( DBID theID ) =
			CallDispatchedFunction( kDeleteRanking );
			
DBErr	AddRanking ( DBID theID, void *rankingData, long size ) =
			CallDispatchedFunction( kAddRanking );

RankingType	*GetRanking ( DBID theID ) =
			CallDispatchedFunction( kGetRanking );

#endif	/* ! unix */
#endif __SERVER__

#endif	/* __Rankings__ */
