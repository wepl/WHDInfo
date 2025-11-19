/*
 *	WHDInfo.c by Codetapper/Action! (codetapper@hotmail.com)
 *
 *	Outputs information about a WHDLoad slave.
 *
 * 0.01 - Beta release (Action members only).
 *      - Correctly supports slaves up to v5.
 *
 * 1.00 - Supports all information in slaves up to v10.
 *      - Displays version string (if one exists).
 *      - Released 21st August 1999.
 *
 * 1.10 - Supports all information in slaves up to v11.
 *      - Optional expansion memory is detected and displayed.
 *      - The special character -1 (which leaves a gap of 1.5 lines) is
 *        correctly handled in the information tag (thanks Bert!)
 *      - Added a fairly crude Kickstart 1.3 Rom detection for slaves.
 *      - Released 18th April 2000.
 *
 * 1.20 - Supports all information in slaves up to v13.
 *      - Fix faulty function keys for debug/quit.
 *      - Flags shown in more useful hex and decimal.
 *      - Released 2nd January 2001.
 *
 * 1.30 - Supports slaves up to v15.
 *      - Fixed bug with flag WHDLF_ClearMem not appearing.
 *      - Added a fairly crude Kickstart 3.1 Rom detection for slaves.
 *      - New resload_Examine/ExNext flag support added.
 *      - Released 1st February 2002.
 *
 * 1.31 - Supports slaves up to v16.
 *	- scancheck for kick30 replaced with kick12
 *	- misc changes to avoid compiler warnings
 *
 * 1.32 (27.07.2011) Wepl
 *	- Supports slaves up to v17
 *
 * 1.33 (21.11.2021) Wepl
 *	- fix detection of WHDLoad Slaves, as only one hunk is
 *	  supported the struct will always start at fixed position 32
 *
 * 1.34 (19.11.2025) Wepl
 *	- use standard whdload.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "endian.h"
#include "whdload.h"

#define	PROGNAME	"WHDInfo"
#define	VERSION		"1"
#define	REVISION	"34"
#define	AUTHOR		"Codetapper, Wepl"
#define	DATE		"19.11.2025"

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

#ifdef AMIGA
static const char *versionstring = "$VER: " PROGNAME " " VERSION "." REVISION " (" DATE ")";
#endif

char *progName = NULL;

char *ws_Flags_Descriptions[] = {
	"Diskimages used by slave (obsolete)",
	"All errors quit WHDLoad",
	"Traps are emulated like autovectors",
	"Ignore division by zero exceptions",
	"Abort if MC68020+ is unavailable",
	"Abort if AGA chipset is unavailable",
	"Don't get the keycode in conjunction with NoVBRMove",
	"Forward \"line-a\" exceptions to handler",
	"Forward \"trapv\" exceptions to handler",
	"Forward \"chk, chk2\" exceptions to handler",
	"Forward \"privilege violation\" exceptions to handler",
	"Forward \"line-f\" exceptions to handler",
	"Clear base and expansion memory with 0",
	"Uses resload_Examine/ExNext",
	"Forward \"division by zero\" exceptions to handler",
	"Forward \"illegal instruction\" exceptions to handler",
};

void printTitle(void)
{
	printf(ASCII_WHITE PROGNAME ASCII_BLACK " v" VERSION "." REVISION " (" DATE ")"
	" by " ASCII_BLUE AUTHOR ASCII_BLACK "\n\n");
}

void printUsage(void)
{
	printf("This program is used to display information about WHDInfo slaves used\n"
	       "in hundreds of HD installers. With this utility, you can quickly view\n"
	       "ALL the slave information including WHDLoad version required, memory,\n"
	       "slave version, copyright information... This is very handy to check you\n"
	       "have the latest slave, finding the quit key, and for installer writers.\n\n");

	printf("Usage: %s <slave>\n\n",progName);

	printf("Examples: %s Exterminator.slave\n"
		   "          %s ram:InterphaseHD/Interphase.slave\n\n",progName,progName);

	printf("Greetings to all Action members, Tachyon for his endian source, all the\n"
	       "weekly IRC guys and of course Bert Jahn for WHDLoad! Visit the Action\n"
	       "website at http://zap.to/action/ for more of our productions!\n\n");
}

void printRawKeyName(UBYTE key)
{
	switch ((int) key)
	{
	case 0:
		printf("None\n");
		break;
	case 0x2b:
		printf("Blank key beside Return (A1200)\n");
		break;
	case 0x41:
		printf("Backspace\n");
		break;
	case 0x42:
		printf("Tab\n");
		break;
	case 0x43:
		printf("Enter (numeric keypad)\n");
		break;
	case 0x44:
		printf("Return\n");
		break;
	case 0x45:
		printf("Escape\n");
		break;
	case 0x46:
		printf("Del\n");
		break;
	case 0x5a:
		printf("NumL (numeric keypad)\n");
		break;
	case 0x5b:
		printf("ScrL (numeric keypad)\n");
		break;
	case 0x5c:
		printf("SysRq (numeric keypad)\n");
		break;
	case 0x5d:
		printf("PrtSc (numeric keypad)\n");
		break;
	case 0x5f:
		printf("Help\n");
		break;
	case 0x62:
		printf("Caps Lock\n");
		break;
	case 0x63:
		printf("Ctrl\n");
		break;
	case 0x64:
		printf("Left Alt\n");
		break;
	case 0x65:
		printf("Right Alt\n");
		break;
	default:
		if ((key >= 0x50) && (key <= 0x59))
		{
			printf("F%d\n",key-0x4f);
		}
		else
		{
			printf("Unknown key! ($%x)\n",key);
		}
	}
}

long getFileSize(FILE *filePtr)
{
	long	fileSize;

	if (fseek(filePtr,0,SEEK_END) == -1)
	{
		printf("Couldn't determine filesize!\n\n");
		return(-1);
	}

	fileSize = ftell(filePtr);

	if (fseek(filePtr,0,SEEK_SET) == -1)
	{
		printf("Couldn't seek to beginning of file!\n\n");
		return(-1);
	}
	return(fileSize);
}

/* ===================================================================
	memfind

	a = memory to search
	b = memory block to find
	n = number of bytes in search memory block
	len = length of memory block

	This function finds the first occurence of a character in a
	block of memory.

	This function does not terminate at a NULL byte.  It always
	searches n bytes.
=================================================================== */
UBYTE *memfind(UBYTE *a, char *b, long n, long len)
{
	long	 remaining, precalc = (len - n) + 1;
	UBYTE	*ptr = a;

	for(;;)
	{
		remaining = precalc - (ptr - a);
		ptr = memchr(ptr,(int) *b,remaining);
		if (ptr != NULL)
		{
			if (memcmp(ptr,b,n) == 0)
			{
				return ptr;
			}
			else
			{
				ptr++;
			}
		}
		else
		{
			break;
		}
	}
	return (NULL);
}

void replaceNegOneInString(char *charPtr)
{
	while (*charPtr != '\0')
	{
		if (*charPtr == -1)
		{
			*charPtr = 0x0a;
		}
		charPtr++;
	}
}

/* ===================================================================
	Main
=================================================================== */

void showWHDInfo(char *fileName);

int main(int argc, char **argv)
{
	progName = argv[0];
	printTitle();

	if (argc <= 1)
	{
		printUsage();
	}
	else
	{
		showWHDInfo(argv[1]);
	}
	return(0);
}

void showWHDInfo(char *fileName)
{
	struct WHDLoadSlave	 slave;
	FILE			*filePtr;
	long			 filePos = 32, fileSize, counter;
	UBYTE			*memPtr = NULL, *vers;
	UWORD			w, *wp;
	char			*kickstart12 = "33180";
	char			*kickstart13 = "34005";
	char			*kickstart31 = "40068";
	const char		*noslave = "File is NOT a WHDLoad Slave!\n\n";

	InitEndian();

	filePtr = fopen(fileName,"rb");
	if (filePtr == NULL)
	{
		printf("Couldn't open file %s\n\n",fileName);
	} else {
		fileSize = getFileSize(filePtr);
		if (fileSize < filePos + sizeof(struct WHDLoadSlave)) {
			puts(noslave);
		} else {
			memPtr = calloc(1,fileSize+1);
			if (memPtr == NULL)
			{
				printf("Couldn't allocate enough memory!\n\n");
			} else {
				if (fread(memPtr, fileSize, 1, filePtr) != 1)
				{
					printf("Couldn't read file %s\n\n",fileName);
				} else {
					
					/* check if it is a WHDLoad Slave file */
					if (strncmp((char*)memPtr+filePos+4,"WHDLOADS",8)) {
						puts(noslave);
					} else {
						/* Copy from memory into the slave structure */
						memcpy(&slave,memPtr+filePos,sizeof(struct WHDLoadSlave));

						/* Fix endian problems */
						slave.ws_Version = GetUWORD(&slave.ws_Version, ENDIAN_BIG);
						slave.ws_Flags = GetUWORD(&slave.ws_Flags, ENDIAN_BIG);
						slave.ws_BaseMemSize = GetULONG(&slave.ws_BaseMemSize, ENDIAN_BIG);
						slave.ws_ExecInstall = GetULONG(&slave.ws_ExecInstall, ENDIAN_BIG);
						slave.ws_GameLoader = GetUWORD(&slave.ws_GameLoader, ENDIAN_BIG);
						slave.ws_CurrentDir = GetUWORD(&slave.ws_CurrentDir, ENDIAN_BIG);
						slave.ws_DontCache = GetUWORD(&slave.ws_DontCache, ENDIAN_BIG);

						if (slave.ws_Version >= 8)
						{
							slave.ws_ExpMem = GetLONG(&slave.ws_ExpMem, ENDIAN_BIG);
						}

						if (slave.ws_Version >= 10)
						{
							slave.ws_name = GetUWORD(&slave.ws_name, ENDIAN_BIG);
							slave.ws_copy = GetUWORD(&slave.ws_copy, ENDIAN_BIG);
							slave.ws_info = GetUWORD(&slave.ws_info, ENDIAN_BIG);

							if (slave.ws_name != 0)
							{
								printf("%s ",memPtr+filePos+slave.ws_name);
							}
							if (slave.ws_copy != 0)
							{
								printf("(c) %s",memPtr+filePos+slave.ws_copy);
							}
							if ((slave.ws_name != 0) || (slave.ws_copy != 0))
							{
								printf("\n");
							}
							if (slave.ws_info != 0)
							{
								replaceNegOneInString((char*)memPtr+filePos+slave.ws_info);
								printf("%s\n\n",memPtr+filePos+slave.ws_info);
							}
						}

						if (slave.ws_Version >= 16)
						{
							slave.ws_kickname = GetUWORD(&slave.ws_kickname, ENDIAN_BIG);
							slave.ws_kicksize = GetULONG(&slave.ws_kicksize, ENDIAN_BIG);
							slave.ws_kickcrc = GetUWORD(&slave.ws_kickcrc, ENDIAN_BIG);
						}

						if (slave.ws_Version >= 17)
						{
							slave.ws_config = GetUWORD(&slave.ws_config, ENDIAN_BIG);
						}

						vers = memPtr;
						for (;;)
						{
							vers = memchr(vers,'$',fileSize-(vers-memPtr));
							if (vers == NULL) break;
							if ((vers[1] == 'V') && (vers[2] == 'E') && (vers[3] == 'R') && (vers[4] == ':'))
							{
								vers += 5;
								if (vers[0] == ' ') vers++;
								printf("%s\n\n",vers);
								break;
							}
							else
							{
								vers++;
							}
						}

						printf("         Slave name: %s\n",fileName);
						printf("         Slave size: %ld bytes\n",fileSize);

						printf("    WHDLoad Version: %d\n",slave.ws_Version);
						printf("              Flags: $%x (%d)\n",slave.ws_Flags,slave.ws_Flags);
						if (slave.ws_Flags > 0)
						{
							for (counter = 0; counter < sizeof(ws_Flags_Descriptions)/sizeof(char *); counter++)
							{
								if (slave.ws_Flags & 1<<counter)
								{
									printf("                     %s\n",ws_Flags_Descriptions[counter]);
								}
							}
						}
						printf("        Base memory: $%lx (%ld KB)\n",slave.ws_BaseMemSize,slave.ws_BaseMemSize>>10);

						printf("Fake Exec installed: ");
						if (slave.ws_ExecInstall == 0)
						{
							printf("None\n");
						}
						else
						{
							printf("$%lx\n",slave.ws_ExecInstall);
						}

						printf("        Game Loader: $%x ($%lx bytes from start of file)\n",slave.ws_GameLoader,filePos+slave.ws_GameLoader);
		
						printf("   Subdir for files: ");
						if (slave.ws_CurrentDir == 0)
						{
							printf("None\n");
						}
						else
						{
							printf("'%s'\n",memPtr+filePos+slave.ws_CurrentDir);
						}

						if (slave.ws_DontCache == 0)
						{
							printf("            Caching: All files\n");
						}
						else
						{
							printf("   Files not cached: '%s'\n",memPtr+filePos+slave.ws_DontCache);
						}
		
						if (slave.ws_Version >= 4)
						{
							printf("          Debug key: ");
							printRawKeyName(slave.ws_keydebug);
							printf("           Exit key: ");
							printRawKeyName(slave.ws_keyexit);
						}
						
						if (slave.ws_Version >= 8)
						{
							printf("   Expansion memory: ");
							if (slave.ws_ExpMem == 0)
							{
								printf("None\n");
							}
							else
							{
								if (slave.ws_ExpMem < 0) {
									printf("$%lx (%ld KB) optional\n",-slave.ws_ExpMem,-slave.ws_ExpMem>>10);
								} else {
									printf("$%lx (%ld KB)\n",slave.ws_ExpMem,slave.ws_ExpMem>>10);
								}
							}
						}

						if (slave.ws_Version >= 16) {
							printf("     Kickstart name: ");
							if (slave.ws_kickcrc == 0xffff) {
								wp = (UWORD*) (memPtr+filePos+slave.ws_kickname);
								while (*wp++) {
									w = GetUWORD(wp++, ENDIAN_BIG);
									printf("'%s' ",memPtr+filePos+w);
								}
							} else {
								if (slave.ws_kickname) {
									printf("'%s'",(char*)memPtr+filePos+slave.ws_kickname);
								} else {
									printf("None");
								}
							}
							printf("\n     Kickstart size: $%lx",slave.ws_kicksize);
							if (slave.ws_kicksize) { printf(" (%ld KB)",slave.ws_kicksize>>10); }
							printf("\n      Kickstart crc: ");
							if (slave.ws_kickcrc == 0xffff) {
								wp = (UWORD*) (memPtr+filePos+slave.ws_kickname);
								while (*wp) {
									w = GetUWORD(wp++, ENDIAN_BIG);
									printf("$%x ",w);
									wp++;
								}
							} else {
								printf("$%x",slave.ws_kickcrc);
							}
							printf("\n");
						}

						if (slave.ws_Version >= 17) {
							char saved, *str, *next, *prefix;
							prefix = "      Configuration: ";
							if (slave.ws_config == 0) {
								printf("%sNone\n",prefix);
							} else {
								str = (char*) memPtr+filePos+slave.ws_config;
								while ((next = strstr(str, ";"))) {
									saved = *++next;
									*next = 0;
									printf("%s%s\n",prefix,str);
									*next = saved;
									str = next;
									prefix = "                     ";
								}
								printf("%s%s\n",prefix,str);
							}
						}

						if (slave.ws_Version >= 20) {
							char *prefix = "Alternate Memory:";
							int ws_MemConfig = GetUWORD(&slave.ws_MemConfig, ENDIAN_BIG);
							if (ws_MemConfig == 0) {
								printf("%20s None\n",prefix);
							} else {
								char *optional;
								int count = 1;
								ULONG c, *p;
								LONG f;
								p = (ULONG*) (memPtr+filePos+ws_MemConfig);
								while ((c = GetULONG(p++, ENDIAN_BIG))) {
									f = GetLONG(p++, ENDIAN_BIG);
									if (f < 0) {
										optional = " optional";
										f = -f;
									} else {
										optional = "";
									}
									printf("%20s %d. Base = $%lx (%ld KB), Exp = $%lx (%ld KB)%s\n",
										prefix, count++, c, c>>10, f, f>>10, optional);
									prefix = "";
								}
							}
						}

						if (memfind(memPtr, kickstart12, strlen(kickstart12), fileSize) != NULL)
						{
							printf("        Information: Slave requires kickstart 1.2 image!\n");
						}
						if (memfind(memPtr, kickstart13, strlen(kickstart13), fileSize) != NULL)
						{
							printf("        Information: Slave requires kickstart 1.3 image!\n");
						}
						if (memfind(memPtr, kickstart31, strlen(kickstart31), fileSize) != NULL)
						{
							printf("        Information: Slave requires kickstart 3.1 image!\n");
						}
						printf("\n");
					}
				}
			}
		}	
	}
	
	/* Close everything we opened */
	if (memPtr) free(memPtr);
	if (filePtr) fclose(filePtr);
}
