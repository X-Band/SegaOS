/*
	File:		MailEdit.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<35>	 8/27/94	CMY		Properly calculate the maximum size of mail during the
									preflight.
		<34>	 8/26/94	CMY		Auto-entry now works when the user has multiple players in the
									player list.
		<33>	 8/26/94	KON		Writing mail while registered is a warning rather than a yes/no
									dialog.
		<32>	 8/23/94	CMY		Fixed auto-entry of mail recipients from player list so that it
									works whenever the To: field is empty.
		<31>	 8/19/94	JBH		Made mail db preflight more accurate
		<30>	 8/15/94	JBH		Disable deferred dialogs to avoid graphics problems.
		<29>	 8/14/94	KON		Use kTheyString rather than init in-line.
		<28>	 8/14/94	KON		When sending two or more mails, the "They" string gets screwed
									up on the Sega. We now get "They" from the stringDB.
		<27>	  8/9/94	CMY		Don't require user to confirm "Cancel" if she hasn't typed
									anything into the mail.
		<26>	  8/8/94	KON		Remove ClearBackdrop.
		<25>	  8/4/94	CMY		Oops -- I was leaking a jizzler.
		<24>	  8/3/94	CMY		Revamped it for new UI look.
		<23>	  8/3/94	CMY		Fixed backgrounds. Changed buttons. Changed jizzle rect.
		<22>	 7/30/94	KON		Added cancel while entering mail dialog.
		<21>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<20>	 7/24/94	CMY		Fixed globals problem
		<20>	 7/24/94	CMY		Fixed a problem with static constants and added some jizzlers
		<19>	 7/22/94	CMY		Fixed reference to nonexistent background graphic.
		<18>	 7/15/94	SAH		Updated to new DBPreflight interface.
		<17>	 7/13/94	CMY		Preflight to determine if there's enough room for outgoing mail.
		<16>	 7/13/94	KON		Added preflight to warn user that they may be interrupted if box
									is registered with the network.
		<15>	 7/12/94	CMY		Zero out the initValue so returning to MailView screen doesn't
									die.
		<14>	 7/11/94	CMY		Made "Reply" work correctly. It now fills in the complete valid
									address of the recipient.
		<13>	  7/8/94	CMY		Start button scrolls through address book entries in New Mails
									screen.
		<12>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<11>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<10>	  7/7/94	HEC		#ifdef ROMDB
		 <9>	  7/5/94	CMY		Added error dialogs for empty address field, title, message.
		 <8>	  7/4/94	ADS		Icon ref is now replaced by whether or not         mail has been
									read.
		 <7>	  7/2/94	KON		Icon ref is now replaced by whether or not mail has been read.
		 <6>	  7/1/94	CMY		Gutted the sonofabitch and redid it to use the Keyboard entry
									manager.
		 <5>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <4>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		 <3>	  6/5/94	KON		Comment CreateNewMail out for Digital World since there's no
									room for the keyboards in SRAM.
		 <2>	  6/3/94	DJ		updating to revised DrawDBGraphic call
		 <1>	  6/2/94	CMY		first checked in

	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "vdp.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "SegaCluts.h"
#include "DBTypes.h"
#include "Errors.h"
#include "GraphicsDB.h"
#include "StringDB.h"
#include "TextUtls.h"
#include "Mail.h"
#include "SegaIn.h"
#include "SegaOS.h"
#include "Screens.h"
#include "Time.h"
#include "Events.h"
#include "Heaps.h"
#include "Backdrops.h"
#include "Keyboard.h"
#include "MungeCode.h"
#include "MailEditPriv.h"
#include "Utils.h"
#include "DialogMgr.h"
#include "PlayerDB.h"
#include "NetRegister.h"
#include "DeferredDialogMgr.h"

#define kLayoutJizzleRect 0

#define kLayoutToString 0
#define kLayoutTitleString 1

static long MailEditDispatch( short action, long command, long *refCon, ScreenParams *params );
static long MailEditPreflight( long initValue );
static void MailEditReset( long refCon );
static long MailEditSetup( ScreenParams *params );
static long MailEditComman