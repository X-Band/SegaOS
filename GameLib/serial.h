

/*
*	Serial chip equates/macros
*/




#define	sBase		((unsigned char *) 0x380000)			// base of the serial register
#define	sRcvTrx		((unsigned char *) sBase+1)
#define	sIntEn		((unsigned char *) sBase+3)
#define	sIntID		((unsigned char *) sBase+5)				// read only
#define	sFifoCtl	((unsigned char *) sBase+5)				// write only
#define	sLineCtl	((unsigned char *) sBase+7)
#define	sMdmCtl		((unsigned char *) sBase+9)
#define	sLineSts	((unsigned char *) sBase+11)
#define	sMdmSts		((unsigned char *) sBase+13)
#define	sScrtch		((unsigned char *) sBase+15)

#define	sDivLow		((unsigned char *) sBase+1)				// must asset sLCRDLab to access!
#define	sDivHi		((unsigned char *) sBase+3)

// line control bits

// bits per character
enum
{
	sLCR5Bits = 0x0,					// five bit words
	sLCR6Bits = 0x1,					// six bit words
	sLCR7Bits = 0x2,					// seven bit words
	sLCR8Bits = 0x3						// eight bit words
};

// stop bits
enum
{
	sLCR1Stop = 0x0,					// one stop bit
	sLCR2Stop = 0x4						// two stop bits
};

enum
{
	sLCRNoPrty = 0x0,					// silly but looks good
	sLCRPrtyEn = 0x8,					// enable parity
	sLCRPrty0 = 0x10,					// even parity if above
	sLCRStky = 0x20,					// sticky parity bit
	sLCRBrk = 0x40,						// send a break
	sLCRDLab = 0x80						// go to extended address space
};

// fifo control register

// fifo control bits
enum
{
	fEnFifos = 0x1,						// enable send and recieve fifos
	fClrRcv = 0x2,						// clear the reciever fifo
	fClrXmt = 0x4						// clear the tranmit fifo
};

// modem control register
enum
{
	sSetDTR = 0x1,						// set dtr line
	sRts = 0x2,							// set request to send
	sLoopback = 0x10					// enable loopback
};

// modem status register
enum
{
	sDeltaCts = 0x1,					// delta clear to send
	sDeltaDataSetReady = 0x2,			// delta data set and ready
	sTrailingRing = 0x4,				// trailing edge of ring detected
	sDeltaCarrierDetect = 0x8,			// delta carrier detect
	sCts = 0x10,						// clear to send
	sDataSetReady = 0x20				// data set ready
};


// line status bits
enum
{
	sDataReady = 0x1,					// byte in receive buffer or fifo
	sOverrunError = 0x2,				// receive buffer overrun
	sParityError = 0x4,					// parity error
	sFrameError = 0x8,
	sBreakInterrupt = 0x10,
	sXmitHoldingEmpty = 0x20,			// transmit holding register empty
	sXmitEmpty = 0x40,					// both holding and xmit buffers empty
	sParityOrFrameError = 0x80			// at least one parity or framing error in fifo
};
