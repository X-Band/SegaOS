/*
	File:		PatchDB.c

	Contains:	xxx put contents here xxx

	Written by:	Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<74>	 8/28/94	SGP		HandleGameError was passing in the wrong error to
									PeerConnectionDropped.
		<73>	 8/28/94	SAH		Allocate a seperate GTSession in PlayCurrentGame for syncotron.
									If we weren't passed one in, allocate a new one for the game.
									Make sure the game heap is the current perm heap before we start
									the game.
		<72>	 8/28/94	JOE		Made LoadGamePatch() do an ASSERT if the current heap is not the
									gGameRuntimeHeap
		<71>	 8/28/94	JOE		Bracket NewMemory for LoadGamePatch() to select the
									gGameRuntimeHeap
		<70>	 8/27/94	SAH		Handle errors better in PlayCurrentGame.
		<69>	 8/27/94	JBH		Removed InstallReliableAddress from blocks allocated in game
									heap (no longer needed and makes us less likely to lose our
									mind)
		<68>	 8/27/94	SAH		New sinko interface.
		<67>	 8/26/94	HEC		New game dispatcher functions from PModem
		<66>	 8/25/94	HEC		Removed kGameEnded box state.  Move UseBoxProblemToken to
									AskForReplay.
		<65>	 8/24/94	SAH		Init the game's GTSession to 19 bit packet format.
		<64>	 8/23/94	SAH		Be more intelligent in HandleGameError with respect to catching
									all the call waiting errors.
		<63>	 8/22/94	ADS		Added ability to determine end of safe-rom-area
		<62>	 8/22/94	SAH		Stinko.
		<61>	 8/21/94	ADS		New lomem layout
		<60>	 8/21/94	JOE		Added GetGamePatchFlags()
		<59>	 8/20/94	HEC		Don't call GTSession_ErrorRecovery in HandleGameError.
		<58>	 8/19/94	ADS		GetPatch -> GetGamePatch for clarity
		<57>	 8/19/94	JBH		Ensure we don't trash the current game's game patch.
		<56>	 8/19/94	HEC		Pass reason into UseBoxProblemToken
		<55>	 8/19/94	JOE		Taunt screen only displays a value for the first game
		<54>	 8/18/94	SAH		Made the game dispatcher be aware of ram based code.
		<53>	 8/18/94	JBH		Added intelligence to game-patch disposing when a new game patch
									doesn't fit.
		<52>	 8/15/94	JOE		Add DisposeGamePatch
		<51>	 8/14/94	ADS		Removed obsolete #include
		<50>	 8/12/94	HEC		Call UseProblemToken -- recredit user's smart card if game
									failed to complete in certain time period.
		<49>	 8/10/94	SAH		Keep track of original error in HandleGameError.
		<48>	 8/10/94	HEC		Restart game in HandleGameError if OS has been enabled after
									peer reconnection.
		<47>	  8/6/94	HEC		Wrote out GameDoDialog to call gamepatch to display message.
		<46>	  8/5/94	SAH		Some debugging printfs. Call GTSession_ErrorRecover for the game
									patch after we have reestablished the connection.
		<45>	  8/5/94	SAH		Minor tweaks.
		<44>	  8/4/94	SAH		Rewrote game results stuff. Added Game error stuff. Fleshed out
									HandleGameError more.
		<43>	  8/1/94	HEC		Removed HandleCallWaiting....became PeerConnectionDropped.
		<42>	 7/31/94	SAH		Fuck.
		<41>	 7/31/94	SAH		Switch the vbl handler over to the timer vbl before we go off to
									play the game.
		<40>	 7/31/94	HEC		Added kOSCheckLine.  Updated to new AskForReplay prototype.
									Added gameOwnsDram game param.
		<39>	 7/30/94	HEC		Fixed stimulator test game loading cache bug. Use Mac's
									BlockMove instead of ByteCopy to flush the cache.
		<38>	 7/29/94	HEC		Dialog constant changed.
		<37>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<36>	 7/28/94	JOE		Call ShutDownSoundMgr() in PlayCurrentGame().
		<35>	 7/26/94	SAH		Added CloseSessionSynch to the game dispatcher.
		<34>	 7/26/94	HEC		Added HandleCallWaiting and fill in HandleGameError a bit.
									Neither is tested yet, but compile.
		<33>	 7/25/94	SAH		Keep track of how many games we've played. if we haven't played
									at least one (or some minimum number, and we die with an error,
									return that error to the server.
		<32>	 7/25/94	ADS		Added Readcontrollers
		<31>	 7/24/94	SAH		Lots of new gametalk functions.
		<30>	 7/19/94	JOE		fixed kGameNameType vs. kGamePatchType mixup
		<2