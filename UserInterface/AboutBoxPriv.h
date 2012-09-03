/*
	File:		AboutBoxPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  8/4/94	JOE		added some globals
		 <5>	 7/26/94	ADS		Mostly complete
		 <4>	 7/17/94	ADS		Names, ditl, vectors
		 <3>	 7/13/94	ADS		More functionality again
		 <2>	 7/10/94	ADS		More functionality
		 <1>	  7/5/94	ADS		first checked in

	To Do:
*/


#include "SegaGraphics.h"
#include "Sprites.h"
#include "DitlMgr.h"
#include "DitlItemSetup.h"


typedef (*AboutProc) (void* globals, long param);

enum	{
	kSetupPlayers = 0,
	kSetupBall,
	kSetupGame,
	kDisposeGame,
	kRunGame,
	kMoveBall,
	kMovePlayer,
	kCheckPaddleHit,
	kSetupNames,
	kMoveNames,
	kDisposeNames,
	kFillOneName,
	
	kNumInternalProcs
};


typedef struct
{
	SpriteRef			sprite;
	long				x, y, xRate, yRate;		// fixed
} NameSprite;

#define kNumNameSprites	(6)		// NO MORE - 6/2*12 = 36, max is 40

#define	kPaddleClutRate	 7

typedef struct
{
	SpriteRef			paddleSprite;
	long				paddleX, paddleY;	// fixed
	long				maxCPURate, maxPlayerRate;
	long				playerAlive;
	long				controllerNum;
} Player;

typedef struct 
{
	DITLItemList*		theDitlList;
	ControlTable*		myControlTable;
	VBLProcRef			myvbl;
	short				namesClut;
	short				paddleClut;
	
	long				totalNames;

	long				lastTime, thisTime, delta;
	long				gameState;
	long				stateTimer;

	SpriteRef			ballSprite;
	long				ballX, ballY;		// fixed
	long				ballDx, ballDy;		// fixed
	short				ballRect[4];
	
	Player				player[2];
	
	NameSprite			names[kNumNameSprites];
	
	AboutProc			procs[kNumInternalProcs];
		
} AboutBoxGlobals;
