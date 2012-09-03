/*
	File:		SecretsPriv.h

	Contains:	Internal structures for secret keys manager

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  8/1/94	JOE		moved SecretSequence struct to Secrets.h
		 <5>	 7/26/94	JBH		Added mazeData to globals.
		 <4>	 7/21/94	ADS		Allow longer sequences (max 12)
		 <3>	 7/17/94	JBH		Added kReturnParam for secret dialog sequences.
		 <2>	  7/5/94	ADS		final structures (yeah, right)
		 <1>	  7/4/94	ADS		first checked in (dummy functionality)

	To Do:
*/


#ifndef __SECRETS_PRIV_H
#define __SECRETS_PRIV_H

#ifndef __DataBase__
#include	"Database.h"
#endif


#define kMaxSecretQueue	12


typedef struct SecretsGlobals
{
	long		mazeData;
	long		time;
	long		screen;
	long		queue[kMaxSecretQueue];
} SecretsGlobals;


#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	SecretsGlobals
#else
extern SecretsGlobals sGlob;
#endif



// The database type "SecretSequence" describes a single
// power-up sequence:  A group of controller strokes, and a
// set of commands to execute.

enum	{				// codes for "command" field
	kNoCommand = 0,			// (param unused)
	kGotoScreen,			// screen #
	kPlaySound,				// sound #
	kExecBlock,				// 24:31=type, 16:23=id, 0:15=param  returns BOOL
	kReturnParam,			// return parameter
	kEnableMaze				// sets maze to active on next FindOpponent
};


// The database type "SecretScreen info" is a lookup table
// for the secret sequences to be allowed per screen.  In addition,
// database entry #0 is a lookup for sequences which are allowed
// on ANY screen.

enum	{				// codes for "flags" field
	kNoGlobalList = 1			// this list overrides global list
};

typedef struct SecretList
{
	char	count;
	char	flags;
	DBID	entries[1];		// per screen, check these sequences
} SecretList;


#endif



