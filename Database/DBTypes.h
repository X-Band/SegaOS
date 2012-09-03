/*
	File:		DBTypes.h

	Contains:	Standard data types for the database

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<76>	 8/24/94	HEC		#define'd database id's instead of enum to avoid accidental
									versionitis and skews.
		<75>	 8/23/94	BET		Roll out Chris' last change, it trashed shit good!
		<74>	 8/23/94	CMY		Moved SecretProcType and labeled it so that it wont be
									renumbered.
		<73>	 8/22/94	DJ		moved kRestartInfoType into SendQ.h
		<72>	 8/21/94	JBH		Added ROM db constants type.
		<71>	 8/19/94	JBH		Made gamepatches and ram icons purgeable. Made daily news
									unpurgeable, as it is being viewed while things are being
									downloaded.
		<70>	 8/18/94	CMY		Added ChortleProcs
		<69>	 8/12/94	JBH		Add kXBandNewsDataType
		<68>	 8/12/94	BET		move kPhoneNumberType to unpurgeable area.
		<67>	 8/12/94	JOE		nuke obolete IconRef stuff, add new RAIC (RAM Icon) type
		<66>	 8/10/94	BET		Added scriptID brokering vi phoneNumber structure.
		<65>	  8/8/94	JOE		Added kCodeletsType
		<64>	  8/7/94	JOE		Added kSoundEffectRefString
		<63>	  8/7/94	ADS		Added restart info (only used by SendQ)
		<62>	  8/4/94	JOE		Added kMathDataType
		<61>	 7/28/94	CMY		Added screen layout type.
		<60>	 7/26/94	CMY		Added ButtonAnimation type.
		<59>	 7/26/94	JBH		Added mazeDataType, to save maze state.
		<58>	 7/22/94	BET		Add kConnectSequenceType for modem connect sequence scripts.
		<57>	 7/22/94	JOE		Added kButtonSequenceFX to control button animation sound
									effects
		<56>	 7/21/94	CMY		Added kButtonSequence to control button animation
		<55>	 7/19/94	CMY		Added buttonGraphic for our golden buttons.
		<54>	 7/19/94	JOE		Nuke kGameNameType; names are now in NGP 1
		<53>	 7/18/94	CMY		Added TextBoxJizzlers
		<52>	 7/17/94	KON		Add kPatternDictionaryType.
		<51>	 7/17/94	ADS		Added about-strings
		<50>	 7/17/94	JBH		(cmd-z) Removed DB type for thank-you string. Now they're in
									writeable strings.
		<49>	 7/16/94	JBH		Added DB type for thank-you string.
		<48>	 7/15/94	JBH		Added DB type for CLUT animation data.
		<47>	 7/14/94	CMY		Add radio button DB types
		<46>	 7/13/94	JOE		Added DB type for pre/post game screen, moved IDSC and ISEG to
									proper purging spot
		<45>	 7/13/94	CMY		Added DB types for mail outbox.
		<44>	 7/10/94	KON		Add vector path data type.
		<43>	 7/10/94	JOE		update Sound FX and BGM types
		<42>	 7/10/94	JOE		added BGM ref and FX ref
		<41>	  7/8/94	JBH		Stupid caps-lock key, it's really kProgressType.
		<40>	  7/8/94	JBH		Here's kProgressType, for progress mgr.
		<39>	  7/6/94	JOE		hurray, another new type; the WSTR.
		<38>	  7/5/94	HEC		Fixed my fucking stupid fuckup.
		<37>	  7/5/94	HEC		Added kConstantsDBType (unpurgeable).
		<36>	  7/5/94	JOE		personification type update
		<35>	  7/5/94	ADS		Revised secret keys type
		<34>	  7/4/94	ADS		Added secret keys type
		<33>	  7/3/94	JOE		Made Custom Icon types (IDSC, ISEG) purgeable.
		<32>	  7/3/94	SAH		Added kAnimation.
		<31>	  7/3/94	JOE		add IDSC, ISEG type
		<30>	  7/2/94	JBH		Added kDialogDismissalType.
		<29>	  7/2/94	JOE		add IREF type
		<28>	  7/1/94	JOE		add RANK type for user game rankings (history page stuff)
		<27>	  7/1/94	SAH		Restructured purge order.
		<26>	 6/30/94	SAH		Made DBTypes an enum and reordered for purging priority.
		<25>	 6/30/94	CMY		Added keyboard layouts for screens.
		<24>	 6/30/94	SAH		Killed some unused types for SendQ stuff.
		<23>	 6/29/94	KON		update for multiple news forms and types.
		<22>	 6/29/94	BET		(Really DJ) Clean up after KON.
		<21>	 6/28/94	SAH		Killed the user type and some other unused ones.
		<20>	 6/20/94	KON		Added the deferred dialog manager.
		<19>	 6/20/94	SGR		added led types
		<18>	 6/14/94	CMY		Dialog templates.
		<17>	 6/14/94	CMY		Added dialog types.
		<16>	 6/14/94	DJ		added k800Number for the 800 number in ROM
		<15>	  6/9/94	KON		Added screen dispatch and screen state table types.
		<14>	  6/5/94	CMY		Added credits and restrictions to DB
		<13>	  6/4/94	KON		Added personification.
		<12>	  6/2/94	BET		added news
		<11>	  6/1/94	KON		Add address book types and player local info type.
		<10>	 5/31/94	SAH		Added kGamePatchType.
		 <9>	 5/31/94	KON		Added test strings, test User ID's, and control tables.
		 <8>	 5/30/94	SAH		Added sound id's.
		 <7>	 5/30/94	KON		Fix conflicting ID's.
		 <6>	 5/30/94	KON		Added coordinates, object descriptors, and layouts.
		 <5>	 5/30/94	SAH		Added kClutType.
		 <4>	 5/29/94	DJ		changed db mail type from result
		 <3>	 5/28/94	KON		Added kStringType.
		 <2>	 5/26/94	KON		Add InBox and OutBox types for 4 users.
		<12>	 5/25/94	HEC		Switch on RESOURCE_DATABASE instead of SIMULATOR
		<11>	 5/24/94	SAH		Made kResultFifoTypeX compile for the non-simulator version.
		<10>	 5/24/94	KON		Added session and stream tyeps.
		 <9>	 5/23/94	KON		Added multiple FIFO's to handle up to four users.
		 <8>	 5/21/94	SAH		Added kAnimatorType.
		 <7>	 5/20/94	SAH		Changed the bitmap type to 'SEGA'
		 <6>	 5/18/94	KON		Added support for FontType and DITLProcType.
		 <5>	 5/17/94	SAH		Added cursor and bitmap types.
		 <4>	 5/16/94	SAH		Added kPathType.
		 <3>	 5/12/94	KON		Add SendQ type. Gave types meaningful names for simulator and
									added non-simulator defines.

	To Do:
*/

#ifndef __DBTypes__
#define __DBTypes__

#ifdef RESOURCE_DATABASE

#define kNGPType					'NGP '
#define kUserType					'USER'
#define kDecompressorType			'DCMP'
#define kSendQType					'SEND'
#define kPathType					'PATH'
#define kCursorType					'SCUR'
#define kBitMapType					'BITM'
#define kFontType					'sFNT'
#define kDITLProcType				'fDTL'
#define	kAnimatorType				'APRC'
#define kSessionType				'SESS'
#define kStreamType					'STRM'
#define kMailInBoxType0				'IBx0'
#define kMailInBoxType1				'IBx1'
#define kMailInBoxType2				'IBx2'
#define kMailInBoxType3				'IBx3'
#define kMailOutBoxType0			'OBx0'
#define kMailOutBoxType1			'OBx1'
#define kMailOutBoxType2			'OBx2'
#define kMailOutBoxType3			'OBx3'
#define kStringType					'TexT'
#define kClutType					'CLUT'
#define kCoordinateType				'CORD'
#define kObjectDesc					'DESC'
#define kScreenLayout				'Laid'
#define	kSoundFXType				'SSND'
#define	kSoundBGMType				'SBGM'
#define kControlLayout				'Cntl'
#define kTestUserID					'TUID'
#define	kGamePatchType				'GPCH'
#define	kAddressBookType0			'ADD0'
#define	kAddressBookType1			'ADD1'
#define	kAddressBookType2			'ADD2'
#define	kAddressBookType3			'ADD3'
#define	kPlayerInfoLocal			'INFO'
#define	kDailyNews					'NEWS'
#define	kPersonificationType		'PERS'
#define	kUserRankings				'RANK'
#define kCreditsDBType				'CRED'
#define kRestrictionsDBType			'REST'
#define kScreenGetDispatchType		'SGDT'
#define kScreenStateTableType		'SSTT'
#define k800Number					'P800'
#define kDialogTemplateType			'DiaT'
#define kDialogType					'Dilg'
#define kDeferredDialogType			'DDlg'
#define kNewsForm					'FORM'
#define kOtherNews					'NEW1'
#define kKeyboardEntryLayout		'Keys'
#define	kIconDescType				'IDSC'
#define	kIconBitMapType				'ISEG'
#define	kDialogDismissalType		'DIS '
#define	kAnimation					'ANIM'
#define	kWriteableString			'WSTR'
#define	kProgressType				'PROG'
#define	kBackgroundMusicRef			'BMID'
#define	kSoundEffectRef				'FXID'
#define	kVectorType					'VCTR'
#define	kTauntGameOverLayout		'TGOS'
#define kRadioButtonLayout			'RADI'
#define	kCLUTDataType				'CDAT'
#define kAboutString				'ABOU'
#define kTextBoxJizzler				'JIZL'
#define kButtonSequence				'BSEQ'
#define kButtonSequenceFX			'BSFX'
#define kConnectSequenceType		'Cnct'
#define	kMazeDataType				'MAZE'
#define	kMathDataType				'MATH'
#define	kSoundEffectRefString		'FXIS'
#define	kCodeletsType				'k0de'
#define	kPhoneNumberType			'phön'
#define	kRAMIconType				'RAIC'
#define	kXBandNewsDataType			'XBND'


#else

/* these items are reserved low priority entries for after ROM freeze - don't use */
/* them before! */
/**************************************************************************************/
/* NOTE: these entries may be purged during normal system operation - be very careful */
/* with what you put here ... ALWAYS ADD TO THE END OF THE LIST!					  */
/**************************************************************************************/

#define kReservedLow0			0
#define kReservedLow1			1
#define kReservedLow2			2
#define kReservedLow3			3
#define kReservedLow4			4
#define kReservedLow5			5
#define kReservedLow6			6
#define kReservedLow7			7
#define kReservedLow8			8
#define kRAMIconType			9
#define kGamePatchType			10
#define kOtherNews				11
#define kDeferredDialogType		12
#define kAboutString			13
	
/* these are reserved mid level entries - don't use them before ROM freeze */
/* these items may be purged by the server but will not be pruged during normal use */

#define kStandardSafeDBType		14
#define kReservedMid0			kStandardSafeDBType
#define kReservedMid1			15
#define kReservedMid2			16
#define kReservedMid3			17
#define kReservedMid4			18
#define kReservedMid5			19
#define kReservedMid6			20
#define kReservedMid7			21
#define kReservedMid8			22
#define kDailyNews				23
#define kNGPType				24
#define kScreenLEDAnimation		25
#define kSessionType			26
#define kStreamType				27
#define kSendQType				28
#define kCreditsDBType			29
#define kMailInBoxType0			30
#define kMailInBoxType1			31
#define kMailInBoxType2			32
#define kMailInBoxType3			33
#define kIconDescType			34
#define kIconBitMapType			35
#define kPersonificationType	36
#define kUserRankings			37
#define kScreenStateTableType	38
#define kScreenLayout			39
#define kKeyboardEntryLayout	40
#define kScreenGetDispatchType	41
#define kDialogTemplateType		42
#define kDialogType				43
#define kControlLayout			44
#define kObjectDesc				45
#define kDITLProcType			46
#define kSoundFXType			47
#define kSoundBGMType			48
#define kStringType				49
#define kClutType				50
#define kCoordinateType			51
#define kTestUserID				52
#define kNewsForm				53
#define kRestrictionsDBType		54
#define kAddressBookType0		55
#define kAddressBookType1		56
#define kAddressBookType2		57
#define kAddressBookType3		58
#define kAnimation				59
#define kFontType				60
#define kBitMapType				61
#define kCursorType				62
#define kPathType				63
#define kAnimatorType			64
#define kVectorType				65
#define kDecompressorType		66
#define kDialogDismissalType	67
#define kSecretListType			68
#define kSecretSequenceType		69
#define kWriteableString		70
#define kProgressType			71
#define kBackgroundMusicRef		72
#define kSoundEffectRef			73
#define kMailOutBoxType0		74
#define kMailOutBoxType1		75
#define kMailOutBoxType2		76
#define kMailOutBoxType3		77
#define kTauntGameOverLayout	78
#define kRadioButtonLayout		79
#define kCLUTDataType			80
#define kPatternDictionaryType	81
#define kTextBoxJizzlerType		82
#define kButtonGraphicType		83
#define kButtonSequenceType		84
#define kButtonSequenceFXType	85
#define kMazeDataType			86
#define kButtonAnimationType	87
#define kScreenLayoutType		88
#define kMathDataType			89
#define kSoundEffectRefString	90
#define kCodeletsType			91
#define kXBandNewsDataType		92
#define kSecretProcType			93
#define kChortleProcType		94
#define kROMDBConstantsType		95
	
/* these entries are protected and will not be purged */
#define kBaseUnpurgeableDBType	96
#define kConstantsDBType		kBaseUnpurgeableDBType
#define kConnectSequenceType	97
#define kPhoneNumberType		98
	
/* don't delete this one */
#define kLastDBType				99

#if ( kLastDBType >= 255 )
#error "Too many db types"
#endif

#endif RESOURCE_DATABASE

#endif __DBTypes__