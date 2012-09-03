/*
	File:		MungeCode.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 5/30/94	CMY		Add opcode for JMP xxxxxxxx
		 <1>	 5/28/94	CMY		first checked in
				 5/28/94	CMY		first checked in

	To Do:
*/


#define kJmpLongOpcode		0x4EF9
#define	kJsrA5Opcode		0x4EAD
#define kPeaA5Opcode		0x486D
#define kLinkA6Opcode		0x4E56
#define	kUnlkA6Opcode		0x4E5E
#define kLeaA5Opcode		0x41ED	/* LEA xx(A5),A0; add (n<<9) for destination An */
#define kLeaRegisterMask	0x0E00	/* Mask for register An field of LEA ---,An */

/*
 * Given a pointer to code (NOT a procPtr -- that's a pointer to a jump table entry)
 * check the code for jump table references, ie: jsr xx(a5), lea xx(a5), pea xx(a5)
 */
Boolean CheckCodeForJTRefs(void *codePtr, long length);

