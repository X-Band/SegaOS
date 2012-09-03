/*
	File:		Errors.c

	Contains:	xxx put contents here xxx

	Written by:	Dave Jevans

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/13/94	ATM		Added Logmsg() prototype for Mac server.
		<14>	 8/13/94	ATM		For __SERVER__, call Logmsg() instead of using fprintf(gLogFile,
									...).
		<13>	 8/12/94	SAH		Boy i love checking in this file.
		<12>	 8/12/94	SAH		More sega poo.
		<11>	 8/12/94	SAH		Made it work on unix and the sega.
		<10>	 8/12/94	DJ		made it work on unix
		 <9>	 8/12/94	HEC		Added ReportDialog.
		 <8>	  8/3/94	DJ		make it work under unix
		 <7>	 7/25/94	ADS		Can't stack crawl, sometime's were in interrupt state
		 <6>	 7/24/94	ADS		Use new dispatcher stack crawl shit
		 <5>	 7/17/94	BET		One more time...
		 <4>	 7/17/94	BET		Make errors go to a FILE * for the server build.  It used to
									send it to the modem.
		 <3>	 7/13/94	BET		unix-ise.
		 <2>	 6/19/94	BET		Geez, at least show me the string in macsbug!-)
		 <2>	 5/24/94	SAH		Fixed non-simulator compile errors.

	To Do:
*/



#include "Errors.h"


/************************************************************************
	This turns off the printfs if the debug or message isn't defined in
	the ThinkC project's Prefixes.
*************************************************************************/


#ifdef DEBUG
#define POOP
#endif

#ifdef MESSAGES
#define POOP
#endif

/************************************************************************/


void ReportDialog(char *file, long line, char *message);

#ifdef POOP

#include <stdio.h>

#ifndef __SERVER__

#include "Dispatcher.h"		// for the error reporting stuff
#include "SegaText.h"
#include "DialogMgr.h"
#include "DeferredDialogMgr.h"

void ReportDialog(char *file, long line, char *message)
{
char	buf[150];
char	lineStr[8];
char	temp[32];

	SegaNumToString(line,lineStr);
	*(long *)&buf[0] = 'ERRO';
	*(long *)&buf[4] = 'R: \0';
	SegaAppendText(buf,message,buf);
	*(long *)&temp[0] = ' FIL';
	*(long *)&temp[4] = 'E: \0';
	SegaAppendText(buf,temp,buf);
	SegaAppendText(buf,file,buf);
	*(long *)&temp[0] = ' LIN';
	*(long *)&temp[4] = 'E: \0';
	SegaAppendText(buf,temp,buf);
	SegaAppendText(buf,lineStr,buf);
	QDefDialog( kDDMainEventLoop, buf, kLargeDialog, 0, 0 );
}

#endif



#ifdef __SERVER__
void Logmsg(char *format, ...);
#else
#define gLogFile stderr
#endif


void ReportError(
	char *file,
	long line,
	char *message)
{
char	buf[150];

#ifdef DISP_CAN_REPORT
	// it would be really, really nice to be able to
	// do this.  unfortunately, sometimes ASSERT gets
	// called from an interrupt or timeproc, and printf
	// is no good at that point.  I would have to do a
	// bunch of globals hacking to make this work, so
	// for the time being, it's outta here.
//	SetDispatchReportMode(kDispReport_Printf);
//	ReportRecentDispatched(); 
#endif

#ifndef __SERVER__
#if defined(SIMULATOR)
	buf[0] = sprintf(&buf[1], "ERROR: %.80s FILE: %.25s LINE: %ld\n", message, file, line) - 1;
	fprintf(gLogFile, &buf[1]);
	DebugStr((ConstStr255Param)buf);
#else
	ReportDialog(file,line,message);
#endif
#else
	Logmsg("ERROR: %s FILE: %s LINE: %ld\n", message, file, line);
#endif
}

void ReportWarning(
	char *file,
	long line,
	char *message)
{
#if defined(__SERVER__) || defined(SIMULATOR)
# ifdef __SERVER__
	Logmsg("WARNING: %s FILE: %s LINE: %ld\n", message, file, line);
# else
	fprintf(gLogFile, "WARNING: %s FILE: %s LINE: %ld\n", message, file, line);
# endif
#else
//	ReportDialog(file,line,message);
#endif
}

void ReportMessage(
	char *file,
	long line,
	char *message)
{
#if defined(__SERVER__) || defined(SIMULATOR)
# ifdef __SERVER__
	Logmsg("MESSAGE: %s FILE: %s LINE: %ld\n", message, file, line);
# else
	fprintf(gLogFile, "MESSAGE: %s FILE: %s LINE: %ld\n", message, file, line);
# endif
#else
//	ReportDialog(file,line,message);
#endif
}

/************************************************************************/

#else	/* POOP */


void ReportError(
	char *file,
	long line,
	char *message)
{

/*
	fprintf(gLogFile, "ERROR: %s FILE: %s LINE: %d\n", message, file, line);
*/

#ifndef unix
#if defined(SIMULATOR) || defined(__SERVER__)
	Debugger();
#else
//	ReportDialog(file,line,message);
#endif
#endif
}


void ReportWarning(
	char *file,
	long line,
	char *message)
{
/*
	fprintf(gLogFile, "WARNING: %s FILE: %s LINE: %d\n", message, file, line);
*/
}

void ReportMessage(
	char *file,
	long line,
	char *message)
{
/*
	fprintf(gLogFile, "MESSAGE: %s FILE: %s LINE: %d\n", message, file, line);
*/
}

/************************************************************************/

#endif	/* POOP */
