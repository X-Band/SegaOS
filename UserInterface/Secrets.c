/*
	File:		Secrets.c

	Contains:	Secret keys manager

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 8/18/94	CMY		Fixed bugs in the "execute ROM DB code" commands.
		<25>	 8/17/94	ADS		Fixed REFGLOBAL during hardinit - bad!
		<24>	 8/13/94	JBH		Slow that maze down!
		<23>	 8/13/94	JBH		ReadHardwareController instead of GetNextCommand for maze. Added
									mazesCompleted stat to maze database entry.
		<22>	 8/11/94	JBH		Call EraseGDevice at proper times in maze. Added sounds to maze.
									Made maze paths black, not clear.
		<21>	 8/10/94	JBH		Fixed maze so Shannon can't go into the walls.
		<20>	  8/7/94	ADS		Another "build" test
		<20>	  8/7/94	ADS		Another "build" test
		<19>	  8/7/94	ADS		Not really a change;  Just a test of the "build" system.
		<18>	  8/5/94	JOE		And another thing for TestThisSeq...don't TestSeq() if the last
									cmd was null!
		<17>	  8/5/94	JOE		Fix TestThisSequence() (don't stuff the cmd in the Q again...)
		<16>	  8/4/94	JBH		Initialized a variable before I used it. Big of me, huh?
		<15>	  8/4/94	JBH		De-seeded RandomShort calls.
		<14>	  8/1/94	JOE		added TestThisSequence()
		<13>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<12>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<11>	 7/27/94	JBH		Changed BlockMove to SegaBlockMove. Made time interval for maze
									time proc smaller.
		<10>	 7/26/94	JBH		Added maze to secrets. Also added a new command (enable maze) to
									secret sequences
		 <9>	 7/21/94	ADS		Allow multiple sequences to fire
		 <8>	 7/20/94	ADS		Vector Mania, the final fury
		 <7>	 7/17/94	JBH		Added kReturnParam command for secret sequences.
		 <6>	  7/6/94	HEC		Fix a globals offset hoser.
		 <5>	  7/5/94	ADS		Fixed timeout
		 <4>	  7/5/94	ADS		seems to have complete functionality
		 <3>	  7/4/94	ADS		Made "secrets" into a real manager
		<1+>	  7/4/94	ADS		MPW fucked me
		 <1>	  7/4/94	ADS		First checked in (partial functionality)

	To Do:
*/




#include "SecretsPriv.h"
#include "Secrets.h"
#include "SegaIn.h"
#include "Screens.h"
#include "Errors.h"
#include "DataBase.h"
#include "DBTypes.h"
#include "Time.h"
#include "SegaSound.h"
#include "Sprites.h"
#include "Heaps.h"
#include "Pattern.h"
#include "SegaScrn.h"
#include "GraphicsDB.h"
#include "Backdrops.h"
#include "SegaVDP.h"
#include "Utils.h"
#include "Controls.h"

/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
SecretsGlobals sGlob;
#endif


#define kSecretMaxTicks		(60 * 2)		/* 2 seconds per command	*/


/*----- Local Functions Declarations -----*/

long			_TrySecretCommand(long command, long screenID, long *forceJump );
Boolean 		_TestThisSequence( long command, SecretSequence* seqData );
Boolean			_PlayMaze( void );
void			_EndPlayMaze( void );



static long			_ExecCommands(SecretSequence* seqData, long* forceJump);
static SecretList* 	_GetSecretList( long curScreen );
static SecretSequence* _GetSecretSequence( DBID seqNum );
static void			_ResetSecretCommand( long curScreen, Boolean clearQueue );
static Boolean			_TestSequence(SecretSequence* seqData);

long				ExecCommands(SecretSequence* seqData, long* forceJump) =
				CallDispatchedFunction( kExecCommands );

SecretList*			GetSecretList( long curScreen ) =
				CallDispatchedFunction( kGetSecretList );

SecretSequence*		GetSecretSequence( DBID seqNum ) =
				CallDispatchedFunction( kGetSecretSequence );

void				ResetSecretCommand( long curScreen, Boolean clearQueue ) =
				CallDispatchedFunction( kResetSecretCommand );

Boolean				TestSequence(SecretSequence* seqData) =
				CallDispatchedFunction( kTestSequence );





/*----- Manager Controller -----*/

long
_SecretControl( short command, long data )
{
SecretsGlobals *globals;
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:

			GETMGRGLOBALSOFFSET(sGlob,offset);

			error = AllocateGlobalSpace ( kSecretManager, offset,
				sizeof(SecretsGlobals), (Ptr *) &globals );
			
			ASSERT_MESG ( !error, "Can't create secrets globals" );
			if ( error != noErr )
				{
				return error;
				}
				
			globals->mazeData = 0;
			
			/* install our selectors */
			SetDispatchedFunction ( kTrySecretCommand,		kSecretManager, _TrySecretCommand );
			SetDispatchedFunction ( kTestThisSequence,		kSecretManager, _TestThisSequence );
			SetDispatchedFunction ( kPlayMaze,				kSecretManager, _PlayMaze );
			SetDispatchedFunction ( kEndPlayMaze,			kSecretManager, _EndPlayMaze );

			SetDispatchedFunction ( kExecCommands,			kSecretManager, _ExecCommands );
			SetDispatchedFunction ( kGetSecretList,			kSecretManager, _GetSecretList );
			SetDispatchedFunction ( kGetSecretSequence,		kSecretManager, _GetSecretSequence );
			SetDispatchedFunction ( kResetSecretCommand,	kSecretManager, _ResetSecretCommand );
			SetDispatchedFunction ( kTestSequence,			kSecretManager, _TestSequence );

			error = noErr;
			break;
		
		case kSoftInialize:
			ResetSecretCommand(-1, true);
			REFGLOBAL(sGlob, mazeData) = 0;
			error = noErr;
			break;
			
		case kHardClose:
		case kSoftClose:
		case kCodeBlockMoved:
		case kGlobalsMoved:
			error = noErr;
			break;
		}
	return error;
}


/*----- Local Functions Declarations -----*/

long _TrySecretCommand(long command, long screenID, long *forceJump )
{
	SecretList*		sequenceList;
	long			myCount;
	long			tryScreen = screenID;
	Boolean			globalListOK = true;
	long			result = 0;
	
	// change screens, clear queue

	if (screenID != REFGLOBAL( sGlob, screen ))
		{ ResetSecretCommand(screenID, true); return result; };

	// time out, clear queue

	if (GetCurrentTime() > REFGLOBAL( sGlob, time ))
		{ ResetSecretCommand(screenID, true); return result; };

	// no command, just return for now

	if (!command)
		return 0;

	// potential hit, add it to queue

	for (myCount = kMaxSecretQueue-2; myCount >= 0; --myCount)
		REFGLOBAL(sGlob, queue)[myCount+1] = REFGLOBAL(sGlob, queue)[myCount];
	REFGLOBAL(sGlob, queue)[0] = command;
	REFGLOBAL(sGlob, time) = GetCurrentTime() + kSecretMaxTicks;

	// iterate through potential sequences

trylist:
	sequenceList = GetSecretList(tryScreen);
	if (sequenceList)	{
		short seqCount;
		if (sequenceList->flags & kNoGlobalList)
			globalListOK = false;
		for (seqCount = 0; seqCount < sequenceList->count; ++seqCount)	{
			DBID			seqNum;
			SecretSequence*	seqData;
			
			seqNum = sequenceList->entries[seqCount];
			seqData = GetSecretSequence(seqNum);
			if (!seqData)
				continue;
				
			//  NOTE:  We don't stop, even if we find a hit.  This means
			//	you could have multiple, equivalent hits on a given
			//  key sequence.  This allows us to make the final determination
			//  between two different sequences, by calling a function in that
			//  sequence.
			//
			//  The last executed sequence gets to return the return value.
				
			if (TestSequence(seqData))	{					// a hit?
				result = ExecCommands(seqData, forceJump);	// should return the last "goto"
			}
		}
	}
	if (tryScreen && globalListOK)	{	// after trying a real screen,
		tryScreen = 0;					// loop back and try "all screens" list
		goto trylist;
	}
	
	// still collecting good data, but no hits (yet...)
	
	return result;
}
	

Boolean _TestThisSequence( long command, SecretSequence* seqData )
{
	if (!command)			// no command, just return for now
		return 0;

	if (!seqData)
		return( false );
		else
			return( TestSequence(seqData) );
}
	

typedef Boolean (*SecretFunc) (short param);

static long _ExecCommands(SecretSequence* seqData, long* forceJump)
{
long		finalCommand = 0;
long		iii;
SecretFunc	func;
DBID		funcID;
DBType		funcType;
Boolean		keepGoing = true;

	for (iii = 0; (iii < 3) && keepGoing; ++iii)
		switch (seqData->command[iii])	{
			case kGotoScreen:
				*forceJump = seqData->commandData[iii];
				finalCommand = kForceGotoScreen;
				break;
			case kPlaySound:				// sound #
				PlayDBFX(seqData->commandData[iii], 0, 0);
				break;
			case kExecBlock:				// 24:31=type, 16:23=id, 0:15=param  returns BOOL
				funcType = seqData->commandData[iii] >> 24;
				funcID = seqData->commandData[iii] >> 16;
				func = (SecretFunc *) DBGetItem( funcType, funcID );
				keepGoing = false;
				if (func) 					// don't trust short-circuiting here!
					if ((func)(seqData->commandData[iii] & 0xFFFF))
						keepGoing = true;
				break;
			case kReturnParam:
				finalCommand = seqData->commandData[iii];
				break;
			case kEnableMaze:
				REFGLOBAL(sGlob, mazeData) = 1;
				break;
		}

	return finalCommand;
}


static Boolean _TestSequence(SecretSequence* seqData)
{
	long count1 = 0;						// count up through queue
	long count2 = seqData->maxCount;		// count down through list
	
	while (--count2 >= 0)
		if (seqData->entries[count2] != REFGLOBAL(sGlob, queue)[count1++])
			return false;
	
	return true;
}
	


static void _ResetSecretCommand( long curScreen, Boolean clearQueue )
{
	if (clearQueue)	{
		REFGLOBAL(sGlob, queue)[0] = 0xFFFFFFFF;	// making 1st entry invalid "clears" queue
	}
	REFGLOBAL( sGlob, time ) = 0x7FFFFFFF;	// first key can come any time
	REFGLOBAL( sGlob, screen ) = curScreen;
}


static SecretList* _GetSecretList( long curScreen )
{
	return (SecretList *) DBGetItem( kSecretListType, curScreen );
}


static SecretSequence* _GetSecretSequence( DBID seqNum )
{
	return (SecretSequence *) DBGetItem( kSecretSequenceType, seqNum );
}

typedef struct MazeData {
	short			mazeColor;
	short			posX;
	short			posY;
	short			cellX;
	short			cellY;
	unsigned long	mazesCompleted;
	char			theMaze[88];
} MazeData;

typedef struct MazePoo {
	TimeProcRef		mazeTimeProc;
	SparkRef		playerSpark;
	SpriteRef		playerSprite;
	long			mazePatterns;
	MazeData		*maze;
} MazePoo;

static Boolean GetSquare( register char *base, register short x, register short y )
{
//	ASSERT_MESG(x >= 0 && x <= 31, "illegal x parameter");
//	ASSERT_MESG(y >= 0 && y <= 21, "illegal y parameter");

	base += (y * 4) + (x >> 3);
	if ((*base) & (1 << (7-(x & 7))))
		return true;
	else
		return false;
}

static void SetSquare( register char *base, register short x, register short y, register Boolean value )
{
	register unsigned char mask;
	ASSERT_MESG(x >= 0 && x <= 31, "illegal x parameter");
	ASSERT_MESG(y >= 0 && y <= 21, "illegal y parameter");
	
	base += (y * 4) + (x >> 3);
	mask = (1 << (7-(x & 7)));
	
	if (value)
		*base |= mask;
	else
		*base &= ~mask;
}

static void DrawMaze( MazePoo *poo )
{
	short colorBlock;
	short iii;
	unsigned long	screenBase;
	short	curDevice;
	SegaGDevice * 	device;
	unsigned short temp;
	short x, y;
	short patBegin;
	short pat;
	char *mazeBase = poo->maze->theMaze;
	
	colorBlock = poo->maze->mazeColor;
	colorBlock |= colorBlock << 4;
	colorBlock |= colorBlock << 8;

	OwnVDP();
	
	/* fill pattern blocks with right data */
	SetVDPIncrement( 2 );
	SetVDPVRAMAddress( (unsigned long) PATOFFSET(poo->mazePatterns) << 5 );

	for (iii=0;iii<16;iii++)
		WriteWordToVDP(0x7777);

	for (iii=0;iii<16;iii++)
		WriteWordToVDP( colorBlock );

	/* fill character memory with maze */
	curDevice = GetCurrentDevice();
	device = GetSegaGDevice ( curDevice );
	screenBase = (unsigned long) GetSegaScreenBaseAddr( curDevice );
	patBegin = PATOFFSET(poo->mazePatterns);

	for (y=0; y<21; y++) {
		temp = 2 * device->charScreenWidth;
		temp *= (y+3);
		temp += 8;			/* we want pattern cell at x=4 (0 based) */

		SetVDPVRAMAddress( screenBase + temp );
		for (x=0;x<31;x++) {
			if (GetSquare(mazeBase, x, y))
				pat = patBegin+1;
			else
				pat = patBegin;
			WriteWordToVDP( pat );
		}
	}
	
	FreeVDP();
	
	VDPIdle();
}

/* x is 0..14, y is 0..9 */
static short BurrowCount( char *base, short x, short y)
{
	short count = 0;

	ASSERT_MESG(x>=0 && x<=14, "illegal X in BurrowCount");
	ASSERT_MESG(y>=0 && y<=9, "illegal Y in BurrowCount");

	if (x>0) {
		if (GetSquare(base, x*2, y*2+1) == false)
			count++;
	}
	if (x<14) {
		if (GetSquare(base, x*2+2, y*2+1) == false)
			count++;
	}
	if (y>0) {
		if (GetSquare(base, x*2+1, y*2) == false)
			count++;
	}
	if (y<9) {
		if (GetSquare(base, x*2+1, y*2+2) == false)
			count++;
	}
	
	return count;
}

/* hack until RandomShort works better */
static short JoshRandom()
{
	short result;
	
	do {
		result = RandomShort() & 0x0f;
		result += (RandomShort() & 0x0f) << 4;
	} while (result > 149);

	return result;
}

static void GenerateNewMaze( MazeData *someData )
{
	short row, col;
	short cellsLeft;
	short directions;
	char *base = someData->theMaze;
	short dir;
	
	someData->mazeColor += 2;
	
	if (someData->mazeColor == 16)
		someData->mazeColor = 9;

	if (someData->mazeColor == 17)
		someData->mazeColor = 8;

	for (row=0;row<21;row++) {
		if (row & 1) {
			someData->theMaze[row*4] = 0xaa;
			someData->theMaze[row*4+1] = 0xaa;
			someData->theMaze[row*4+2] = 0xaa;
			someData->theMaze[row*4+3] = 0xaa;		
		} else {
			someData->theMaze[row*4] = 0xff;
			someData->theMaze[row*4+1] = 0xff;
			someData->theMaze[row*4+2] = 0xff;
			someData->theMaze[row*4+3] = 0xfe;		
		}
	}
	someData->theMaze[84] = 0;
	someData->theMaze[85] = 0;
	someData->theMaze[86] = 0;
	someData->theMaze[87] = 0;
	
	cellsLeft = 149;
	
	row = JoshRandom();	
	col = row / 10;
	row %= 10;
	
	while (cellsLeft) {
		directions = 0;
		if (col > 0) {
			if (BurrowCount(base, col-1, row) == 0)
				directions = 1;
		}
		if (col < 14) {
			if (BurrowCount(base, col+1, row) == 0)
				directions |= 2;
		}
		if (row > 0) {
			if (BurrowCount(base, col, row-1) == 0)
				directions |= 4;
		}
		if (row < 9) {
			if (BurrowCount(base, col, row+1) == 0)
				directions |= 8;
		}
		
		/* directions == 0 means the cell we are in is a dead end */
		
		if (directions) {
			/* pick one, burrow */
			while (directions) {
				dir = RandomShort() & 0x03;
				
				switch (dir) {
					case 0:			/* burrow left */
						if (directions & 0x01) {
							SetSquare(base, col*2, row*2+1, false);
							col--;
							directions = 0;
						}
						break;
					case 1:			/* burrow right */
						if (directions & 0x02) {
							SetSquare(base, col*2+2, row*2+1, false);
							col++;
							directions = 0;
						}
						break;
					case 2:			/* burrow up */
						if (directions & 0x04) {
							SetSquare(base, col*2+1, row*2, false);
							row--;
							directions = 0;
						}
						break;
					case 3:			/* burrow down */
						if (directions & 0x08) {
							SetSquare(base, col*2+1, row*2+2, false);
							row++;
							directions = 0;
						}
						break;
				}
			}
			cellsLeft--;
			directions = 1;
		}
		
		if (!directions) {
			while (true) {
				row = JoshRandom();	
				col = row / 10;
				row %= 10;
				if (BurrowCount(base, col, row))
					break;
			}
		}
		
	}
	
	col = 0;		/* just in case they all have 2 or more (very rare!) */
	do {
		row = JoshRandom() % 10;
		col++;
	} while (BurrowCount(base, 14, row) != 1 && col < 16);
	SetSquare(base, 30, row*2+1, false);
	
	col = 0;
	do {
		row = JoshRandom() % 10;
		col++;
	} while (BurrowCount(base, 0, row) != 1 && col < 16);
	
	
	/* place player */
	someData->posX = 168;
	someData->posY = 160+16*row;
	someData->cellX = 1;
	someData->cellY = row*2+1;
	
}

static long MazeTimeProc( long time, long data )
{
	MazePoo *somePoo = (MazePoo *) data;
	MazeData *someData = somePoo->maze;
	short command1, dummy;
	char *base;
	short x,y;
	short right, left, up, down;

	/* check position. If to the right of goal, create new maze, draw it */
	if (someData->cellX > 30) {
		
		PlayDBFX( kMazeCompleteSnd, 0, 0 );
		
		GenerateNewMaze( someData );
		DrawMaze( somePoo );
		MoveSprite( somePoo->playerSprite, someData->posX, someData->posY );
		DrawSprite( somePoo->playerSprite );
		someData->mazesCompleted++;
		
	}
	
	/* check controller. If moved, and move is legal, move sprite */
	command1 = ReadHardwareController( 1 );
	
	if (command1 & (kUP | kDOWN | kLEFT | kRIGHT)) {
		base = someData->theMaze;
		
		x = someData->cellX;
		y = someData->cellY;
		
		right = (command1 & kRIGHT);
		left = (command1 & kLEFT);
		up = (command1 & kUP);
		down = (command1 & kDOWN);
		
		if ( right && up && ( GetSquare( base, x+1, y-1 ) == false ) )
		{
			someData->posX += 8;
			someData->posY -= 8;
			someData->cellX++;
			someData->cellY--;
		}
		else if ( right && down && ( GetSquare( base, x+1, y+1 ) == false ) )
		{
			someData->posX += 8;
			someData->posY += 8;
			someData->cellX++;
			someData->cellY++;
		}
		else if ( left && down && ( GetSquare( base, x-1, y+1 ) == false ) )
		{
			someData->posX -= 8;
			someData->posY += 8;
			someData->cellX--;
			someData->cellY++;
		}
		else if ( left && up && ( GetSquare( base, x-1, y-1 ) == false ) )
		{
			someData->posX -= 8;
			someData->posY -= 8;
			someData->cellX--;
			someData->cellY--;
		}
		else if ( right && (GetSquare( base, x+1, y ) == false) )
		{
			someData->posX += 8;
			someData->cellX++;
		}
		else if ( left && (GetSquare( base, x-1, y ) == false) )
		{
			someData->posX -= 8;
			someData->cellX--;
		}
		else if ( down && (GetSquare( base, x, y+1 ) == false) )
		{
			someData->posY += 8;
			someData->cellY++;
		}
		else if ( up && (GetSquare( base, x, y-1 ) == false) )
		{
			someData->posY -= 8;
			someData->cellY--;
		}
		
		/* If the player tried to move, but couldn't, play a noise */
		if ( x == someData->cellX && y == someData->cellY )
		{
			PlayDBFX( kMazeBadMoveSnd, 0, 0 );
		}
		else			/* player moved - play a happy sound */
		{
			PlayDBFX( kMazeMoveSnd, 0, 0 );
		}

		MoveSprite( somePoo->playerSprite, someData->posX, someData->posY );
		DrawSprite( somePoo->playerSprite );
	}
	
	VDPIdle();

	return (GetCurrentTime()+6);
}

/* Called at start of FindOpponent. Returns false if the player didn't request a maze by */
/* pressing the secret sequence. Else it sets up the world and returns true. Setting up the */
/* world involves drawing the correct screen, setting up the time procs, etc. */
/* Also called by MazeScreen.c, which exists merely to call this. */
Boolean _PlayMaze( void )
{
	MazePoo *somePoo;
	MazeData *someData;
	void *databaseEntry;

	if (REFGLOBAL( sGlob, mazeData) == 0)
		return ( false );

	ASSERT_MESG( REFGLOBAL( sGlob, mazeData) == 1, "unexpected maze data value for start!");
		
	somePoo = NewMemory( kTemp, sizeof( MazePoo ) );
	ASSERT_MESG(somePoo, "NewMemory failed.");
	
	databaseEntry = DBGetItem( kMazeDataType, 0 );
	
	someData = (MazeData *) NewMemory(kTemp, sizeof( MazeData ) );
	ASSERT_MESG(someData, "NewMemory failed.");
	someData->mazeColor = 14;
	someData->mazesCompleted = 0L;
	
	if ( databaseEntry ) {
		SegaBlockMove( databaseEntry, (Ptr) someData, sizeof( MazeData ) );
	} else {
		GenerateNewMaze( someData );
	}
	
	/* draw screen */
	EraseGDevice( kScrollA );
	SetBackdropID ( kBackdrop, true, 0 );	/* ===BRAIN DAMAGE=== need right constant! */

	/* Play some background music */
	StartDBBGM( kMazeBackgroundMusic );

	somePoo->maze = someData;
	somePoo->playerSpark = NewSpark( 10 );
	somePoo->playerSprite = GetSparkSprite( somePoo->playerSpark );
	somePoo->mazePatterns = NewPatternBlock( 2 );
	
	MoveSprite( somePoo->playerSprite, someData->posX, someData->posY );
	DrawSprite( somePoo->playerSprite );
	
	ASSERT_MESG( somePoo->mazePatterns, "couldn't allocate 2 patterns!!");
	
	REFGLOBAL( sGlob, mazeData) = (long) somePoo;
	
	DrawMaze( somePoo );
	
	/* setup time proc */
	somePoo->mazeTimeProc = AddTimeRequest( MazeTimeProc, (long)somePoo );
	
	return (true);
}

void _EndPlayMaze( void )
{
	MazePoo *somePoo;
	MazeData *someData;
	
	if (REFGLOBAL( sGlob, mazeData) < 2)		// nobody should call in this state, but just in case
		return;

	somePoo = (MazePoo *) REFGLOBAL( sGlob, mazeData);	
	
	RemoveTimeRequest( somePoo->mazeTimeProc );		/* stop game */
	DisposeSpark( somePoo->playerSpark );			/* throw away player */
	FreePatternBlock( somePoo->mazePatterns );		/* throw away pattern mems */
	DBAddItem( kMazeDataType, 0, somePoo->maze );	/* save maze state in DB */

	DisposeMemory( (void *) somePoo->maze );
	DisposeMemory( (void *) somePoo );
	
	EraseGDevice( kScrollA );

	REFGLOBAL( sGlob, mazeData ) = 0;
}