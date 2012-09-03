// ===========================================================================
//	DebugMacro.h					©1994 Brian Topping All rights reserved.
// ===========================================================================

#define DEBUG 1

#ifdef DEBUG
	#define IfDEBUGStr(a,b) { if (a) DebugStr(b); }
#else
	#define IfDEBUGStr(a,b)
#endif
