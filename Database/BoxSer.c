/*
	File:		BoxSer.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<66>	 8/29/94	ADS		Fixed that
		<65>	 8/29/94	ADS		Added recording of lastboxstate & boxState for crash records
		<64>	 8/26/94	BET		Fix a jeissboy in ClearNetErrors.
		<63>	 8/25/94	BET		Add new dual error record jizz.
		<62>	 8/25/94	ADS		Get rid of grungy ROM globals
		<61>	 8/25/94	HEC		Init dialNetworkAgain global at SoftInitialize.
		<60>	 8/24/94	HEC		Call waiting and keyboard options are now user-based.
		<59>	 8/23/94	BET		Add ClearNetErrors().
		<58>	 8/22/94	ADS		Add hidden box serial # calls
		<57>	 8/21/94	ADS		New lomem layout
		<56>	 8/21/94	JOE		Init Hue values to 0
		<55>	 8/21/94	KON		Added second local access number.
		<54>	 8/21/94	JBH		Made ROM DB constants in a table, to save space and time.
		<53>	 8/20/94	JOE		add Get/SetLocalUserCustomROMClutID
		<52>	 8/20/94	CMY		Added Get/Set AcceptChallengeOption.
		<51>	 8/19/94	HEC		Return card info in GetBoxXBANDCard if token exists also.
									Restored simulator code for testing with boxphonenumbers.
		<50>	 8/18/94	HEC		Fixed typo.
		<49>	 8/18/94	HEC		Pass a reason for using the problem token to UseBoxProblemToken.
		<48>	 8/18/94	JOE		SetGameValue routines now nuke any existing string if given a
									nil ptr or a ptr to an empty string
		<47>	 8/18/94	ADS		Use ASSERT to protect array lookups
		<46>	 8/18/94	ADS		Hide gBoxID globals
		<45>	 8/18/94	HEC		Init problem token to 1.
		<44>	 8/17/94	JOE		really set the Box Validation Token
		<43>	 8/17/94	JOE		Now the Game Value funcs deal with strings
		<42>	 8/14/94	ADS		Tuck away a copy of each box serial # in crash-record
		<41>	 8/13/94	KON		Removed test users 1-3. Made stimulator have default icon 1 so
									it doesn't ask us stupid questions all the time.
		<40>	 8/13/94	BET		Add GetNetErrors
		<39>	 8/12/94	JOE		Changes to Test User init: install ROM icon ID's the new way,
									set the box region & boxid to whatever Test User 0 has.
		<38>	 8/12/94	HEC		Utilize new kUsedSmartCardForConnectState box state.
		<37>	 8/11/94	JOE		ChangeUserPersonificationPart now wants a flag
		<36>	 8/10/94	BET		Added scriptID brokering vi phoneNumber structure.
		<35>	  8/7/94	ADS		SETOSSTATE()
		<34>	  8/7/94	JOE		added SetGameValue()
		<33>	  8/7/94	JOE		added GetGameValue() for the Versus screen
		<32>	  8/6/94	SAH		Clear wins and losses in ClearGameResults.
		<31>	  8/4/94	SAH		Get those damn servies!
		<30>	  8/4/94	SAH		Added new stuff to keep track of game errors.
		<29>	  8/3/94	JOE		Init user icon ref in lowmem
		<28>	  8/3/94	HEC		Added options accessor routines.
		<27>	  8/2/94	SAH		Flag gOSState when we init the box id globals.
		<26>	  8/1/94	HEC		Added lots of fun new routines for setting/getting box tokens,
									smart card stuff, etc.
		<25>	 7/27/94	KON		Add set and check network dial again flag to support 800 number.
		<24>	 7/27/94	SAH		Put all the test user id shit here.
		<23>	 7/20/94	DJ		added GetCompetitionResults
		<22>	 7/19/94	DJ		set and get of Hometown
		<21>	 7/16/94	KON		Help Shannon init the globals thoroughly.
		<20>	 7/15/94	SAH		New calls for game results.
		<19>	 7/11/94	SAH		Let the server set the box id. ROM buiild gets the box phone
									number from the ROM for simplicity.
		<18>	 7/10/94	SAH		More thoroughly init the box id globals.
		<17>	 7/10/94	HEC		Added DBSetConstants
		<16>	  7/7/94	HEC		Moved DBGetConstant and DBAddConstant from DB.c
		<14>	 8/28/56	SAH		Changed base phone number to local access number.
		<13>	  7/2/94	SAH		Added BoxID calls.
		<12>	  7/2/94	DJ		added game resultflags to _SetGameScores
		<11>	  7/1/94	HEC		Added SetBoxMaster function.
		<10>	 6/28/94	SAH		Made new box serial number structure.
		 <9>	 6/27/94	KON		Fix all references to SRAM game ID.
		 <8>	 6/27/94	KON		Added get and set curGameIDSRAM.
		 <7>	 6/20/94	KON		Add IsBoxMaster call.
		 <6>	 6/17/94	KON		Add game score stuff.
		 <5>	 6/14/94	KON		Add box state stuff.
		 <4>	 6/13/94	KON		