
#include "helpers\standards.h"

/*
 *@@ doshQueryFileSize:
 *      returns the size of an already opened file
 *      or 0 upon errors.
 *      Use doshQueryPathSize to query the size of
 *      any file.
 *
 *@@changed V0.9.16 (2001-10-19) [umoeller]: now returning APIRET
 */

APIRET doshQueryFileSize(HFILE hFile,       // in: file handle
                         PULONG pulSize)    // out: file size (ptr can be NULL)
{
    APIRET arc;
    FILESTATUS3 fs3;
    if (!(arc = DosQueryFileInfo(hFile, FIL_STANDARD, &fs3, sizeof(fs3))))
        if (pulSize)
            *pulSize = fs3.cbFile;
    return arc;
}

/*
 *@@ doshQueryPathSize:
 *      returns the size of any file,
 *      or 0 if the file could not be
 *      found.
 *
 *      Use doshQueryFileSize instead to query the
 *      size if you have a HFILE.
 *
 *      Otherwise this returns:
 *
 *      --  ERROR_FILE_NOT_FOUND
 *      --  ERROR_PATH_NOT_FOUND
 *      --  ERROR_SHARING_VIOLATION
 *      --  ERROR_FILENAME_EXCED_RANGE
 *      --  ERROR_INVALID_PARAMETER
 *
 *@@changed V0.9.16 (2001-10-19) [umoeller]: now returning APIRET
 */

APIRET doshQueryPathSize(PCSZ pcszFile,         // in: filename
                         PULONG pulSize)        // out: file size (ptr can be NULL)
{
    APIRET arc;

    if (pcszFile)      // V0.9.16 (2001-12-08) [umoeller]
    {
        FILESTATUS3 fs3;
        if (!(arc = DosQueryPathInfo((PSZ)pcszFile, FIL_STANDARD, &fs3, sizeof(fs3))))
            if (pulSize)
                *pulSize = fs3.cbFile;
    }
    else
        arc = ERROR_INVALID_PARAMETER;

    return arc;
}


/*
 * doshClose:
 *      closes an XFILE opened by doshOpen and
 *      sets *ppFile to NULL.
 *
 *@@added V0.9.16 (2001-10-19) [umoeller]
 */

APIRET doshClose(PXFILE *ppFile)
{
    APIRET arc = NO_ERROR;
    PXFILE pFile;

    if (    (ppFile)
         && (pFile = *ppFile)
       )
    {
        // set the ptr to NULL
        *ppFile = NULL;

        FREE(pFile->pbCache);
        FREE(pFile->pszFilename);

        if (pFile->hf)
        {
            DosSetFileSize(pFile->hf, pFile->cbCurrent);
            DosClose(pFile->hf);
            pFile->hf = NULLHANDLE;
        }

        free(pFile);
    }
    else
        arc = ERROR_INVALID_PARAMETER;

    return arc;
}


/*
 * doshOpen:
 *      wrapper around DosOpen for simpler opening
 *      of files.
 *
 *      ulOpenMode determines the mode to open the
 *      file in (fptr specifies the position after
 *      the open):
 *
 +      +-------------------------+------+------------+-----------+
 +      |  ulOpenMode             | mode | if exists  | if new    |
 +      +-------------------------+------+------------+-----------+
 +      |  XOPEN_READ_EXISTING    | read | opens      | fails     |
 +      |                         |      | fptr = 0   |           |
 +      +-------------------------+------+------------+-----------+
 +      |  XOPEN_READWRITE_EXISTING r/w  | opens      | fails     |
 +      |                         |      | fptr = 0   |           |
 +      +-------------------------+------+------------+-----------+
 +      |  XOPEN_READWRITE_APPEND | r/w  | opens,     | creates   |
 +      |                         |      | appends    |           |
 +      |                         |      | fptr = end | fptr = 0  |
 +      +-------------------------+------+------------+-----------+
 +      |  XOPEN_READWRITE_NEW    | r/w  | replaces   | creates   |
 +      |                         |      | fptr = 0   | fptr = 0  |
 +      +-------------------------+------+------------+-----------+
 *
 *      In addition, you can OR one of the above values with
 *      the XOPEN_BINARY flag:
 *
 *      --  If XOPEN_BINARY is set, no conversion is performed
 *          on read and write.
 *
 *      --  If XOPEN_BINARY is _not_ set, all \n chars are
 *          converted to \r\n on write.
 *
 *      *ppFile receives a new XFILE structure describing
 *      the open file, if NO_ERROR is returned.
 *
 *      The file pointer is then set to the beginning of the
 *      file _unless_ XOPEN_READWRITE_APPEND was specified;
 *      in that case only, the file pointer is set to the
 *      end of the file so data can be appended (see above).
 *
 *      Otherwise this returns:
 *
 *      --  ERROR_FILE_NOT_FOUND
 *      --  ERROR_PATH_NOT_FOUND
 *      --  ERROR_SHARING_VIOLATION
 *      --  ERROR_FILENAME_EXCED_RANGE
 *
 *      --  ERROR_NOT_ENOUGH_MEMORY
 *      --  ERROR_INVALID_PARAMETER
 *
 *@@added V0.9.16 (2001-10-19) [umoeller]
 *@@changed V0.9.16 (2001-12-18) [umoeller]: fixed error codes
 */

APIRET doshOpen(PCSZ pcszFilename,   // in: filename to open
                ULONG flOpenMode,       // in: XOPEN_* mode
                PULONG pcbFile,         // in: new file size (if new file is created)
                                        // out: file size
                PXFILE *ppFile)
{
    APIRET  arc = NO_ERROR;

    ULONG   fsOpenFlags = 0,
            fsOpenMode =    OPEN_FLAGS_FAIL_ON_ERROR
                          | OPEN_FLAGS_NO_LOCALITY
                          | OPEN_FLAGS_NOINHERIT;

    switch (flOpenMode & XOPEN_ACCESS_MASK)
    {
        case XOPEN_READ_EXISTING:
            fsOpenFlags =   OPEN_ACTION_FAIL_IF_NEW
                          | OPEN_ACTION_OPEN_IF_EXISTS;
            fsOpenMode |=   OPEN_SHARE_DENYWRITE
                          | OPEN_ACCESS_READONLY;

            // run this first, because if the file doesn't
            // exists, DosOpen only returns ERROR_OPEN_FAILED,
            // which isn't that meaningful
            // V0.9.16 (2001-12-08) [umoeller]
            arc = doshQueryPathSize(pcszFilename,
                                    pcbFile);
        break;

        case XOPEN_READWRITE_EXISTING:
            fsOpenFlags =   OPEN_ACTION_FAIL_IF_NEW
                          | OPEN_ACTION_OPEN_IF_EXISTS;
            fsOpenMode |=   OPEN_SHARE_DENYWRITE
                          | OPEN_ACCESS_READWRITE;

            arc = doshQueryPathSize(pcszFilename,
                                    pcbFile);
        break;

        case XOPEN_READWRITE_APPEND:
            fsOpenFlags =   OPEN_ACTION_CREATE_IF_NEW
                          | OPEN_ACTION_OPEN_IF_EXISTS;
            fsOpenMode |=   OPEN_SHARE_DENYREADWRITE
                          | OPEN_ACCESS_READWRITE;
            // _Pmpf((__FUNCTION__ ": opening XOPEN_READWRITE_APPEND"));
        break;

        case XOPEN_READWRITE_NEW:
            fsOpenFlags =   OPEN_ACTION_CREATE_IF_NEW
                          | OPEN_ACTION_REPLACE_IF_EXISTS;
            fsOpenMode |=   OPEN_SHARE_DENYREADWRITE
                          | OPEN_ACCESS_READWRITE;
            // _Pmpf((__FUNCTION__ ": opening XOPEN_READWRITE_NEW"));
        break;
    }

    if ((!arc) && fsOpenFlags && pcbFile && ppFile)
    {
        PXFILE pFile;
        if (pFile = NEW(XFILE))
        {
            ULONG ulAction;

            ZERO(pFile);

            // copy open flags
            pFile->flOpenMode = flOpenMode;

            if (!(arc = DosOpen((PSZ)pcszFilename,
                                &pFile->hf,
                                &ulAction,
                                *pcbFile,
                                FILE_ARCHIVED,
                                fsOpenFlags,
                                fsOpenMode,
                                NULL)))       // EAs
            {
                // alright, got the file:

                if (    (ulAction == FILE_EXISTED)
                     && ((flOpenMode & XOPEN_ACCESS_MASK) == XOPEN_READWRITE_APPEND)
                   )
                    // get its size and set ptr to end for append
                    arc = DosSetFilePtr(pFile->hf,
                                        0,
                                        FILE_END,
                                        pcbFile);
                else
                    arc = doshQueryFileSize(pFile->hf,
                                            pcbFile);
                    // file ptr is at beginning

                #ifdef DEBUG_DOSOPEN
                 if (arc)
                    _Pmpf((__FUNCTION__ ": DosSetFilePtr/queryfilesize returned %d for %s",
                                arc, pcszFilename));
                #endif

                // store file size
                pFile->cbInitial
                = pFile->cbCurrent
                = *pcbFile;

                pFile->pszFilename = strdup(pcszFilename);
            }
            #ifdef DEBUG_DOSOPEN
            else
                 _Pmpf((__FUNCTION__ ": DosOpen returned %d for %s",
                             arc, pcszFilename));
            #endif

            if (arc)
                doshClose(&pFile);
            else
                *ppFile = pFile;
        }
        else
            arc = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
        if (!arc)       // V0.9.19 (2002-04-02) [umoeller]
            arc = ERROR_INVALID_PARAMETER;

    return arc;
}


/* *@@ gpihLoadBitmapFile:
 *      this loads the specified bitmap file into
 *      the given HPS. Note that the bitmap is _not_
 *      yet selected into the HPS.
 *
 *      If the file contains only a single bitmap,
 *      this bitmap is used.
 *
 *      If it contains a bitmap array, we use the
 *      "best bitmap" in the array, which is determined
 *      from the following criteria (in this order):
 *
 *      --  a device-dependent bitmap, if its device
 *          resolution is not too large and the given
 *          HPS can display all its colors;
 *
 *      --  a device-dependent bitmap, if its device
 *          resolution is not too large, even if the
 *          given HPS cannot display all its colors;
 *
 *      --  a device-independent bitmap, if the given
 *          HPS can display all its colors;
 *
 *      --  the first device-independent bitmap in
 *          the file;
 *
 *      --  the first bitmap in the file.
 *
 *      Support for bitmap arrays was added with V0.9.19.
 *      I'm not quite sure if the above is the same way
 *      of selecting the "best bitmap" that GpiLoadBitmap
 *      would do, but without any documentation, who is
 *      supposed to know.
 *
 *      Returns:
 *
 *      --  NO_ERROR: *phbm has received new HBITMAP,
 *          to be freed with GpiDeleteBitmap.
 *
 *      --  ERROR_INVALID_PARAMETER
 *
 *      --  ERROR_INVALID_DATA: file exists, but we
 *          can't understand its format.
 *
 *      plus the error codes from doshOpen and DosRead.
 *
 *@@changed V0.9.4 (2000-08-03) [umoeller]: this didn't return NULLHANDLE on errors
 *@@changed V0.9.19 (2002-04-14) [umoeller]: rewritten to support bitmap arrays, prototype changed
 */

APIRET gpihLoadBitmapFile(HBITMAP *phbm,        // out: bitmap if NO_ERROR
                          HPS hps,              // in: HPS for bmp
                          PCSZ pcszBmpFile)     // in: bitmap filename
{
    APIRET arc;
    PXFILE pFile;
    ULONG cbFile = 0;

    if (!hps || !pcszBmpFile || !phbm)
        return ERROR_INVALID_PARAMETER;

    if (!(arc = doshOpen(pcszBmpFile,
                         XOPEN_READ_EXISTING | XOPEN_BINARY,
                         &cbFile,
                         &pFile)))
    {
        PBYTE   pData;
        if (!(pData = (PBYTE)malloc(cbFile)))
            arc = ERROR_NOT_ENOUGH_MEMORY;
        else
        {
            if (!(arc = DosRead(pFile->hf,
                                pData,
                                cbFile,
                                &cbFile)))
            {
                // check bitmap magic codes
                PBITMAPFILEHEADER2  pbfh = (PBITMAPFILEHEADER2)pData;

                switch (pbfh->usType)
                {
                    case BFT_BMAP:     // "BM"
                        // single bitmap in file (no array):
                        if (!(*phbm = GpiCreateBitmap(hps,
                                                      &pbfh->bmp2,
                                                      CBM_INIT,
                                                      (PBYTE)pbfh + pbfh->offBits,
                                                      (PBITMAPINFO2)&pbfh->bmp2)))
                            arc = ERROR_INVALID_DATA;
                    break;

                    case BFT_BITMAPARRAY:   // "BA"
                    {
                        // define a handy union for all the above bullshit
                        typedef union
                        {
                            BITMAPFILEHEADER    Old;
                            BITMAPFILEHEADER2   New;
                        } BMPUNION, *PBMPUNION;

                        PBMPUNION       puFirstDI = NULL,   // first device-independent bitmap
                                        puBestDI = NULL,    // best device-independent bitmap
                                        puFirstDD = NULL,   // first device-dependent bitmap
                                        puBestDD = NULL,    // best device-dependent bitmap
                                        puFirstAny = NULL,  // first bitmap of any type
                                        puUse;

                        // get device resolution for this HPS
                        // so we can select the "best bitmap"
                        #define GET_CAPS_FIRST  CAPS_WIDTH
                        #define GET_CAPS_LAST   CAPS_COLOR_BITCOUNT
                        #define GET_NO_CAPS     GET_CAPS_LAST - GET_CAPS_FIRST + 1

                        LONG alCaps[GET_NO_CAPS];
                        PBITMAPARRAYFILEHEADER2 pba = (PBITMAPARRAYFILEHEADER2)pData;

                        DevQueryCaps(GpiQueryDevice(hps),
                                     GET_CAPS_FIRST,
                                     GET_NO_CAPS,
                                     alCaps);

                        #define BITCOUNT    alCaps[CAPS_COLOR_BITCOUNT - GET_CAPS_FIRST]
                        #define WIDTH       alCaps[CAPS_WIDTH - GET_CAPS_FIRST]
                        #define HEIGHT      alCaps[CAPS_HEIGHT - GET_CAPS_FIRST]

                        // for-all-bitmaps-in-array loop
                        while (pba)
                        {
                            PBMPUNION puThis = (PBMPUNION)&pba->bfh2;

                            LONG cx = 0,
                                 cy,
                                 cBitCount;

                            // ignore this if the type isn't "BM"
                            if (puThis->Old.usType == BFT_BMAP)
                            {
                                // fill the three, but watch out, the offsets are
                                // different between old and new formats
                                if (puThis->Old.bmp.cbFix == sizeof(BITMAPINFOHEADER))
                                {
                                    // old format:
                                    cx = puThis->Old.bmp.cx;
                                    cy = puThis->Old.bmp.cy;
                                    cBitCount = puThis->Old.bmp.cBitCount;
                                }
                                else if (puThis->Old.bmp.cbFix == sizeof(BITMAPINFOHEADER2))
                                {
                                    // new format:
                                    cx = puThis->New.bmp2.cx;
                                    cy = puThis->New.bmp2.cy;
                                    cBitCount = puThis->New.bmp2.cBitCount;
                                }
                            }

                            if (cx)
                            {
                                // store first bitmap of any type
                                if (!puFirstAny)
                                    puFirstAny = puThis;

                                // check device resolution... device-independent
                                // one has cxDisplay and cyDisplay set to 0
                                if (    (!pba->cxDisplay)
                                     && (!pba->cyDisplay)
                                   )
                                {
                                    // device-independent:

                                    // store first device-independent bmp
                                    if (!puFirstDI)
                                        puFirstDI = puThis;

                                    if (cBitCount <= BITCOUNT)
                                        // we can display all the colors:
                                        puBestDI = puThis;
                                }
                                else
                                {
                                    // device-dependent:
                                    // ignore if device resolution is too large
                                    if (    (pba->cxDisplay <= WIDTH)
                                         && (pba->cyDisplay <= HEIGHT)
                                       )
                                    {
                                        if (!puFirstDD)
                                            puFirstDD = puThis;

                                        if (cBitCount <= BITCOUNT)
                                            puBestDD = puThis;
                                    }
                                }
                            } // end if cx

                            // go for next bmp in array
                            if (pba->offNext)
                                // another one coming up:
                                // this ofs is from the beginning of the file
                                pba = (PBITMAPARRAYFILEHEADER2)(pData + pba->offNext);
                            else
                                // no more bitmaps:
                                break;
                        } // end while (pba)

                        if (    (puUse = puBestDD)
                             || (puUse = puFirstDD)
                             || (puUse = puBestDI)
                             || (puUse = puFirstDI)
                             || (puUse = puFirstAny)
                           )
                        {
                            PBITMAPINFOHEADER2 pbih2;
                            PBYTE pbInitData;

                            if (puUse->Old.bmp.cbFix == sizeof(BITMAPINFOHEADER))
                            {
                                // old format:
                                pbih2 = (PBITMAPINFOHEADER2)&puUse->Old.bmp;
                                pbInitData = (PBYTE)pData + puUse->Old.offBits;
                            }
                            else
                            {
                                // new format:
                                pbih2 = &puUse->New.bmp2;
                                pbInitData = (PBYTE)pData + puUse->New.offBits;
                            }

                            if (!(*phbm = GpiCreateBitmap(hps,
                                                          pbih2,
                                                          CBM_INIT,
                                                          pbInitData,
                                                          (PBITMAPINFO2)pbih2)))
                                arc = ERROR_INVALID_DATA;
                        }
                        else
                            arc = ERROR_INVALID_DATA;
                    }
                    break;
                }
            }

            free(pData);
        }

        doshClose(&pFile);
    }

    return arc;
}


