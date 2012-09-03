/*
	File:		Challnge.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  8/3/94	DJ		added kChallengeTypes for new way of sending challenges to
									server
		 <5>	 7/15/94	DJ		made work for server
		 <4>	 7/11/94	KON		Add find network opponent serial number and download mail
									constants.
		 <3>	  6/4/94	KON		Managerize.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.

	To Do:
*/

#ifndef __Challnge__
#define __Challnge__

//
// These are stuffed into the boxsernum of the challengeUserIdentification
// by findopponent.  Then, DoSendChallengeOpCode checks this and sends the
// challengeType (see #defines below) and the userIdentification if needed.
//
#define kFindNetworkOpponentSerialNumber	-2L
#define kDownloadOnlyMailSerialNumber		-3L


//
// These are sent to the server as a char by DoSendChallengeOpCode to indicate
// which kind of connect this is.
//
#define kChallengeTypeChallengeByBoxSerialNumber	1
#define kChallengeTypeChallengeByHandle			2
#define kChallengeTypeAutomatch					3
#define kChallengeTypeMailOnly					4


#ifndef __SERVER__

phoneNumber * GetChallengePhoneNumber( void ) =
	CallDispatchedFunction( kGetChallengePhoneNumber );

void SetChallengePhoneNumber( phoneNumber *thePhoneNumber ) =
	CallDispatchedFunction( kSetChallengePhoneNumber );

userIdentification * GetChallengeIdentification( void ) =
	CallDispatchedFunction( kGetChallengeIdentification );

void SetChallengeIdentification( userIdentification *theIdentification ) =
	CallDispatchedFunction( kSetChallengeIdentification );

#endif __SERVER__

#endif __Challnge__
