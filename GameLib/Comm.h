

typedef short CommError;

enum
{
	eCantOpenConnection = -1000,
	eNoByteReady,
	eTransmitBufferFull,
	eConnectionLost,
	eNoConnection,
	eParityError,
	eFrameError,
	eTimeout
};



CommError	InitConnection ( void ) = 
	CallDispatchedFunction( kInitConnection );

CommError	ReadSerialByte ( unsigned char *byte ) = 
	CallDispatchedFunction ( kReadSerialByte );
	
CommError	WriteSerialByte ( unsigned char byte ) = 
	CallDispatchedFunction ( kWriteSerialByte );

CommError	TransmitBufferFree ( void ) = 
	CallDispatchedFunction ( kTransmitBufferFree );
	
CommError	WaitForConnection ( void ) = 
	CallDispatchedFunction ( kWaitForConnection );

CommError	TestForConnection ( void ) = 
	CallDispatchedFunction ( kTestForConnection );

CommError	AcknowledgeConnection ( void ) =
	CallDispatchedFunction ( kAcknowledgeConnection );

CommError	DialNumber ( char * number ) = 
	CallDispatchedFunction ( kDialNumber );

CommError	Disconnect ( void ) =
	CallDispatchedFunction ( kDisconnect );
