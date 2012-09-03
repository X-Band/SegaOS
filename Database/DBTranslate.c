/*
	File:		DBTranslate.c

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  7/1/94	SAH		first checked in

	To Do:
*/


#ifdef SIMULATOR

#include "SegaOS.h"
#include "OSGlobals.h"
#include "Database.h"
#include "DataBase_priv.h"
#include "heaps.h"

#undef	MANAGERGLOBALTYPE
#include "heapsPriv.h"



#define TRANSPTR(_ptr,_old,_new) \
		(((long)(_ptr) - (long)(_old)) + (long)(_new))
		
void TranslateROM(Handle romHandle, long romStart)
{
	// Translate pointers to be relative to start of rom
	
	DBTypeNode *nexttype;
	DBListNode *nextlist;
	DBTypeNode *rt;
	DBListNode *n;
	long old, new;
	Heap *		hp;
	Block *		bp;
	Block *		pp;
	Block *		np;

	// NOTE: ROMHeader's offset to first DBTypeNode remains the same!
	
	HLock(romHandle);
	
	/* translate the db */
	old = gRomDatabase;
	new = (long)*romHandle;
	rt = (DBTypeNode *) ((Ptr)((ROMHeader *)gRomDatabase)->rDBTypeNodeOffset + (long)gRomDatabase);
	for (; rt; rt = nexttype)
	{
		for (n = rt->list; n; n = nextlist)
		{
			nextlist = n->next;
			if (nextlist)
				((DBListNode *)TRANSPTR(n,old,new))->next =
					(DBListNode *) TRANSPTR(n->next,old,romStart);
		}
		nexttype = rt->next;
		if (rt->next)
			((DBTypeNode *)TRANSPTR(rt,old,new))->next =
				(DBTypeNode *) TRANSPTR(rt->next,old,romStart);
		if (rt->list)
			((DBTypeNode *)TRANSPTR(rt,old,new))->list =
				(DBListNode *) TRANSPTR(rt->list,old,romStart);
	}
	
	/* translate the heap */
	hp = (Heap *) TRANSPTR(gRomHeap,old,new);
	bp = (Block *) TRANSPTR(hp->head,old,new);
	hp->head = (Block *) TRANSPTR(bp,new,romStart);
	
	np = (Block *) TRANSPTR(hp->tail,old,new);
	hp->tail = (Block *) TRANSPTR(np,new,romStart);
	np->prev = (Block *) TRANSPTR(np->prev,old,romStart);
	
	pp = (Block *) -1L;
	
	while ( bp )
		{
		/* if this block has a prev, set it to the mapped one */
		if ( bp->prev )
			{
			bp->prev = pp;
			}
		
		/* get the real prev ptr for the next block */
		pp = (Block *) TRANSPTR(bp,new,romStart);
		
		/* translate the next ptr if not nil */
		np = bp->next;
		if ( np )
			{
			np = (Block *) TRANSPTR(np,old,new);
			bp->next = (Block *) TRANSPTR(np,new,romStart);
			}

		bp = np;
		}
	
	HUnlock(romHandle);
}

#endif