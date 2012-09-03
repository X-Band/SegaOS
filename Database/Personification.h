/*
	File:		Personification.h

	Contains:	xxx put contents here xxx

	Written by: Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 8/21/94	DJ		added kPersonificationRAMIcon
		<25>	 8/20/94	JOE		added kPersonificationROMClutID
		<24>	 8/12/94	JOE		Support fixed custom icon stuff
		<23>	 8/11/94	JOE		For box<->server pers exchanges, only send/recv parts that
									changed
		<22>	  8/4/94	HEC		JOE switched kCreatePersonificationBundle and
									kParsePersonificationBundle in prototypes.
		<21>	  8/3/94	JOE		nuke OpponentVerificationTag
		<20>	  8/3/94	JOE		icon tweak
		<19>	  8/3/94	JOE		changed for personification mgr rewrite
		<18>	  8/2/94	HEC		Personification bundles.
									Added ReceivePersonficationBundle, ParsePersonificationBundle,
									and CreatePersonificationBundle
		<17>	 7/21/94	BET		add #ifdef __SERVER__
		<16>	 7/21/94	BET		Fix caps on DataBase.h
		<15>	 7/18/94	DJ		added opponentVerificationTag for peer verification
		<14>	 7/18/94	KON		Remove hometown from PersonificationRec since it's now part of
									the userIdentification.
		<13>	 7/18/94	KON		Take icon refs out of personification structure because they are
									part of the userIdentification now.
		<12>	 7/12/94	JOE		update for new server messages, etc.
		<11>	 7/11/94	JOE		long-lost changes
		<10>	 7/11/94	CMY		Changed phantom user numbering.
		 <9>	  7/6/94	JOE		updates for ghost personifications
		 <8>	  7/6/94	JOE		more coding in my sleep.
		 <7>	  7/5/94	JOE		noted that taunt & about string selectors return an xyString
		 <6>	  7/5/94	JOE		fixed town/bday, added password support
		 <5>	  7/5/94	JOE		updated for new personification model
		 <4>	 6/15/94	KON		Add personification icon ID.
		 <3>	 6/14/94	SAH		Managerized.
		 <2>	  6/4/94	KON		Change kAboutText to kPersonificationAboutText.
		 <1>	  6/4/94	KON		first checked in

	To Do:
*/

#ifndef __Personification__
#define __Personification__

#ifndef __Database__
#include "DataBase.h"
#endif __Database__

#ifndef __UsrConfg__
#include "UsrConfg.h"
#endif __UsrConfg__

#ifndef __SegaIcons__
#include "SegaIcons.h"
#endif __SegaIcons__

#ifndef __SegaIcons__
#include "Animators.h"
#endif __SegaIcons__

#ifndef __Messages__
#include "Messages.h"
#endif __Messages__


#define		kOpponentID		0xFF

#define		kNumUsers		4

typedef struct PersonificationSetupRec
{
	userIdentification		*userIDData;			// userID, Town, Name, ROMIconID
	Password				pwData;					// not used for peer-peer
	Ptr						TauntTextData;
	Ptr						InfoTextData;
	RAMIconBitMapPtr		RAICData;
	unsigned long			RAICDataSize;
	IconSetup				IDSCData;
} PersonificationSetupRec;



// These are the indexes for the Get/Remove personification part utility routines
//  They are also flags are used to mark changed Personification Parts.
//  They go into the userValidated fields of the BoxIdentification struct.

#define	kPersonificationHandle		0x01	
#define	kPersonificationPassword	0x02			
#define	kPersonificationROMIconID	0x04			
#define	kPersonificationTauntText	0x08			// C string Ptr
#define	kPersonificationAboutText	0x10			// C string Ptr
#define	kPersonificationTown		0x20	
#define	kPersonificationROMClutID	0x40			// if 0xFF, use built-in clut
#define	kPersonificationRAMIcon		0x80

#define	kAllPersonificationParts	0xFF


#ifdef __SERVER__
// do something so the server can use the file

#else

//
// for local users
//

void	ChangeUserPersonificationPart( DBID theID, short index, void *data, Boolean validated ) =
	CallDispatchedFunction( kChangeUserPersonificationPart );

void *	GetPersonificationPart( DBID theID, short index ) =
	CallDispatchedFunction( kGetPersonificationPart );


//
// for local users or opponents
//

void	DisposePersonificationSetup( PersonificationSetupRec *OPers ) =
	CallDispatchedFunction( kDisposePersonificationSetup );


//
// for opponent
//

void 	InstallOpponentPersonification( PersonificationSetupRec *data ) =
	CallDispatchedFunction( kInstallOpponentPersonification );


//
// Server assist routines
//

MessErr GetPersonificationFromWire( unsigned char curUserID ) =
	CallDispatchedFunction( kGetPersonificationFromWire );

void 	PutPersonificationOnWire( unsigned char theUserID ) =
	CallDispatchedFunction( kPutPersonificationOnWire );


//
// Peer-Peer Personification Exchange
//

OSErr	ReceivePersonficationBundle( Ptr buffer, long size ) =
	CallDispatchedFunction( kReceivePersonficationBundle );

Ptr		CreatePersonificationBundle( DBID theID ) =
	CallDispatchedFunction( kCreatePersonificationBundle );

void	ParsePersonificationBundle( Ptr buffer, PersonificationSetupRec *OPers ) =
	CallDispatchedFunction( kParsePersonificationBundle );



#endif //__SERVER__
#endif __Personification__
