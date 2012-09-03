#ifdef	ROMDB
#undef	SIMULATOR
#endif

// Define either of the following to force peer-to-peer connection
// If setting master, make sure you set the challenger's CSTR resource string.
// If slave, trigger peer listen by going into the find opponent screen.
//#define FORCE_MASTER 1
//#define FORCE_SLAVE 1

/*
	File:		FindOpponent.c

	Contains:	Screen handler for Connect-To-Catapult screen

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

	   <183>	 9/18/94	ADS		1.0 bug:  ConnectRetry when POPEN fails too
	   <182>	 8/28/94	KON		Set the backdrop to the main backdrop when showing dialogs over
									the (hidden) news.
	   <181>	 8/27/94	KON		Differentiate between X25 chat sendQ items and 800 number sendQ
									items.
	   <180>	 8/26/94	KON		Eliminate flashing due to redrawing in the news.
	   <179>	 8/26/94	ADS		Leftover debug strings in non-debug builds!
	   <178>	 8/26/94	CMY		Erase the server strings when erasing the XBAND logo so they're
									not sitting alone on the screen.
	   <177>	 8/26/94	HEC		Move peer-to-peer #defines to top. Added pad bytes to front of
									peer-to-peer phone number.
	   <176>	 8/26/94	KON		Hide the newspaper text when bringing up server dialogs.
	   <175>	 8/25/94	BET		Add multiple NetErrorRecords for X25 and 800.
	   <174>	 8/25/94	KON		Remove calls to ClearSendQ, MarkAddressBookUnchanged, and
									ClearMailQ since those are done as part of a server message now.
	   <173>	 8/25/94	KON		ServerConnectNewsDone no longer bracketed by #ifdef USEDISK.
	   <172>	 8/25/94	HEC		#define SHOW_POPEN_ERRS in DebugSelectors.h to enable debug
									display of POpen errors on server connect.  Also don't redo
									preflight if redialing server.
	   <171>	 8/24/94	BET		Remove DEBUGCHATSCRIPT #ifdef's
	   <170>	 8/24/94	SGP		Fixed peer-to-peer includes.
	   <169>	 8/23/94	DJ		added debugging display for sendq debugchatscript
	   <168>	 8/23/94	KON		Add another dialog when the X25 fails, use the dialnetwork again
									mechanism, rather than a goto to retry.
	   <167>	 8/22/94	DJ		more of that
	   <166>	 8/22/94	DJ		cleaned up DEBUGCHATSCRIPT to go on sendq, not mail
	   <165>	 8/22/94	ADS		yet another USEDISK tweak
	   <164>	 8/21/94	KON		Added Are you sure the number your dialing from is xxx if the
									X.25 connection fails after all the retries.
	   <163>	 8/21/94	HEC		Include "opponent.h"
	   <162>	 8/21/94	HEC		ConnectToPeer takes new default values.
	   <161>	 8/21/94	KON		Use second local access number on every other retry.
	   <160>	 8/21/94	KON		Wrapper FindOpponent setup with get and set current user incase
									there is an error and the server has the current user set to
									something else.
	   <159>	 8/20/94	DJ		moved the mail jizzler to after POpen for chat script debugging
	   <158>	 8/20/94	BET		Null terminate subject line for chat debug.
	   <157>	 8/20/94	ADS		Fix DiskTransport (yet again)
	   <156>	 8/19/94	DJ		added send of phys layer chatscript debug stuff by mail.
	   <155>	 8/18/94	HEC		Pass reason to UseBoxProblemToken.
	   <154>	 8/18/94	DJ		fixed error dialog for kScriptTimeoutErr
	   <153>	 8/18/94	ADS		PhoneNumbers.h is obsolete
	   <152>	 8/18/94	ATM		(HEC) NetRegister should have been NetRegisterDone...
	   <151>	 8/18/94	DJ		fixed error dialogs when line drops during a server connection
	   <150>	 8/18/94	KON		If there's a network error, call SetBoxMaster( false ).
	   <149>	 8/18/94	HEC		Now debit smartcard in preflight.  Made sure error cases in
									setup call UseProblemToken.
	   <148>	 8/17/94	HEC		Moving clearing of local access number to NetRegister.c
	   <147>	 8/17/94	HEC		Move display of deferred dialogs to after we setup slave PListen
									so we can jump out of dialogs when ring is detected.
	   <146>	 8/15/94	KON		Do all time critical box->server comm in the first packet.
	   <145>	 8/14/94	JBH		Added FlushUserEvents before news comes up.
	   <144>	 8/14/94	HEC		Forgot to make it compile.  Dah....
	   <143>	 8/14/94	HEC		Simplify slave peer listen by calling ConnectToPeer.
	   <142>	 8/13/94	HEC		Always disable call waiting during server connect.
	   <141>	 8/13/94	BET		Added SendNetErrors
	   <140>	 8/12/94	DJ		turned on personifuckation
	   <139>	 8/12/94	KON		Add DebugSelectors to support USEDISK #define.
	   <138>	 8/12/94	HEC		SmartCard changes.
	   <137>	 8/11/94	JBH		Made status messages appear during maze.
	   <136>	 8/11/94	JBH		Moved XBandLogo and sparks into sprite manager
	   <135>	 8/11/94	JBH		Changed to new XBand logo. Fixed text drawing in wrong text
									gdevice for status messages. Keep TheAnimator data persistent
									until we trash TheAnimator.
	   <134>	 8/10/94	BET		Added scriptID brokering vi phoneNumber structure.
	   <133>	  8/9/94	CMY		Added a flag to Hide/Show Backdrop.
	   <132>	  8/9/94	KON		Hide background if there are dialogs to display after server
									connect to eliminate flicker in news.
	   <131>	  8/9/94	HEC		Check for kHandshakeErr during connect.
	   <130>	  8/9/94	KON		Hide backdrops while showing server connect done dialogs to
									eliminate flicker.
	   <129>	  8/9/94	CMY		Move server connect text messages up 2 pixels. (Really)
	   <128>	  8/8/94	SAH		Disable server talk for FORCE_SLAVE so that we can get some of
									those bytes back.
	   <127>	  8/6/94	JBH		Added a server-controlled text message to display to user during
									connect.
	   <126>	  8/6/94	DJ		more X25SUCKS
	   <125>	  8/6/94	DJ		more X25SUCKS
	   <124>	  8/6/94	DJ		X25 SUCKS increases timeout vals and retry count.
	   <123>	  8/5/94	DJ		added msSendInvalidPersID (commented out for now)
	   <122>	  8/4/94	DJ		uncommented game errors (user server 71 or greater)
	   <121>	  8/4/94	SAH		Send game errors to the server (commented out for now).
	   <120>	  8/3/94	HEC		Disable call waiting only when user chooses this in options
									screen.  Clear localaccessphone number when user says he moved
									in options screen.
	   <119>	  8/3/94	DJ		added a dialog when processmsg detects an error
	   <118>	  8/2/94	BET		Add 800 connect checking
	   <117>	  8/1/94	KON		Call news idle proc four times per second rather than 60.
	   <116>	  8/1/94	KON		Remove GetData dependency.
	   <115>	  8/1/94	HEC		Disable call waiting option. DebitSmartCard param.
	   <114>	 7/31/94	KON		If there's no news, don't spin waiting for a button press.
	   <113>	 7/29/94	KON		Add decompressor options to SetBackdropID.
	   <112>	 7/29/94	DJ		added even more checkerror
	   <111>	 7/29/94	DJ		added better clear/check of network errors (there were some
									missing PCheckErrors)
	   <110>	 7/29/94	KON		Add decompressor options to DrawGraphic, and make sparks go
									behind text and graphics.
	   <109>	 7/29/94	KON		Add waiting for answer string to show progress during network
									connnect.
	   <108>	 7/28/94	JBH		Resets news countdown timer value to a reasonable value, in case
									the server neglects to do this.
	   <107>	 7/28/94	BET		Add SetRemoteModemTryingToConnectState(false) call before
									calling PListen on peer connect to avoid false calling
									indication.
	   <106>	 7/27/94	KON		Add network dial again if flag is set. Needed to support 800
									number.
	   <105>	 7/27/94	KON		Add new logo, free up logo graphic before drawing news.
	   <104>	 7/26/94	DJ		added TSetTransportHold to optimize sequential packets
	   <103>	 7/26/94	JBH		Added secret maze to findopponent sequence.
	   <102>	 7/25/94	DJ		added CONNECT_FOREVER define for server connect testing
	   <101>	 7/25/94	KON		Redo the return icon so it's handled automagically by news
									server connect done.
	   <100>	 7/25/94	KON		Redo the way globals work, add return icon to news.
		<99>	 7/24/94	KON		Don't call PListen if the net didn't register us for some
									reason. Removed refereces to mail only connect and just check
									isboxnetregistered instead.
		<98>	 7/24/94	JBH		Added sparks to the server connect screen.
		<97>	 7/23/94	SAH		fuck fuck
		<96>	 7/23/94	SAH		fuck
		<95>	 7/23/94	SAH		Added an inline assembly glue hack for the opponent phone number
									for the master if FORCE_MASTER.
		<94>	 7/23/94	HEC		Added compile switches to build peer-to-peer only versions.
									Define FORCE_MASTER or FORCE_SLAVE for fun, fun, fun!
		<93>	 7/23/94	KON		Do main event loop dialogs during news viewing if slave so the
									network has timed out message comes up.
		<92>	 7/22/94	HEC		checking smart card before asking user if he wants to reregister
									in preflight.
		<91>	 7/21/94	KON		Fix text colors.
		<90>	 7/21/94	KON		Check result of PListenAsync and if it fails signal a severe
									error message.
		<89>	 7/20/94	DJ		Sending game results
		<88>	 7/17/94	KON		Mark address book as updated on successful server connect.
		<87>	 7/17/94	DJ		Send msCreditDebitInfo and msBoxType
		<86>	 7/17/94	HEC		Send msSendCreditToken.
		<85>	 7/17/94	HEC		Send debit card message in SendOutgoingMessages.
		<84>	 7/17/94	HEC		Debit smart cards during network connect.
		<83>	 7/15/94	HEC		New flag to POpen to turn off call waiting when dialing the
									server. Probably don't want to do this if we know call waiting
									doesn't exist for the user.
		<82>	 7/15/94	KON		Make mail connect show you the news but not wait for a call and
									certainly not dial out to a nil challenger!
		<81>	 7/15/94	CMY		Delete outgoing mail after successful server connect
		<80>	 7/14/94	CMY		Don't clobber the current screen's state when a deeper screen's
									preflight fails.
		<79>	 7/14/94	ADS		Minor tweak for USEDISK builds
		<78>	 7/14/94	KON		Only throw away pattern mems on news screen kill.
		<77>	 7/14/94	KON		Check errors on TOpen and add them to the POpen retry mayhem.
		<76>	 7/13/94	KON		If box is already registered, deregister the box and close the
									physical layer.
		<75>	 7/13/94	DJ		fixed peer connect by using boxstate
		<74>	 7/13/94	DJ		moved connection retry count check into if(err)
		<73>	 7/13/94	KON		Removed reference to kWaitingForGame box state and use
									IsBoxNetRegistered instead.
		<72>	 7/12/94	KON		Remove you are registered dialog which is now a server
									responsibility.
		<71>	 7/11/94	KON		Remove competative request since all connects are done via a
									challenge request now. Also removed NetRegister since this is
									done via a server message.
		<70>	 7/11/94	DJ		removed msCompetitiveChallenge
		<69>	 7/10/94	HEC		Add check for smart credit card inserted during connection to
									server.
		<68>	  7/8/94	SAH		Removed TimeProc hack and pass real function ptr to CheckJTRef
									thing.
		<67>	  7/8/94	ADS		Fix interaction with "connect yes/no" dialog
		<66>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<65>	  7/8/94	ADS		Add connect-disable-password logic
		<64>	  7/7/94	KON		Make server time outs and retry constants part of the
									DBConstants database.
		<63>	  7/7/94	HEC		Removed stupid DelayTicks in connect logic.
		<62>	  7/7/94	SAH		Made the CheckError a TCheckError until CheckError is
									managerized.
		<61>	  7/7/94	KON		Remove extra NetRegisterDone and change Error_Mesg to Warning on
									unknown network error.
		<60>	  7/6/94	HEC		Fix a lea(a5) think c fuckup for the NewsTimeProc with some
									really fucked up assembly. Blame Shannon for this one.
		<59>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<58>	  7/5/94	DJ		added send of addrbookvalidation msg
		<57>	  7/5/94	HEC		Removed useless DelayTicks in server connect
									(FindOpponentSetup).
		<56>	  7/5/94	BET		Move PCheckError someplace it gets hit all the time.
		<55>	  7/5/94	DJ		added PCheckError after POpen fails
		<54>	  7/5/94	KON		Net register only after successful server connect. Clear trash
									when a long server string draws before a short one (only happens
									in some error cases).
		<53>	  7/4/94	ADS		New interface for screen control
		<52>	  7/4/94	KON		Move news control idle to a time manager routine during server
									connect so we get better controller reading resolution than each
									server opcode. Currently the network read procs are all
									synchronous and don't give