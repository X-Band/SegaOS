/*
	File:		RankingMgr.c

	Contains:	xxx put contents here xxx

	Written by:	Joe britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<11>	 8/15/94	JOE		Fix possible alignment problem
		<10>	 8/14/94	JOE		Secret stats are smaller now
		 <9>	  8/4/94	JOE		Fixed crasher if there were no secret stats
		 <8>	  8/2/94	JOE		added GetRankingSize(), added size param to AddRanking()
		 <7>	  8/1/94	JOE		add secret stats suppr
		 <6>	  8/1/94	JOE		hacks in progress
		 <5>	 7/26/94	JOE		Changes for new Ranking screen
		 <4>	 7/23/94	JOE		Updating for new Ranking screen
		 <3>	 7/19/94	ADS		Vector Mania
		 <2>	  7/8/94	JOE		CountRankings() returns a short now
		 <1>	  7/7/94	JOE		first checked in
	To Do:
*/


#include "SegaOS.h"
#include "DBTypes.h"
#include "DitlMgr.h"
#include "DataBase.h"
#include "Errors.h"
#include "StatText.h"
#include "StatIconGraphics.h"
#include "GameID.h"
#include "UsrConfg.h"
#include "Personification.h"
#include "Errors.h"
#include "Secrets.h"
#include "RankingMgr.h"
#include "RankingMgrPriv.h"
#include "TextButt.h"
#include "StringDB.h"
#include "heaps.h"

#ifndef SIMULATOR
RankingGlobals rankGlobs;
#endif	



void		_GetFirstRanking ( RankUpdate *theRank );
void		_GetNextRanking ( RankUpdate *theRank );
void		_GetPrevRanking ( RankUpdate *theRank );
char		*_GetHiddenStat( long command );
Boolean		_NextRankingExists ( void );
Boolean		_PrevRankingExists ( void );
short		_CountRankings ( void );
DBID		_GetFirstRankingID ( void );
DBID		_GetNextRankingID ( DBID prevID );
DBID		_GetUniqueRankingID ( void );
long		_GetRankingSize( DBID theID );
DBErr		_DeleteRanking ( DBID theID );
DBErr 		_AddRanking ( DBID theID, void *rankingData, long size );
RankingType	*_GetRanking( DBID theID );

static void FillInFromDBRank( RankUpdate *theRank );
static void CacheHiddenStatSeqs( RankSecretSeq *seq );


long
_RankingControl ( short command, long data )
{
long 			offset;
short 			error;
RankingGlobals	*globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			
			GETMGRGLOBALSOFFSET(rankGlobs,offset);
			
			error = AllocateGlobalSpace ( kRankingManager, offset,
											sizeof(RankingGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create Ranking globals" );
			if ( error != noErr )
				{
				return error;
				}
		
			/* install our selectors */
			SetDispatchedFunction ( kGetFirstRanking,		kRankingManager,	_GetFirstRanking );
			SetDispatchedFunction ( kGetNextRanking,		kRankingManager,	_GetNextRanking );
			SetDispatchedFunction ( kGetPrevRanking,		kRankingManager,	_GetPrevRanking );
			SetDispatchedFunction ( kGetHiddenStat,			kRankingManager,	_GetHiddenStat );
			SetDispatchedFunction ( kNextRankingExists,		kRankingManager,	_NextRankingExists );
			SetDispatchedFunction ( kPrevRankingExists,		kRankingManager,	_PrevRankingExists );
			SetDispatchedFunction ( kCountRankings,			kRankingManager,	_CountRankings );
			SetDispatchedFunction ( kGetFirstRankingID,		kRankingManager,	_GetFirstRankingID );
			SetDispatchedFunction ( kGetNextRankingID,		kRankingManager,	_GetNextRankingID );
			SetDispatchedFunction ( kGetUniqueRankingID,	kRankingManager,	_GetUniqueRankingID );
			SetDispatchedFunction ( kGetRankingSize,		kRankingManager,	_GetRankingSize );
			SetDispatchedFunction ( kDeleteRanking,			kRankingManager,	_DeleteRanking );
			SetDispatchedFunction ( kAddRanking,			kRankingManager,	_AddRanking );
			SetDispatchedFunction ( kGetRanking,			kRankingManager,	_GetRanking );
			break;
		
		case kSoftInialize:
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}



void _GetFirstRanking ( RankUpdate *theRank )
{
unsigned char		jizz;
unsigned char		index;
DBID				checkID;
short				numRanks;
RankingType			*RankPtr;
long				curGameID;
userIdentification	*curUser;

	curUser = GetCurrentLocalUser();					// who is it
	curGameID = GetGameID();							// what they're playing
	
	for (jizz = 0; jizz != kMaxNumRankings; jizz++)						// zero the rank ID array
		REFGLOBAL( rankGlobs, curUserRanks[jizz] ) = kDefaultRankingID;

	REFGLOBAL( rankGlobs, curRanking ) = 0;
	
	numRanks = CountRankings();

// walk the RANK resources & fill in the curUserRanks array

	checkID = GetFirstRankingID();
	index = 0;

	for (jizz = 0; jizz < numRanks; jizz++ )
		{
		RankPtr = GetRanking( checkID );
		if ( RankPtr && (RankPtr->userID == curUser->userID) )
			{
			REFGLOBAL( rankGlobs, curUserRanks[index] ) = checkID;
			if (RankPtr->gameID == curGameID)						// also the current game?
				REFGLOBAL( rankGlobs, curRanking ) = index;
			index++;
			}
		checkID = GetNextRankingID( checkID );
		}

	FillInFromDBRank( theRank );
}


void _GetNextRanking ( RankUpdate *theRank )
{
	REFGLOBAL( rankGlobs, curRanking )++;
	FillInFromDBRank( theRank );
}


void _GetPrevRanking ( RankUpdate *theRank )
{
	REFGLOBAL( rankGlobs, curRanking )--;
	FillInFromDBRank( theRank );
}


char *_GetHiddenStat( long command )
{
unsigned short	iii,jjj;
unsigned short	seqLen;
SecretSequence	*seq;
RankSecretSeq	*rankSeq;

	for ( iii = 0; iii != REFGLOBAL( rankGlobs, numSecretStats ); iii++ )
	{
		if ( REFGLOBAL( rankGlobs, secretStatSeqs[iii] ) )
		{
			rankSeq = REFGLOBAL( rankGlobs, secretStatSeqs[iii] );
			seq = (SecretSequence *)NewMemory( kTemp, sizeof(SecretSequence) + (((rankSeq->maxCount) - 1) << 1) ); 
			seq->maxCount = rankSeq->maxCount;
			for ( jjj = 0; jjj != seq->maxCount; jjj++ )
				 seq->entries[jjj] = rankSeq->entries[jjj];
			
			if ( TestThisSequence( command, seq ) )
			{
				DisposeMemory( seq );
				seqLen = sizeof(RankSecretSeq) + ((rankSeq->maxCount - 1) << 1);
				return((char *)((unsigned long)rankSeq + seqLen));					// return ptr to following string
			}
			
			DisposeMemory( seq );
		}
	}

	return (0);
}


Boolean _NextRankingExists( void )
{
	if ( REFGLOBAL( rankGlobs, curUserRanks[(REFGLOBAL( rankGlobs, curRanking ) + 1)] ) != 0 )
		return( true );
	else
		return( false );
}


Boolean _PrevRankingExists( void )
{
	if ( REFGLOBAL( rankGlobs, curRanking ) != 0 )
		return( true );
	else
		return( false );
}


short _CountRankings ( void )
{
	return ( DBCountItems ( kUserRankings ) );
}


DBID _GetFirstRankingID ( void )
{
	return ( DBGetFirstItemID ( kUserRankings ) );
}


DBID _GetNextRankingID ( DBID prevID )
{
	return ( DBGetNextItemID ( kUserRankings, prevID ) );
}


DBID _GetUniqueRankingID ( void )
{
	return ( DBGetUniqueID ( kUserRankings ) );
}

long _GetRankingSize( DBID theID )
{
	return ( DBGetItemSize ( kUserRankings, theID ) );
}

DBErr _DeleteRanking ( DBID theID )
{
	return ( DBDeleteItem ( kUserRankings, theID ) );
}


DBErr _AddRanking ( DBID theID, void *rankingData, long size )
{
	return ( DBAddItemPtrSize ( kUserRankings, theID, rankingData, size ) );
}

RankingType	*_GetRanking( DBID theID )
{
	return ( (RankingType *)DBGetItem( kUserRankings, theID ) );
}


// technically, this should be in a vector, but since it's only called
// by two tiny covers, the patching expense would be low.

static void FillInFromDBRank( RankUpdate *theRank )
{
RankingType		*RankPtr;
unsigned short	iii;
RankSecretSeq 	*rankSeq;

	RankPtr = GetRanking( REFGLOBAL( rankGlobs, curUserRanks[REFGLOBAL( rankGlobs, curRanking )] ) );
		
	if (RankPtr)
	{
	  theRank->gameNameString = RankPtr->rankData;
	  iii = 0;
	  
	  while ( RankPtr->rankData[iii] )
	  	iii++;
	  theRank->curLevelString = &(RankPtr->rankData[++iii]);
	  
	  while ( RankPtr->rankData[iii] )
	  	iii++;
	  theRank->curPointsString = &(RankPtr->rankData[++iii]);
	  
	  while ( RankPtr->rankData[iii] )
	  	iii++;
	  theRank->nextLevelString = &(RankPtr->rankData[++iii]);
	  
	  while ( RankPtr->rankData[iii] )
	  	iii++;
	  theRank->nextPointsString = &(RankPtr->rankData[++iii]);

	  theRank->hiddenStatsCount = RankPtr->numHiddenStats;
	  REFGLOBAL( rankGlobs, numSecretStats) = RankPtr->numHiddenStats;
	  
	  if ( RankPtr->numHiddenStats )
	  {
	  	while ( RankPtr->rankData[iii] )			// skip past next points string
	  		iii++;
	  		
		rankSeq = (RankSecretSeq *)&(RankPtr->rankData[++iii]);						
		rankSeq = (RankSecretSeq *)(((unsigned long)rankSeq + 1) & 0xfffffffe);		// it's word-aligned in the resource

	  	CacheHiddenStatSeqs( rankSeq );
	  }
	}
}


static void CacheHiddenStatSeqs( RankSecretSeq *rankSeq )
{
unsigned short	seqNum;
unsigned short	seqLen;

	seqNum = 0;
	
	do
	{
		REFGLOBAL( rankGlobs, secretStatSeqs[seqNum] ) = rankSeq;			// cache ptr to this one
		
		seqLen = sizeof(RankSecretSeq) + ((rankSeq->maxCount - 1) << 1);	// each entry is a short
	
		rankSeq = (RankSecretSeq *)((unsigned long)rankSeq + seqLen);		// add len of keyseq
		while ( *(char *)rankSeq )											// walk past string
			rankSeq = (RankSecretSeq *)((unsigned long)rankSeq + 1);
			
		rankSeq = (RankSecretSeq *)((unsigned long)rankSeq + 1);			// now seq points to the next secret sequence
		
		rankSeq = (RankSecretSeq *)(((unsigned long)rankSeq + 1) & 0xfffffffe);		// it's word-aligned in the resource
		
		seqNum++;
	}
	while ( seqNum < REFGLOBAL( rankGlobs, numSecretStats) );				// do however many there are (up to kMaxNumSecretStats)
}







