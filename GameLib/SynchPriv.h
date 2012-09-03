
typedef
struct SynchGlobals
{
	short dummy;
} SynchGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE SynchGlobals
#else

extern SynchGlobals synch;

#endif
