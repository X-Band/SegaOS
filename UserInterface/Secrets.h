/*
	File:		Secrets.h

	Contains:	External interface to secret keys manager

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 8/18/94	CMY		Added constants for DB procs that are called from secret key
									sequences.
		 <7>	  8/5/94	JOE		added constant for Jizzlation disable sequence
		 <6>	  8/1/94	JOE		added TestThisSequence()
		 <5>	  8/1/94	JOE		make SecretSequence struct not so secret
		 <4>	 7/26/94	JBH		Added maze to secrets.
		 <3>	  7/4/94	ADS		Made "secrets" into a real manager
		<1+>	  7/4/94	ADS		MPW fucked me
		 <1>	  7/4/94	ADS		first checked in (partial functionality)

	To Do:
*/



#ifndef __SECRETS__
#define __SECRETS__

#ifndef __Dispatcher__
#include "Dispatcher.h"
#endif

/*
  Secret Procs
 */
 
enum
{
	kSecretChortleProc = 1,
	kLastSecretProc
};


/*
  Standard Standalone Secret Sequences
 */
 
#define		kJizzlationDisableSequence		128

typedef struct SecretSequence
{
	char	maxCount;
	char	command[3];			// up to 3 commands
	long	commandData[3];		// their params
	short	entries[1];
} SecretSequence;


long TrySecretCommand(long command, long screenID, long *forceJump ) =
	CallDispatchedFunction( kTrySecretCommand );

Boolean TestThisSequence( long command, SecretSequence* seqData ) =
	CallDispatchedFunction( kTestThisSequence );

Boolean PlayMaze( void ) =
	CallDispatchedFunction( kPlayMaze );
	
void	EndPlayMaze( void ) =
	CallDispatchedFunction( kEndPlayMaze );

#endif

