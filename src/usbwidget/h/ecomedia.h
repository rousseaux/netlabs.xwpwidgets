/****************************** Module Header ******************************
*
*   (c) Copyright eCoSoftware 2006-2009. All Rights Reserved
*
*   Module Name: ECOMEDIA.H
*
*   Media library include file.
*
\***************************************************************************/
/* ECOMEDIA.H */
#ifndef  ECOMEDIA_H_INCLUDED
#define  ECOMEDIA_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <mmioos2.h>

#pragma pack(1)

/***********************************************************************************************/
/*                                                                                             */
/* Data types and macro definitions for a new bitmap format HBITMAP2 written by Yagiza         */
/*                                                                                             */
/***********************************************************************************************/
/* Public types definition */

typedef LHANDLE HBITMAP2; // New bitmap handle

/* New bitmap header structure */
struct _BITMAP2HEADER;
typedef struct _BITMAP2HEADER BITMAP2HEADER, *PBITMAP2HEADER;
struct _BITMAP2HEADER
{
    ULONG   cb;         // bytes count
    USHORT  usCx;       // bitmmap width
    USHORT  usCy;       // bitmap height
    BYTE    cChannels;  // Number of color channels: 3 - RGB, 4 - RGBA
    BYTE    cBits;      // Bits per pixel: 8 is the only valid value for now
};

/* New bitmap structure */
struct _BITMAP2;
typedef struct _BITMAP2 BITMAP2, *PBITMAP2;
struct _BITMAP2
{
    ULONG   cb;                 // bytes count
    USHORT  usCx;               // bitmmap width
    USHORT  usCy;               // bitmap height
    BYTE    cChannels;          // Number of color channels: 3 - RGB, 4 - RGBA
    BYTE    cBits;              // Bits per pixel: 8 is the only valid value for now
    BYTE    cBitmapData[1]; // Bitmap data (each row padded to 4-byte alignment)
};

/* Error codes, returned by Gpi2GetLastError() function */
#define GPI2ERR_BASE                        0xB000
#define GPI2ERR_INV_HBITMAP2                GPI2ERR_BASE+0x0001
#define GPI2ERR_INV_BITMAP_FORMAT           GPI2ERR_BASE+0x0002
#define GPI2ERR_INV_DIMENSION               GPI2ERR_BASE+0x0003
#define GPI2ERR_INV_PARAMETER               GPI2ERR_BASE+0x0004
#define GPI2ERR_MEMORY_ALLOCATION_FAILED    GPI2ERR_BASE+0x0005
#define GPI2ERR_MEMORY_SET_FAILED           GPI2ERR_BASE+0x0006
#define GPI2ERR_INV_PNG_FORMAT                          GPI2ERR_BASE+0x0007
#define GPI2ERR_CANNOT_READ_PNG             GPI2ERR_BASE+0x0008

/***********************************************************************************************/
/*                                                                                             */
/* End of data types and macro definitions for a new bitmap format HBITMAP2 written by Yagiza  */
/*                                                                                             */
/***********************************************************************************************/

#define BGD_DECLARE(rt) extern rt
// _Export
// #define BGD_DECLARE(rt)  extern rt _Export




/* Obsoled! DLL ENTRY NAME "LoadBitmap" ORDINAL 1 */
#define LoadBitmap Gpi2LoadBitmap

/* DLL ENTRY NAME "Gpi2LoadBitmap" ORDINAL 2 */
BGD_DECLARE(HBITMAP) APIENTRY Gpi2LoadBitmap ( HAB hab, HDC hdc, HPS *hps, PSZ pszFileName );

/* DLL ENTRY NAME "Gpi2GetBitmap" ORDINAL 3 */
BGD_DECLARE(HBITMAP) APIENTRY Gpi2GetBitmap (HWND hwnd, PSZ pszFileName);
BGD_DECLARE(APIRET) APIENTRY Gpi2QueryProportionBox(int W, int H, int w, int h, int *picw, int *pich);
BGD_DECLARE(APIRET) APIENTRY Gpi2SaveScreenshot(char *pszFileName, char *ext, char *format, ULONG width, ULONG height, int src_bytesperpixel, void**  src_buf, FOURCC fccSrcFormat);

/*****************************************************************************
 Gpi2ScaleBitmap error codes
*****************************************************************************/
#define BMSERR_OK                       0
#define BMSERR_BASE                     0x8500
#define BMSERR_INVALID_HANDLE           (BMSERR_BASE+ 1)
#define BMSERR_UNKNOWN_FILTER_TYPE      (BMSERR_BASE+ 2)
#define BMSERR_WRONG_IMAGE_TYPE         (BMSERR_BASE+ 3)
#define BMSERR_NO_MEMORY                (BMSERR_BASE+ 4)
#define BMSERR_SELECT_BITMAP            (BMSERR_BASE+ 5)
#define BMSERR_QUERY_BITS               (BMSERR_BASE+ 6)
#define BMSERR_SET_BITS                 (BMSERR_BASE+ 7)
#define BMSERR_CREATE_IMAGE             (BMSERR_BASE+ 8)
#define BMSERR_INVALID_SIZE             (BMSERR_BASE+ 9)


/**
 * Gpi2ScaleBitmap()
 *
 * !!! D E P R E C A T E D !!!
 * It is strongly not recommended to use this function!!!
 * Use Gpi2ScaleBitmap2() instead!
 *
 *  Creates a copy of a bitmap and scale it to specified size
 *  Parameters:
 *   hps     - presentation space handle
 *   hbm     - valid handle of bitmap to scale
 *   phdstbm - pointer to store created bitmap handle
 *   width   - image width to set
 *   height  - image height to set
 *   keep    - keep aspect ratio flag
 *  Return values:
 *   0 or error code (BMSERR_...), created bitmap handle at phdstbm
 */

BGD_DECLARE(APIRET) APIENTRY Gpi2ScaleBitmap
(
        HPS             hps,
        HBITMAP         hbm,
        PHBITMAP        phdstbm,
        ULONG           width,
        ULONG           height,
        BOOL            keep
);

/**
 * Scale BITMAP2
 *  Creates a copy of a bitmap and scale it to specified size
 *  Parameters:
 *   hbitmap2 - Source BITMAP2 handle to scale
 *   width    - image width to set
 *   height   - image height to set
 *   bKeep     - keep aspect ratio flag
 *  Return values:
 *   BITMAP2 handle of scaled bitmap or NULLHANDLE if an error occured
 *   Use Gpi2GetLastError() to retreive an error code
 */

BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2ScaleBitmap2(HBITMAP2 hbitmap2, ULONG width, ULONG height, BOOL bKeep);


/*****************************************************************************
 Filter types
*****************************************************************************/
#define BMFT_SHARPEN    1       /* Sharpen filter   */
#define BMFT_GRAYSCALE  2       /* Grayscale filter */

/*****************************************************************************
 Filter error codes
*****************************************************************************/
#define BMFERR_OK                       0
#define BMFERR_BASE                     0x5F00
#define BMFERR_INVALID_HANDLE           (BMFERR_BASE+ 1)
#define BMFERR_UNKNOWN_FILTER_TYPE      (BMFERR_BASE+ 2)
#define BMFERR_WRONG_IMAGE_TYPE (BMFERR_BASE+ 3)
#define BMFERR_NO_MEMORY                (BMFERR_BASE+ 4)
#define BMFERR_SELECT_BITMAP            (BMFERR_BASE+ 5)
#define BMFERR_QUERY_BITS               (BMFERR_BASE+ 6)
#define BMFERR_SET_BITS         (BMFERR_BASE+ 7)

/*****************************************************************************
 Functions
*****************************************************************************/

/**
 * Filter
 *
 * !!! D E P R E C A T E D !!!
 * It is strongly not recommended to use this function!!!
 *
 *  Applies filter onto a specified bitmap image
 *  Parameters:
 *   hps   - presentation space handle
 *   hbm   - valid handle of bitmap to filter
 *   ftype - type of filter to be applied (BMFT_...)
 *  Return values:
 *   0 or error code (BMFERR_...)
 */
BGD_DECLARE(APIRET) APIENTRY Gpi2FilterBitmap (HPS hps, HBITMAP hbm, ULONG ftype);

/*****************************************************************************
 Version check by Yagiza
*****************************************************************************/
BGD_DECLARE(VOID) APIENTRY Gpi2GetVersion(PULONG pulMajor, PULONG pulMinor);
BGD_DECLARE(USHORT) APIENTRY Gpi2GetVersionString(PSZ pszBuffer, USHORT cbBufLen);

/*****************************************************************************
 PNG-related routines by Yagiza
*****************************************************************************/
#define RT_PNG  400
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2ReadPng(PBYTE pPngData, ULONG ulBackgroundColor, ULONG flOptions);
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2LoadPng(HMODULE hmod, ULONG ulId, ULONG ulColor, ULONG flOptions);

/*****************************************************************************
 OS/2-Icon-related routines by Yagiza
*****************************************************************************/
BGD_DECLARE(ULONG)              APIENTRY Gpi2GetIconData(PICONINFO pIconInfo, PVOID* pIconData);
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2ReadBitmap(PVOID pIconData, PULONG pulCx, PULONG pulCy, USHORT usBitCount, ULONG ulBckgColor, ULONG ulColor0, ULONG ulColor1, ULONG flOptions);
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2ParseIconInfo(PICONINFO pIconInfo, PULONG pulCx, PULONG pulCy, USHORT usBitCount, ULONG ulBckgColor, ULONG ulColor0, ULONG ulColor1, ULONG flOptions);

/*****************************************************************************
 Alpha-blending routines by Yagiza
*****************************************************************************/
BGD_DECLARE(BOOL)       APIENTRY Gpi2DrawBitmap(HPS hpsTarget, HBITMAP2 hbm2Source, LONG lCount, PPOINTL aptlPoints);
BGD_DECLARE(BOOL)       APIENTRY Gpi2BitBlt(HBITMAP2 hbm2Target, HBITMAP2 hbm2Source, LONG lCount, PPOINTL aptlPoints, ULONG ulBckgColor, ULONG flOptions);

/*****************************************************************************
 New bitmap routines by Yagiza
*****************************************************************************/
#define FL_STRIPALPHA   0x01    // "Ignore alpha channel"
#define FL_PROCESSALPHA 0x02    // Process alpha by mixing image with specified background color
#define FL_COPYALPHA    0x04    // Copy alpha channel when blitting
#define FL_USEBKGD              0x08    // Use bKGD PNG chunk to determine background color.

BGD_DECLARE(APIRET)             APIENTRY Gpi2GetLastError();
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2CreateBitmap(USHORT usCx, USHORT usCy, BYTE cChannels, BYTE cBits, PBYTE pbBitmapData);
BGD_DECLARE(BOOL)               APIENTRY Gpi2DeleteBitmap(HBITMAP2 hbitmap2);
BGD_DECLARE(BOOL)               APIENTRY Gpi2QueryBitmapInfo(HBITMAP2 hbitmap2, PBITMAP2HEADER pbmp2header);
BGD_DECLARE(BOOL)               APIENTRY Gpi2QueryBitmapBits(HBITMAP2 hbitmap2, LONG lScanStart, LONG lScans, PBYTE pbBuffer);
BGD_DECLARE(BOOL)               APIENTRY Gpi2SetBitmapBits(HBITMAP2 hbitmap2, LONG lScanStart, LONG lScans, PBYTE pbBuffer);
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2CopyBitmap(HBITMAP2 hbitmap2);
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2CropBitmap(HBITMAP2 hbitmap2, PRECTL prcl);
BGD_DECLARE(HBITMAP2)   APIENTRY Gpi2Bitmap2FromBitmap(HBITMAP hbmp);
BGD_DECLARE(HBITMAP)    APIENTRY Gpi2BitmapFromBitmap2(HBITMAP2 hbmp2, ULONG ulColor, ULONG flFlags);

#pragma pack()

#ifdef __cplusplus
}
#endif
#endif /* ECOMEDIA_H_INCLUDED */
