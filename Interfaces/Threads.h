/*
	File:		Threads.h

	Contains:	External Interface to Thread Manager

	Copyright:	© 1991-1994 by Apple Computer, Inc., all rights reserved.

*/

#ifndef __THREADS__
#define __THREADS__

#include <Memory.h>

/* Thread Gestalt Selectors */
enum {
#define gestaltThreadMgrAttr	'thds'			/* Thread Manager attributes */
	
	gestaltThreadMgrPresent		= 0,			/* bit true if Thread Mgr is present */
	gestaltSpecificMatchSupport	= 1,			/* bit true if Thread Mgr supports exact match creation option */
	gestaltThreadsLibraryPresent = 2			/* bit true if ThreadsLibrary (Native version) has been loaded */
};

/* Thread states */
typedef unsigned short			ThreadState;
#define kReadyThreadState		((ThreadState) 0)
#define kStoppedThreadState		((ThreadState) 1)
#define kRunningThreadState		((ThreadState) 2)

/* Thread environment characteristics */
typedef void*	ThreadTaskRef;

/* Thread characteristics */
typedef unsigned long			ThreadStyle;
#define kCooperativeThread		(1<<0)
#define kPreemptiveThread		(1<<1)

/* Thread identifiers */
typedef unsigned long			ThreadID;
#define kNoThreadID				((ThreadID) 0)
#define kCurrentThreadID		((ThreadID) 1)
#define kApplicationThreadID	((ThreadID) 2)

/* Options when creating a thread */
typedef unsigned long			ThreadOptions;
#define kNewSuspend				(1<<0)
#define kUsePremadeThread		(1<<1)
#define kCreateIfNeeded			(1<<2)
#define kFPUNotNeeded			(1<<3)
#define kExactMatchThread		(1<<4)

/* Information supplied to the custom scheduler */
struct SchedulerInfoRec {
	unsigned long	InfoRecSize;
	ThreadID		CurrentThreadID;
	ThreadID		SuggestedThreadID;
	ThreadID		InterruptedCoopThreadID;
};
typedef struct SchedulerInfoRec SchedulerInfoRec;
typedef SchedulerInfoRec *SchedulerInfoRecPtr;

/* Prototype for thread's entry (main) routine */
typedef pascal void *			(ThreadEntryProc)(void *threadParam);
typedef ThreadEntryProc			*ThreadEntryProcPtr;

/* Prototype for custom thread scheduler routine */
typedef pascal ThreadID			(ThreadSchedulerProc)(SchedulerInfoRecPtr schedulerInfo);
typedef ThreadSchedulerProc		*ThreadSchedulerProcPtr;

/* Prototype for custom thread switcher routine */
typedef pascal void				(ThreadSwitchProc)(ThreadID threadBeingSwitched, void *switchProcParam);
typedef ThreadSwitchProc		*ThreadSwitchProcPtr;

/* Prototype for thread termination notification routine */
typedef pascal void				(ThreadTerminationProc)(ThreadID threadTerminated, void *terminationProcParam);
typedef ThreadTerminationProc	*ThreadTerminationProcPtr;

/* Prototype for debugger NewThread notification */
typedef pascal void					(DebuggerNewThreadProc)(ThreadID threadCreated);
typedef DebuggerNewThreadProc		*DebuggerNewThreadProcPtr;

/* Prototype for debugger DisposeThread notification */
typedef pascal void					(DebuggerDisposeThreadProc)(ThreadID threadDeleted);
typedef DebuggerDisposeThreadProc		*DebuggerDisposeThreadProcPtr;

/* Prototype for debugger schedule notification */
typedef pascal ThreadID				(DebuggerThreadSchedulerProc)(SchedulerInfoRecPtr schedulerInfo);
typedef DebuggerThreadSchedulerProc	*DebuggerThreadSchedulerProcPtr;

/* Errors */
enum {
	threadTooManyReqsErr	= -617,
	threadNotFoundErr		= -618,
	threadProtocolErr		= -619
};

#ifdef __cplusplus
extern "C" {
#endif

/* Thread Manager routines */
pascal OSErr CreateThreadPool(ThreadStyle threadStyle, short numToCreate, Size stackSize)
	= {0x303C,0x0501,0xABF2};

pascal OSErr GetFreeThreadCount(ThreadStyle threadStyle, short *freeCount)
	= {0x303C,0x0402,0xABF2};

pascal OSErr GetSpecificFreeThreadCount( ThreadStyle threadStyle, Size stackSize, short *freeCount )
	= {0x303C,0x0615,0xABF2};

pascal OSErr GetDefaultThreadStackSize(ThreadStyle threadStyle, Size *stackSize)
	= {0x303C,0x0413,0xABF2};

pascal OSErr ThreadCurrentStackSpace(ThreadID thread, unsigned long *freeStack)
	= {0x303C,0x0414,0xABF2};

pascal OSErr NewThread(	ThreadStyle threadStyle,
						ThreadEntryProcPtr threadEntry,
						void *threadParam,
						Size stackSize,
						ThreadOptions options,
						void **threadResult,
						ThreadID *threadMade)
	= {0x303C,0x0E03,0xABF2};

pascal OSErr DisposeThread(ThreadID threadToDump, void *threadResult, Boolean recycleThread)
	= {0x303C,0x0504,0xABF2};

pascal OSErr YieldToThread(ThreadID suggestedThread)
	= {0x303C,0x0205,0xABF2};

pascal OSErr YieldToAnyThread(void)
	= {0x42A7,0x303C,0x0205,0xABF2};

pascal OSErr GetCurrentThread(ThreadID *currentThreadID)
	= {0x303C,0x0206,0xABF2};

pascal OSErr GetThreadState(ThreadID threadToGet, ThreadState *threadState)
	= {0x303C,0x0407,0xABF2};

pascal OSErr SetThreadState(ThreadID threadToSet, ThreadState newState, ThreadID suggestedThread)
	= {0x303C,0x0508,0xABF2};

pascal OSErr SetThreadStateEndCritical(ThreadID threadToSet, ThreadState newState, ThreadID suggestedThread)
	= {0x303C,0x0512,0xABF2};

pascal OSErr SetThreadScheduler(ThreadSchedulerProcPtr threadScheduler)
	= {0x303C,0x0209,0xABF2};

pascal OSErr SetThreadSwitcher(ThreadID thread, ThreadSwitchProcPtr threadSwitcher, void *switchProcParam, Boolean inOrOut)
	= {0x303C,0x070A,0xABF2};

pascal OSErr SetThreadTerminator(ThreadID thread, ThreadTerminationProcPtr threadTerminator, void *terminationProcParam)
	= {0x303C,0x0611,0xABF2};

pascal OSErr ThreadBeginCritical(void)
	= {0x303C,0x000B,0xABF2};

pascal OSErr ThreadEndCritical(void)
	= {0x303C,0x000C,0xABF2};

pascal OSErr SetDebuggerNotificationProcs (	DebuggerNewThreadProcPtr notifyNewThread,
											DebuggerDisposeThreadProcPtr notifyDisposeThread,
											DebuggerThreadSchedulerProcPtr notifyThreadScheduler)
	= {0x303C,0x060D,0xABF2};

pascal OSErr GetThreadCurrentTaskRef (ThreadTaskRef *threadTRef)
	= {0x303C,0x020E,0xABF2};

pascal OSErr GetThreadStateGivenTaskRef (ThreadTaskRef threadTRef, ThreadID threadToGet, ThreadState *threadState)
	= {0x303C,0x060F,0xABF2};

pascal OSErr SetThreadReadyGivenTaskRef (ThreadTaskRef threadTRef, ThreadID threadToSet)
	= {0x303C,0x0410,0xABF2};

#ifdef __cplusplus
}
#endif

#endif /* __THREADS__ */
