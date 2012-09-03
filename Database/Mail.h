/*
	File:		MailCntl.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<20>	 8/19/94	JBH		GetOutBoxTypeForCurrentUser made public
		<19>	  8/1/94	KON		Remove unix pad poop.
		<18>	 7/26/94	BET		ifdef around kTitleSize to avoid redefinition with RPC
		<17>	 7/24/94	DJ		padding on sega to match sun
		<16>	 7/19/94	DJ		mail serialNumber is a short now
		<15>	 7/18/94	KON		Bumped mail title size to 34.
		<14>	 7/16/94	KON		Bump the max in box entries to 10.
		<13>	 7/15/94	CMY		Added DeleteAllOutBoxMail
		<12>	 7/14/94	CMY		Added GetIndexOutBoxMail.
		<12>	 7/14/94	CMY		Add size field to GetIndexOutBoxMail
		<11>	 7/14/94	CMY		Added GetIndexOutBoxMail.
		<10>	 7/13/94	CMY		Added CountOutBoxEntries.
		 <9>	  7/3/94	KON		Add MarkMailItemRead function.
		 <8>	  7/2/94	KON		Change iconRef to flag saying whether the mail item has been
									read or not.
		 <7>	 6/30/94	SAH		Killed all the old out box db calls.
		 <6>	 6/20/94	KON		GetInBoxMailGraphic now takes a screen parameter.
		 <5>	 6/14/94	SAH		Managerized.
		 <4>	 6/13/94	KON		Add call to get correct mail icon depending on amount of mail.
		 <3>	 5/31/94	DJ		added #ifdef __SERVER__ and some comments about the to and from
									fields in a mail
		 <2>	 5/28/94	KON		Change the date to a long instead of a string.
		 <1>	 5/27/94	KON		first checked in

	To Do:
*/


#ifndef __Mail__
#define __Mail__

#ifndef __DataBase__
#include "DataBase.h"
#endif __DataBase__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifndef __UsrConfg__
#include "UsrConfg.h"
#endif __UsrConfg__

#ifndef	kTitleSize
#define	kTitleSize	34
#endif

#define kMaxInBoxEntries 10
#define kMaxOutBoxEntries 8

typedef struct
{
	userIdentification	from;
	userIdentification	to;
	short				serialNumber;
	long				date;
	char				hasThisMailBeenRead;
	char				title[kTitleSize];		//Null terminated string
	char				message[1];		//Null terminated string
} Mail;


#ifndef __SERVER__

short CountInBoxEntries( void ) = 
	CallDispatchedFunction( kCountInBoxEntries );

short CountOutBoxEntries( void ) = 
	CallDispatchedFunction( kCountOutBoxEntries );

void AddMailToOutBox( Mail *theMail ) = 
	CallDispatchedFunction( kAddMailToOutBox );

void AddMailToInBox( Mail *theMail ) = 
	CallDispatchedFunction( kAddMailToInBox );

void RemoveMailFromInBox( short index ) = 
	CallDispatchedFunction( kRemoveMailFromInBox );

void DeleteAllOutBoxMail( void ) =
	CallDispatchedFunction( kDeleteAllOutBoxMail );

Mail *GetIndexInBoxMail( short index ) = 
	CallDispatchedFunction( kGetIndexInBoxMail );

Mail *GetIndexOutBoxMail( short index, long *size ) = 
	CallDispatchedFunction( kGetIndexOutBoxMail );

DBID GetInBoxGraphicID( short whichScreen ) = 
	CallDispatchedFunction( kGetInBoxGraphicID );

void MarkMailItemRead( short index ) =
	CallDispatchedFunction( kMarkMailItemRead );

DBType GetOutBoxTypeForCurrentUser( void ) =
	CallDispatchedFunction( kGetOutBoxTypeForCurrentUser );

#endif __SERVER__

#endif __Mail__