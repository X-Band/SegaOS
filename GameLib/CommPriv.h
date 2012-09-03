


#define	MANAGERGLOBALTYPE		CommGlobals


typedef
struct CommGlobals
{
	short status;
	short lastError;
} CommGlobals;


/* connection status */
enum
{
	kNoConnection,
	kWaitingForConnection,
	kOpenConnection
};
