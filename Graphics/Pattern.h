/*
	File:		Pattern.h

	Contains:	xxx put contents here xxx

	Written by:	Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 7/26/94	CMY		NewPatternBlockHigh - allocates pattern blocks from the top.
		 <3>	 7/16/94	BET		Add #ifdef __SERVER__ to vectorized function declarations.
		 <2>	  7/3/94	JBH		Added DeallocateTopPatternBlock.
		 <2>	 5/18/94	HEC		#include "OSManagers.h"
		 <1>	 5/18/94	HEC		first checked in

	To Do:
*/


#ifndef __Pattern__
#define __Pattern__

#include "OSManagers.h"
#include "Dispatcher.h"

#define PATSIZE(_x) 	((long)(_x)>>16)
#define PATOFFSET(_x) 	((long)(_x)&0xFFFF)


#ifdef __SERVER__

#define InitPatternManager ( a )				_InitPatternManager ( a )
#define NewPatternBlock ( a )					_NewPatternBlock ( a )
#define FreePatternBlock ( a )					_FreePatternBlock ( a )
#define DeallocateTopPatternBlock ( )			_DeallocateTopPatternBlock ( )

#else

short			InitPatternManager ( short numPats ) =
	CallDispatchedFunction( kInitPatternManager );
	
long			NewPatternBlock ( short numPats ) =
	CallDispatchedFunction( kNewPatternBlock );

long			NewPatternBlockHigh ( short numPats ) =
	CallDispatchedFunction( kNewPatternBlockHigh );

void			FreePatternBlock ( long pattern ) =
	CallDispatchedFunction( kFreePatternBlock );
	
void			DeallocateTopPatternBlock ( void ) =
	CallDispatchedFunction( kDeallocateTopPatternBlock );

#endif // __SERVER__
#endif __Pattern__
