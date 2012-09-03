/*
	File:		MungeCode.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	  7/8/94	SAH		Fix code as we go.
		 <2>	  7/8/94	SAH		Fixed CheckCodeForJTRefs.
		 <1>	 5/28/94	CMY		first checked in

	To Do:
*/


#include "Errors.h"
#include "MungeCode.h"
#include "JumpTable.h"
#include "Opcodes68k.h"

static JumpTableEntry * GetCodeJTEntry ( short * reference );

Boolean CheckCodeForJTRefs(void *codePtr, long length)
{
	short *instructionWalker;
	short segment;
	long instructionCount = length/2;
	Boolean inCode = false;
	Boolean hasJTRef = false;
	Boolean functionHasJTRef;
	short functionSeg;
	short branchOffset;
	JumpTableEntry * jtEntry;
	
	
	/* get our segment ptr */
	jtEntry = (JumpTableEntry *) ((unsigned long) codePtr - 2);
	segment = jtEntry->loaded.segmentNumber;
	
	/* get the address to start walking at */
	instructionWalker = (short *) jtEntry->loaded.address;
		
	while (instructionCount--) {
		if (*instructionWalker == kLinkA6Opcode)
			inCode = true;
		
		if (*instructionWalker == kUnlkA6Opcode)
			inCode = false;
		
		if (inCode && (	*instructionWalker == kJsrA5Opcode ||
						*instructionWalker == kPeaA5Opcode ||
						(*instructionWalker & ~kLeaRegisterMask) == kLeaA5Opcode)) {
						
			/* is it in our segment */
			jtEntry = GetCodeJTEntry ( instructionWalker );
			
			functionHasJTRef = true;
			
			/* see if the segment is loaded */
			if ( jtEntry->loaded.jumpLongOpcode == kJmpLongOpcode )  {
			
				functionSeg = jtEntry->loaded.segmentNumber;
				if ( functionSeg == segment )  {
					functionHasJTRef = false;
					
					/* make it pc relative */
					branchOffset = (long) jtEntry->loaded.address - (long) instructionWalker;
					branchOffset -= 2;
					
					/* create the new opcode */
					if ( *instructionWalker == kJsrA5Opcode )
						*instructionWalker++ = kJsrPCOpcode;
						
					else if ( *instructionWalker == kPeaA5Opcode )
						*instructionWalker++ = kPeaPCOpcode;
						
					else if ( (*instructionWalker & ~kLeaRegisterMask) == kLeaA5Opcode )  {
						*instructionWalker++ = ( *instructionWalker ^ kLeaA5Opcode ) | kLeaPCOpcode;
					}

					*instructionWalker = branchOffset;
				}
			}
			
			if ( functionHasJTRef )  {
				WARNING_MESG("Code contains a jump-table reference");
				hasJTRef = true;
			}
		}
		
		++instructionWalker;
	}
	
	return hasJTRef;
}

static  JumpTableEntry * GetCodeJTEntry ( short * reference )
{
	short functionJTOffset;

	functionJTOffset = *(reference + 1);
	asm  {
		move.l	a5,a0
		add.w	functionJTOffset,a0
		subq.w	#2,a0
		move.l	a0,d0
	}
}
