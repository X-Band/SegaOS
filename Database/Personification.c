/*
	File:		Personification.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<48>	 8/27/94	CMY		Preflight RAM DB add when adding custom RAM icon during
									peer-to-peer.
		<47>	 8/21/94	DJ		added kPersonificationRAMIcon
		<46>	 8/20/94	JOE		added support for kPersonificationROMClutID
		<45>	 8/20/94	DJ		putonwire doesn't send the RAM icon or handle... getfromwire
									doesn't receive the handle
		<44>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<43>	 8/19/94	JOE		When installing opponent personification, nuke any old RAM icon
									we might have for him
		<42>	 8/18/94	JOE		User BoxSer routines to Get/Set game value strings
		<41>	 8/18/94	JOE		Now the peers swap game value strings
		<40>	 8/15/94	JOE		Now the server is responsible for putting up the "your shit
									changed" dialogs
		<39>	 8/12/94	JOE		update local user's cached user info whenever a part of her
									personification is changed
		<38>	 8/12/94	JOE		support fixed custom icon stuff
		<37>	 8/11/94	DJ		added sending of message opcode for putonwire
		<36>	 8/11/94	JOE		For box<->server pers exchanges, only send/recv parts that
									changed
		<35>	 8/10/94	DJ		fixed sending of shorts as chars
		<34>	  8/9/94	SAH		Transmit the box id and user id for peer to peer. Get the
									current opponent personification id from
									GetOpponentPersonificationID (and set it too).
		<33>	  8/4/94	SAH		Fixed some odd byte alignment crappies for the sega.
		<32>	  8/4/94	JOE		Thanks, Ted.  Fixed a bug in the server info exchange code,
									added dialogs that pop up if the server edits your taunt or user
									info.
		<31>	  8/4/94	HEC		(and SAH) Fuck, Joe!  Fixed your stinking poo yet again!  You
									get the award for the most bugs per file, ever!
		<30>	  8/3/94	JOE		let the user know (via a dialog) if the server decided not edit
									the requested handle
		<29>	  8/3/94	JOE		icon tweak
		<28>	  8/3/94	JOE		pretty much rewrote
		<27>	  8/3/94	HEC		Wittled down personifuckation bundle size a wee tad.
		<26>	  8/2/94	HEC		Must send empty taunt/info strings if none available in the
									database.
		<25>	  8/2/94	HEC		Fixed nil string case of taunts/info when bundling/unbundling
									crap.
		<24>	  8/2/94	HEC		Personification bundle crap.
		<23>	  8/1/94	JOE		Bitch!
		<22>	 7/31/94	KON		Use writeable string calls rather than hitting the database
									directly.
		<21>	 7/21/94	SAH		(JOE). Oh no, my eye!
		<20>	 7/18/94	DJ		added opponent verification
		<19>	 7/18/94	JOE		Fixed hometown changes.
		<18>	 7/18/94	KON		Commented out references to hometown (since that's now part of
									the userIdentification). Fix me, JOE!
		<17>	 7/18/94	KON		Update for icons being in the userIdentification structure
									rather than the personification struct.
		<16>	 7/18/94	ADS		Dispatcher cleanup
		<15>	 7/13/94	ADS		Vectorize EVERYTHING
		<14>	 7/12/94	JOE		was DBAddItem()'ing a non-memory mgr block (thanks Shannon!)
		<13>	 7/12/94	JOE		update for server messages, etc.
		<12>	 7/11/94	JOE		long-lost changes
		<11>	 7/11/94	CMY		Changed phantom user numbering.
		<10>	  7/6/94	JOE		moved taunt & info to WSTRs
		 <9>	  7/6/94	JOE		updated while half asleep.  hope it's ok.
		 <8>	  7/5/94	JOE		tested, fixed minor probs
		 <7>	  7/5/94	JOE		fixed town/bday, added password support
		 <6>	  7/5/94	JOE		hack until the Town/Bday stuff is in there
		 <5>	  7/5/94	JOE		rewrote for updated personification model
		 <4>	 6/30/94	SAH		Removed database purge values.
		 <3>	 6/28/94	SAH		Updated to new user id/box id stuff.
		 <2>	 6/14/94	SAH		Managerized.
		 <1>	  6/4/94	KON		first checked in

	To Do:
*/

#include "Personification.h"
#include "SegaOS.h"
#include "DataBase.h"
#include "Errors.h"
#include "DBTypes.h"
#include "Heaps.h"
#include "SegaBM.h"
#include "SegaText.h"
#include "GetData.h"
#include "TransportLayer.h"
#include "PersonificationPriv.h"
#include "Opponent.h"
#include "StringDB.h"
#include "Utils.h"
#include "BoxSer.h"
#include "DialogMgr.h"

void 	_ChangeUserPersonificationPart( DBID theID, short index, void *data, Boolean validated );
void *	_GetPersonificationPart( DBID theID, short index );						
void 	_InstallOpponentPersonification( PersonificationSetupRec *data );

void 	_PutPersonificationOnWire( unsigned char theUserID );
MessErr _GetPersonificationFromWire( unsigned char curUserID );
void 	_DisposePersonificationSetup( PersonificationSetupRec *OPers );

OSErr	_ReceivePersonficationBundle( Ptr buffer, long size );
Ptr		_CreatePersonificationBundle( DBID theID );
void	_ParsePersonificationBundle( Ptr buffer, PersonificationSetupRec *OPers );



long
_PerfornicationControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* install our selectors */
			SetDispatchedFunction ( kChangeUserPersonificationPart,		kPerfornicationManager,	_ChangeUserPersonificationPart );
			SetDispatchedFunction ( kGetPersonificationPart,			kPerfornicationManager,	_GetPersonificationPart );
			SetDispatchedFunction ( kInstallOpponentPersonification,	kPerfornicationManager,	_InstallOpponentPersonification );

			SetDispatchedFunction ( kGetPersonificationFromWire,		kPerfornicationManager,	_GetPersonificationFromWire );
			SetDispatchedFunction ( kPutPersonificationOnWire,			kPerfornicationManager,	_PutPersonificationOnWire );

			SetDispatchedFunction ( kDisposePersonificationSetup,		kPerfornicationManager,	_DisposePersonificationSetup );

			SetDispatchedFunction ( kReceivePersonficationBundle,		kPerfornicationManager,	_ReceivePersonficationBundle );
			SetDispatchedFunction ( kParsePersonificationBundle,		kPerfornicationManager,	_ParsePersonificationBundle );
			SetDispatchedFunction ( kCreatePersonificationBundle,		kPerfornicationManager,	_CreatePersonificationBundle );
			break;
		
		case kSoftInialize:
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}




//
// Can only change local user personification parts
//
// Only Server-initiated changes should call this with validated == true
//

void _ChangeUserPersonificationPart( DBID theID, short index, void *data, Boolean validated )
{
	if ( theID < kOpponentRangeStart )			// local user only
	{
		switch ( index )
		{
			case kPersonificationHandle:
						SetLocalUserName ( theID, data );
						break;
						
			case kPersonificationPassword:
						SetLocalUserPassword ( theID, data );
						break;
						
			case kPersonificationTown:	
						SetBoxHometown ( data );
						break;
						
			case kPersonificationROMIconID:
						SetLocalUserROMIconID ( theID, *(DBID *)data );
						break;
						
			case kPersonificationTauntText:	
						SetLocalUserTaunt ( theID, data );
						break;
						
			case kPersonificationAboutText:		
						SetLocalUserInfo ( theID, data );
						break;
					
			case kPersonificationROMClutID:
						SetLocalUserCustomROMClutID( theID, *(DBID *)data );
						break;
			
			case kPersonificationRAMIcon:
						// don't have to do anything cuz is loaded elsewhere
						break;
			
			default:
						ERROR_MESG("Unknown personificationPart");
						break;
		}
		
		if ( !validated )
			InvalidateUserPersonificationParts( theID, index );		// server needs to check out his shit
			else
				ValidateUserPersonificationParts( theID, index );
				
		if ( theID == GetCurUserID() )		// if changes are for the current user, make them take effect immediately
			SetCurUserID( theID );
	}
}



void *_GetPersonificationPart( DBID theID, short index )
{
	if ( theID < kOpponentRangeStart )								// local user or opponent?
	{
		switch ( index )
		{
			case kPersonificationHandle:	
						return GetLocalUserName ( theID );
						break;
						
			case kPersonificationPassword:
						return GetLocalUserPassword ( theID );
						break;
						
			case kPersonificationTown:
						return GetBoxHometown();
						break;
						
			case kPersonificationROMIconID:								
						return (void *)GetLocalUserROMIconID ( theID );		// note - returns a DBID
						break;
						
			case kPersonificationTauntText:	
						return GetLocalUserTaunt ( theID );
						break;
						
			case kPersonificationAboutText:		
						return GetLocalUserInfo ( theID );
						break;

			case kPersonificationROMClutID:
						return (void *)GetLocalUserCustomROMClutID ( theID );	// note - returns a DBID
						break;
			
			case kPersonificationRAMIcon:
						return NULL;		/// you get the custom icon some other way -dj
						break;
			
			default:
						ERROR_MESG("Unknown personificationPart");
						break;
		}
	}
	else	
		return ( 0 );
}



void _InstallOpponentPersonification( PersonificationSetupRec *PSR )
{	
DBID	ramIconID;

	SetWriteableString( kOpponentTaunt, PSR->TauntTextData );
	SetWriteableString( kOpponentInfo, PSR->InfoTextData );
						
	NukePlayerRAMIcon( PSR->userIDData );			/* Nuke any previous RAM icon we might have for this guy */
						
	if ( PSR->RAICData )							/* does he have a RAM icon? */
	{
		ramIconID = DBGetUniqueID( kRAMIconType );
		
		if ( DBPreflight( kRAMIconType, ramIconID, PSR->RAICDataSize, kPreflightLight ) == noErr )
		{
			DBAddItemPtrSize( kRAMIconType, ramIconID, PSR->RAICData, PSR->RAICDataSize );
		
			if ( (PSR->IDSCData).frameDelay != 0 )
				DBAddItemPtrSize( kIconDescType, ramIconID, &(PSR->IDSCData).frameDelay, sizeof(IconSetup) );
		}
	}

	SetCurOpponentIdentification( PSR->userIDData );
}




void _DisposePersonificationSetup( PersonificationSetupRec *OPers )
{
	DisposeMemory( (Ptr)OPers->userIDData );
	DisposeMemory( OPers->TauntTextData );
	DisposeMemory( OPers->InfoTextData );
	DisposeMemory( OPers->RAICData );
}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Utility routines for xferring personification recs
//


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Server <-> Box Personification Xfer

//
// Retrieve & install Server-initiated User Personification changes.
//
// The info exchanged looks like this:
//
//									    Taunt	    Info
//		[flags]< [Handle] [Password] [size:Text] [size:Text] [ROMIconID][size:RAIC][IDSC] [ROMClutID] >
//				    1         2           3           4                   5					   6
//
//	The data is for the specified user.  The flags field (a char) identifies which one(s) of the 
//	 personification field(s) 1-6 will be sent down the wire.
//
//	If the specified user has no invalid parts, just 0x00 (the flags) is sent.
//
// For the icons:
// If the sender doesn't have a Custom Icon, he sends 0 for the RAIC size, then no more.
//
// The IDSC is just the framerate (a short) for an animated icon.  If the sender's custom
//  icon is static, 0 is sent.
//
// string sizes are shorts.  icon size is a short.  the idsc is a short.
// the flags field is an unsigned char.
//

MessErr _GetPersonificationFromWire( unsigned char curUserID )
{
MessErr				result;
short				theSize;
unsigned char		flags;
UserName			handle;
Password			pw;
DBID				ROMIconID;
DBID				ROMClutID;
DBID				ramID;
RAMIconBitMapPtr	ramIconData;
IconSetup			ramIconIDSC;
char				*string;
userIdentification	userIdent;

	string = 0;
	result = 0;


// get [flags]

	GetDataSync( sizeof(unsigned char), (Ptr)&flags );
	if( GetDataError() )
		goto abort;


	if ( flags == 0 )			// no flags, he's clean
	{
		ValidateUserPersonificationParts( curUserID, kAllPersonificationParts );
		return( 0 );
	}
	
	
// get [Password]

	if ( flags & kPersonificationPassword )
	{
		GetDataSync( sizeof(Password), (Ptr)&pw );
		if( GetDataError() )
			goto abort;
		ChangeUserPersonificationPart( curUserID, kPersonificationPassword, &pw, true );
	}


// get [size:Taunt]

	if ( flags & kPersonificationTauntText )
	{
		GetDataSync( sizeof(short), (Ptr)&theSize );
		if( GetDataError() )
			goto abort;

		string = NewMemory( kTemp, theSize );
		
		GetDataSync( theSize, string );
		if( GetDataError() )
			goto abort;

		ChangeUserPersonificationPart( curUserID, kPersonificationTauntText, string, true );
		
		DisposeMemory( string );
	}
	
	
// get [size:Info]

	if ( flags & kPersonificationAboutText )
	{
		GetDataSync( sizeof(short), (Ptr)&theSize );
		if( GetDataError() )
			goto abort;

		string = NewMemory( kTemp, theSize );
		GetDataSync( theSize, string );
		if( GetDataError() )
			goto abort;

		ChangeUserPersonificationPart( curUserID, kPersonificationAboutText, string, true );
		
		DisposeMemory( string );
	}
	

// get [ROMIconID]
// get [size:RAIC][IDSC] (or 0 if there's no custom icon)

	if ( flags & kPersonificationROMIconID )
	{
		GetDataSync( sizeof(DBID), (Ptr)&(ROMIconID) );					// get ROM ID
		if( GetDataError() )
			goto abort;

		ChangeUserPersonificationPart( curUserID, kPersonificationROMIconID, (Ptr)&ROMIconID, true );

	}
	
	if ( flags & kPersonificationRAMIcon)
	{
	
		GetDataSync( sizeof(short), (Ptr)&theSize );
		if( GetDataError() )
			goto abort;

		if ( theSize  )													// did they get a RAM icon?
		{
			FillInUserIdentification( curUserID, &userIdent );			// trash any old one
			NukePlayerRAMIcon( &userIdent );
			
			ramIconData = NewMemory( kTemp, theSize );					// get mem for the icon
		
			GetDataSync( theSize, (Ptr)ramIconData );					// suck the icon data off the wire
			if( GetDataError() )
				goto abort;

			GetDataSync( sizeof(IconSetup), (Ptr)&ramIconIDSC );		// get the framerate for the icon, in case it's animated
			if( GetDataError() )
				goto abort;

			ramID = DBGetUniqueID( kRAMIconType );						// try to add the icon to the DB

			if ( DBPreflight( kRAMIconType, ramID, theSize, kPreflightLight ) == noErr )
			{
				DBAddItemPtrSize( kRAMIconType, ramID, (Ptr)ramIconData, theSize );
				if ( ramIconIDSC.frameDelay != 0 )
					DBAddItemPtrSize( kIconDescType, ramID, (Ptr)&ramIconIDSC, sizeof(IconSetup) );
			}
				
			DisposeMemory( ramIconData );
		}
	}
	

// get [ROMClutID]

	if ( flags & kPersonificationROMClutID )
	{
		GetDataSync( sizeof(DBID), (Ptr)&(ROMClutID) );					// get ROM Clut ID
		if( GetDataError() )
			goto abort;
		ChangeUserPersonificationPart( curUserID, kPersonificationROMClutID, (Ptr)&ROMClutID, true );
	}
	
	return result;

abort:
	result = kFatalStreamError;
	if ( string )
		DisposeMemory( string );
	return result;
}



//
// Send Server Invalidated User Personification parts.
//
// The info exchanged looks like this:
//
//									    Taunt	    Info
//		[flags]< [Handle] [Password] [size:Text] [size:Text] [ROMIconID] [ROMClutID] >
//				    1         2           3           4           5			  6
//
//	The data sent is for the specified user.  The flags field (a char) identifies which one(s) of the 
//	 personification field(s) 1-6 will be sent down the wire.
//
// For the icons:
// The box never sends the server a custom icon.
//
// string sizes are shorts.  icon size is a short.  the idsc is a short.
// the flags field is an unsigned char.
//

void _PutPersonificationOnWire( unsigned char theUserID )
{
unsigned char		flags;
char				*shit;
unsigned short		theSize;
DBID				ROMIconID;
DBID				ROMClutID;
messOut				opCode = msSendInvalidPers;


	TWriteData( sizeof(messOut), (Ptr)&opCode, true );	// send the message opcode.


// send [flags]

	flags = GetValidationFlags( theUserID );
	TWriteData( sizeof(unsigned char), (Ptr)&flags, true );


// send [Password]

	if ( flags & kPersonificationPassword )
		TWriteData( sizeof(Password), GetPersonificationPart( theUserID, kPersonificationPassword ), true );


// send [size:Taunt]

	if ( flags & kPersonificationTauntText )
	{
		shit = GetPersonificationPart( theUserID, kPersonificationTauntText );
		theSize = GetStringLength( shit ) + 1;
		TWriteData( sizeof(short), (Ptr)&theSize, true );
		TWriteData( (long)theSize, shit, true );
	}


// send [size:Info]

	if ( flags & kPersonificationAboutText )
	{
		shit = GetPersonificationPart( theUserID, kPersonificationAboutText );
		theSize = GetStringLength( shit ) + 1;
		TWriteData( sizeof(short), (Ptr)&theSize, true );
		TWriteData( (long)theSize, shit, true );
	}


// send [ROMIconID]

	if ( flags & kPersonificationROMIconID )
	{
		ROMIconID = (DBID)GetPersonificationPart( theUserID, kPersonificationROMIconID );
		TWriteData( sizeof(DBID), (Ptr)&(ROMIconID), true );			// send ROMID
	}

// the box never sends the server a custom icon [RAMIcon]

// send [ROMClutID]

	if ( flags & kPersonificationROMClutID )
	{
		ROMClutID = (DBID)GetPersonificationPart( theUserID, kPersonificationROMClutID );
		TWriteData( sizeof(DBID), (Ptr)&(ROMClutID), true );			// send ROMClutID
	}
}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Box <-> Box Personification Xfer


// Create and return a temp buffer containing the personfication 
// information to send over the wire to the opponent using GameTalk.
// We allocate the buffer here. The caller must dispose of it when done.
//
// The info exchanged looks like this:
//
//				         	  			               			       Taunt	  Info	    Value	   Custom Icon
//	opcode	[BOXID][UserID][ROMIconID][ROMClutID][pad][Town][Handle][size:TexT][size:Text][size:Text][size:RAIC][IDSC]
//			\_______________________________________________________________________________________/\_______________/
//					  							Mandatory				        			  			 Optional
//
// If the sender doesn't have a Custom Icon, he sends 0 for the ISEG size, then no more.
//
// The IDSC is just the framerate (a short) for an animated icon.  If the sender's custom
//  icon is static, 0 is sent.
//
// The receiver must always accept all resources.

Ptr _CreatePersonificationBundle( unsigned char theUser )
{
UserName			handle;
Hometown			town;
IconSetup			*theIDSC;
IconSetup			noIDSC;
Ptr					info;
long				infoSize;
Ptr					taunt;
long				tauntSize;
Ptr					value;
long				valueSize;
Ptr					customIcon;
long				customIconSize;
long				size;
Ptr					buffer;
Ptr					bufPtr;
char				emptyString[1];
DBID				ROMIconID;
DBID				ROMClutID;
DBID				RAMIconID;
userIdentification	user;

	emptyString[0] = 0;


// prefetch taunt string to get its size

	taunt = GetPersonificationPart( theUser, kPersonificationTauntText );
	if (taunt) {
		tauntSize = GetStringLength( taunt ) + 1;
	} else {
		taunt = emptyString;
		tauntSize = 1;
	}

// prefetch info string to get its size

	info = GetPersonificationPart( theUser, kPersonificationAboutText );
	if (info) {
		infoSize = GetStringLength( info ) + 1;
	} else {
		info = emptyString;
		infoSize = 1;
	}

// prefetch Value string to get its size

	value = GetLocalGameValue();
	if (value) {
		valueSize = GetStringLength( value ) + 1;
	} else {
		value = emptyString;
		valueSize = 1;
	}

// prefetch RAIC & IDSC, if the user is using a custom icon

	customIconSize = 0;										
	theIDSC = 0;

	FillInUserIdentification( theUser, &user );
	RAMIconID = PlayerRAMIconExists( &user );
			
	if ( RAMIconID != kDBReservedItemType )
	{
		customIconSize = DBGetItemSize( kRAMIconType, RAMIconID );
		customIcon = DBGetItem( kRAMIconType, RAMIconID );
	}
	
// calculate the size of the bundle

	tauntSize = (tauntSize + 1) & 0xFFFE;
	infoSize = (infoSize + 1) & 0xFFFE;
	valueSize = (valueSize + 1) & 0xFFFE;
	customIconSize = (customIconSize + 1) & 0xFFFE;
	
	size =	sizeof(BoxSerialNumber)					// box id
		  + sizeof(DBID)							// user id
		  + sizeof(DBID)							// [ROMIconID]
		  + sizeof(DBID)							// [ROMClutID]
		  + sizeof(char)							// [pad]
		  + sizeof(Hometown)						// [Town]
		  + sizeof(UserName)						// [Handle]
		  + sizeof(short)							// [size of Taunt]
		  + tauntSize								// [Taunt string]
		  + sizeof(short)							// [size of Info]
		  + infoSize								// [Info string]
		  + sizeof(short)							// [size of Value]
		  + valueSize								// [Value string]
		  + sizeof(short)							// [size of ISEG]
		  + customIconSize;							// [RAIC]
		  
	if ( customIconSize )
		size += sizeof(IconSetup);					// [IDSC]

// allocate buffer big enough for the bundle

	bufPtr = buffer = NewMemory( kTemp, size );

// send box id and user id
	ByteCopy ( bufPtr, (Ptr)&(GetCurrentLocalUser()->box), sizeof(BoxSerialNumber) );
	bufPtr += sizeof(BoxSerialNumber);

	ByteCopy ( bufPtr, (Ptr)&(GetCurrentLocalUser()->userID), sizeof(DBID) );
	bufPtr += sizeof(DBID);
	
// send [ROMIconID]
	ROMIconID = (DBID)GetPersonificationPart( theUser, kPersonificationROMIconID );
	ByteCopy( bufPtr, (Ptr)&(ROMIconID), sizeof(DBID) );
	bufPtr += sizeof(DBID);

// send [ROMClutID]
	ROMClutID = (DBID)GetPersonificationPart( theUser, kPersonificationROMClutID );
	ByteCopy( bufPtr, (Ptr)&(ROMClutID), sizeof(DBID) );
	bufPtr += sizeof(DBID);

// send [pad]
	ByteCopy( bufPtr, (Ptr)&(ROMClutID), sizeof(char) );
	bufPtr += sizeof(DBID);

// send [Town]

	ByteCopy( bufPtr, GetPersonificationPart( theUser, kPersonificationTown ), sizeof(Hometown) );
	bufPtr += sizeof(Hometown);

// send [Handle]

	ByteCopy( bufPtr, GetPersonificationPart( theUser, kPersonificationHandle ), sizeof(UserName) );
	bufPtr += sizeof(UserName);

// send [size:Taunt]
	
	*(short *)bufPtr = tauntSize;
	bufPtr += sizeof(short);
	ByteCopy( bufPtr, (Ptr)taunt, tauntSize );
	bufPtr += tauntSize;

// send [size:Info]
	
	*(short *)bufPtr = infoSize;
	bufPtr += sizeof(short);
	ByteCopy( bufPtr, (Ptr)info, infoSize );
	bufPtr += infoSize;

// send [size:Value]
	
	*(short *)bufPtr = valueSize;
	bufPtr += sizeof(short);
	ByteCopy( bufPtr, (Ptr)value, valueSize );
	bufPtr += valueSize;

// send [size:RAIC][IDSC] (or 0 if there's no custom icon)

	*(short *)bufPtr = customIconSize;
	bufPtr += sizeof(short);
	if (customIconSize)
	{
		ByteCopy( bufPtr, (Ptr)customIcon, customIconSize );			// send RAIC
		bufPtr += customIconSize;
		theIDSC = DBGetItem( kIconDescType, RAMIconID );				// is it animated?
		if (theIDSC)
			ByteCopy( bufPtr, (Ptr)theIDSC, sizeof(IconSetup) );		// send IDSC
			else
			{	
				noIDSC.frameDelay = 0;
				ByteCopy( bufPtr, (Ptr)&noIDSC, sizeof(IconSetup) );	// send 0
			}
	}
	return buffer;
}


// Extract information from a personfication bundle.
// Caller disposes the buffer.
// The info exchanged looks like this:
//
//				         	  			               			       Taunt	  Info	    Value	   Custom Icon
//	opcode	[BOXID][UserID][ROMIconID][ROMClutID][pad][Town][Handle][size:TexT][size:Text][size:Text][size:RAIC][IDSC]
//			\_______________________________________________________________________________________/\_______________/
//					  							Mandatory				        			  			 Optional

void _ParsePersonificationBundle( Ptr buffer, PersonificationSetupRec *OPers )
{
long				theSize;
Ptr					bufPtr;

	bufPtr = buffer;

	OPers->userIDData = (userIdentification *)NewMemory( kTemp, sizeof(userIdentification) );
	
// get BoxIdentification and user ID
	ByteCopy( (Ptr)&(OPers->userIDData->box), bufPtr, sizeof(BoxSerialNumber) );
	bufPtr += sizeof(BoxSerialNumber);

	ByteCopy( (Ptr)&(OPers->userIDData->userID), bufPtr, sizeof(DBID) );
	bufPtr += sizeof(DBID);
		
// get [ROMIconID]

	ByteCopy( (Ptr)&(OPers->userIDData)->ROMIconID, bufPtr, sizeof(DBID) );
	bufPtr += sizeof(DBID);

// get [ROMClutID]

	ByteCopy( (Ptr)&(OPers->userIDData)->colorTableID, bufPtr, sizeof(DBID) );
	bufPtr += sizeof(DBID);

// get [pad]

	bufPtr += sizeof(char);

// get [Town]

	ByteCopy( (Ptr)&(OPers->userIDData)->userTown, bufPtr, sizeof(Hometown) );
	bufPtr += sizeof(Hometown);

// get [Handle]

	ByteCopy( (Ptr)&(OPers->userIDData)->userName, bufPtr, sizeof(UserName) );
	bufPtr += sizeof(UserName);

// get [size:Taunt]

	theSize = *(short *) bufPtr;
	bufPtr += sizeof(short);
	OPers->TauntTextData = NewMemory( kTemp, theSize );
	ByteCopy( (Ptr)OPers->TauntTextData, bufPtr, theSize );
	bufPtr += theSize;

// get [size:Info]

	theSize = *(short *) bufPtr;
	bufPtr += sizeof(short);
	OPers->InfoTextData = NewMemory( kTemp, theSize );
	ByteCopy( (Ptr)OPers->InfoTextData, bufPtr, theSize );
	bufPtr += theSize;

// get [size:Value]

	theSize = *(short *) bufPtr;
	bufPtr += sizeof(short);
	SetOppGameValue( bufPtr );
	bufPtr += theSize;

// get [size:RAIC][IDSC] (or 0 if there's no custom icon)

	theSize = *(short *)bufPtr;
	bufPtr += sizeof(short);

	if ( theSize )
	{
		OPers->RAICData = NewMemory( kTemp, theS