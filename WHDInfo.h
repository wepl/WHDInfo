/*
	WHDInfo.h

	Header file for WHDInfo.
*/

#ifndef _WHDINFO_H_
#define _WHDINFO_H_

#define	AMIGA

#define	PROGNAME	"WHDInfo"
#define	VERSION		"1"
#define	REVISION	"30"
#define	AUTHOR		"Codetapper"
#define	GROUP		"Action"
#define	EMAIL		"codetapper@hotmail.com"
#define	YEAR		"2002"

#ifdef AMIGA
#include <exec/types.h>
#define	ASCII_BLACK	"\x1b[0m"
#define	ASCII_BLUE	"\x1b[33m"
#define	ASCII_WHITE	"\x1b[32m"
#else
#include "exectypes.h"
#define	ASCII_BLACK	""
#define	ASCII_BLUE	""
#define	ASCII_WHITE	""
#endif
#include "endian.h"

/* ===========================================================================
   Slave		Version 1..3
=========================================================================== */

struct WhdloadSlave
{
	ULONG	ws_Security;	//0x70ff4e75	(moveq -1,d0 + rts)
	char	ws_ID[8];	//"WHDLOADS"
	UWORD	ws_Version;	//version of Whdload that is required
	UWORD	ws_Flags;	//see below
	ULONG	ws_BaseMemSize;	//size of mem required by game
				//(must be multiple of $1000, max=$200000)
	ULONG	ws_ExecInstall;	//address in BaseMem where is space for a fake
				//ExecLibrary installed by the WHDLoad to
				//survive a RESET
				//for example $400
				//required are at least 84 Bytes
				//=0 means unsupported
	RPTR	ws_GameLoader;	//start of slave-code
				//will called from WHDLoad after init in
				//SuperVisor
				//slave must be 100.00% PC-RELATIVE !
	RPTR	ws_CurrentDir;	//subdirectory in which WHDLoad should search
				//for files
	RPTR	ws_DontCache;	//pattern string for files which must not cached
				//starting WHDLoad 0.107 this is obsolete

/* ===========================================================================
  additional	Version 4..7
=========================================================================== */

	UBYTE	ws_keydebug;	//raw key code to quit with debug
				//works only if vbr is moved !
				//=0 means no key
	UBYTE	ws_keyexit;	//raw key code to exit
				//works only if vbr is moved !
				//=0 means no key

/* ===========================================================================
   additional	Version 8..9
=========================================================================== */

        LONG   ws_ExpMem;       //size of required expansions memory, during
				//initialisation overwritten by WHDLoad with
				//address of the memory (multiple of $1000)
				//if negative it is optional

/* ===========================================================================
   additional	Version 10
=========================================================================== */

	RPTR	ws_name;	//name of the installed program
	RPTR	ws_copy;	//year and owner of the copyright
	RPTR	ws_info;	//additional informations (author, version...)
};

void printTitle(void);
void printUsage(void);
void printRawKeyName(UBYTE key);
long getFileSize(FILE *filePtr);
void replaceNegOneInString(char *charPtr);
void showWHDInfo(char *fileName);

#endif	/* _WHDINFO_H_ */
