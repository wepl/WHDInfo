#ifndef ENDIAN_H
#define ENDIAN_H

#include "exectypes.h"

/* Endian modes */

#define	ENDIAN_BIG	TRUE
#define	ENDIAN_LITTLE	FALSE

/* Function prototypes */

BOOL InitEndian(VOID);
LONG GetLONG(LONG *ptr, BYTE endianMode);
ULONG GetULONG(ULONG *ptr, BYTE endianMode);
WORD GetWORD(WORD *ptr, BYTE endianMode);
UWORD GetUWORD(UWORD *ptr, BYTE endianMode);
VOID PutLONG(LONG *ptr, LONG val, BYTE endianMode);
VOID PutULONG(ULONG *ptr, ULONG val, BYTE endianMode);
VOID PutWORD(WORD *ptr, WORD val, BYTE endianMode);
VOID PutUWORD(UWORD *ptr, UWORD val, BYTE endianMode);

#endif
