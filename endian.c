#include "endian.h"

static UBYTE bigEndian = 0xff;			/* Default to big endian format */

/****************************************************************************
 * FUNCTION			InitEndian
 *
 * Flag the native endian format of the host machine.
 *
 * INPUTS
 *
 * None.
 *
 * OUTPUTS
 *
 * Returns TRUE if the host machine uses big endian ("Motorola") format,
 * or FALSE if it uses little endian ("Intel") format.
 *
 * NOTES
 *
 * You must call this function prior to calling any of the Get or Put
 * functions.
 */
BOOL InitEndian(VOID)
{
	UWORD	test = 0xff00;
	
	return (bigEndian = *((UBYTE *)&test));
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			GetLONG
 *
 * Return a signed longword while accounting for endian differences.
 *
 * INPUTS
 *
 * ptr				Pointer to LONG data value.
 *
 * endianMode		The endian format of the data value:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * Returns a LONG in the host's native endian format.
 */
LONG GetLONG(LONG *ptr, BYTE endianMode)
{
	return (LONG)GetULONG((ULONG *)ptr, endianMode);
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			GetULONG
 *
 * Return an unsigned longword while accounting for endian differences.
 *
 * INPUTS
 *
 * ptr				Pointer to ULONG data value.
 *
 * endianMode		The endian format of the data value:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * Returns a ULONG in the host's native endian format.
 */
ULONG GetULONG(ULONG *ptr, BYTE endianMode)
{
    ULONG   val;

	if (bigEndian)
	{
		if (endianMode)					/* ENDIAN_BIG = TRUE */
			return *ptr;
	}

	else
	{
		if (!endianMode)				/* ENDIAN_LITTLE = FALSE */
			return *ptr;
	}

	val = *ptr;
	return (ULONG)((val << 24) | ((val << 8) & 0x00ff0000) | ((val >> 8) & 0x0000ff00) | (val >> 24));

#if 0
	*((UBYTE *)&val + 3) = *((UBYTE *)ptr);
	*((UBYTE *)&val + 2) = *((UBYTE *)ptr + 1);
	*((UBYTE *)&val + 1) = *((UBYTE *)ptr + 2);
	*((UBYTE *)&val) = *((UBYTE *)ptr + 3);

	return val;
#endif
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			GetWORD
 *
 * Return a signed word while accounting for endian differences.
 *
 * INPUTS
 *
 * ptr				Pointer to WORD data value.
 *
 * endianMode		The endian format of the data value:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * Returns a WORD in the host's native endian format.
 */
WORD GetWORD(WORD *ptr, BYTE endianMode)
{
	return (WORD)GetUWORD((UWORD *)ptr, endianMode);
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			GetUWORD
 *
 * Return an unsigned word while accounting for endian differences.
 *
 * INPUTS
 *
 * ptr				Pointer to UWORD data value.
 *
 * endianMode		The endian format of the data value:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * Returns a UWORD in the host's native endian format.
 */
UWORD GetUWORD(UWORD *ptr, BYTE endianMode)
{
    UWORD   val;

	if (bigEndian)
	{
		if (endianMode)					/* ENDIAN_BIG = TRUE */
			return *ptr;
	}

	else
	{
		if (!endianMode)				/* ENDIAN_LITTLE = FALSE */
			return *ptr;
	}

	val = *ptr;
	return (UWORD)((val << 8) | (val >> 8));
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			PutLONG
 *
 * Store a signed longword in the required endian format.
 *
 * INPUTS
 *
 * ptr				Pointer to the address to store the LONG data.
 *
 * val				The value to store in native endian format.
 *
 * endianMode		The endian format to store the data value in:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * None.
 */
VOID PutLONG(LONG *ptr, LONG val, BYTE endianMode)
{
	PutULONG((ULONG *)ptr, (ULONG)val, endianMode);
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			PutULONG
 *
 * Store an unsigned longword in the required endian format.
 *
 * INPUTS
 *
 * ptr				Pointer to the address to store the ULONG data.
 *
 * val				The value to store in native endian format.
 *
 * endianMode		The endian format to store the data value in:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * None.
 */
VOID PutULONG(ULONG *ptr, ULONG val, BYTE endianMode)
{
	if (bigEndian)
	{
		if (endianMode)					/* ENDIAN_BIG = TRUE */
		{
			*ptr = val;
			return;
		}
	}

	else
	{
		if (!endianMode)				/* ENDIAN_LITTLE = FALSE */
		{
			*ptr = val;
			return;
		}
	}

	*ptr = ((val << 24) | ((val << 8) & 0x00ff0000) | ((val >> 8) & 0x0000ff00) | (val >> 24));
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			PutWORD
 *
 * Store a signed word in the required endian format.
 *
 * INPUTS
 *
 * ptr				Pointer to the address to store the WORD data.
 *
 * val				The value to store in native endian format.
 *
 * endianMode		The endian format to store the data value in:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * None.
 */
VOID PutWORD(WORD *ptr, WORD val, BYTE endianMode)
{
	PutUWORD((UWORD *)ptr, (UWORD)val, endianMode);
}
/***************************************************************************/



/****************************************************************************
 * FUNCTION			PutUWORD
 *
 * Store an unsigned word in the required endian format.
 *
 * INPUTS
 *
 * ptr				Pointer to the address to store the UWORD data.
 *
 * val				The value to store in native endian format.
 *
 * endianMode		The endian format to store the data value in:
 *
 *					ENDIAN_BIG
 *					ENDIAN_LITTLE
 *
 * OUTPUTS
 *
 * None.
 */
VOID PutUWORD(UWORD *ptr, UWORD val, BYTE endianMode)
{
	if (bigEndian)
	{
		if (endianMode)					/* ENDIAN_BIG = TRUE */
		{
			*ptr = val;
			return;
		}
	}

	else
	{
		if (!endianMode)				/* ENDIAN_LITTLE = FALSE */
		{
			*ptr = val;
			return;
		}
	}

	*ptr = ((val << 8) | (val >> 8));
}
/***************************************************************************/
