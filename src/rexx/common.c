
/*
 *@@ RwgtEncodeString:
 *      encode a string so that it can safely be included in a
 *      setup string.  ASCII codes between 0 and 31 as well as
 *      ';', '=', and '%' are encoded as '%xx' where xx is the
 *      hexadecimal value of the character in upper case.
 *
 *      Use RwgtFree to release the returned value.
 *
 *@@added V0.1.0 (2001-01-27) [lafaix]
 */

int iscntrl(int c) { return ((c >= 0) && (c <= 31)); }
int isxdigit(int c) { return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')); }
int isdigit(int c) { return ((c >= '0') && (c <= '9')); }

PSZ RwgtEncodeString(PSZ pszSource) // in: pszSource must not be NULL
{
    PSZ pszDest = (PSZ) malloc(sizeof(CHAR)*strlen(pszSource)*3+1);
            // in the worst case, the encoded string is 3 time longer
    PSZ pszSourceCurr = pszSource,
        pszDestCurr = pszDest;
    CHAR ch;

    if (pszDest)
    {
        while ((ch = *pszSourceCurr++))
        {
            if (   (iscntrl(ch))
                || (ch == ';')
                || (ch == '=')
                || (ch == '%'))
            {
                sprintf(pszDestCurr, "%%%02X", ch);
                pszDestCurr += 3;
            }
            else
                *pszDestCurr++ = ch;
        }

        *pszDestCurr = 0;
    }

    return (pszDest);
}

/*
 *@@ RwgtDecodeString:
 *      decodes a string encoded by RwgtEncodeString.
 *
 *      Use RwgtFree to release the returned value.
 *
 *@@added V0.1.0 (2001-01-27) [lafaix]
 */

PSZ RwgtDecodeString(PSZ pszEncodedSource) // in: pszEncodedSource must not be NULL
{
    PSZ pszDest = (PSZ) malloc(strlen(pszEncodedSource)+1);
            // decoded string cannot be longer that source
    PSZ pszSourceCurr = pszEncodedSource,
        pszDestCurr = pszDest;
    CHAR ch, ch2, ch3;

    if (pszDest)
    {
        while ((ch = *pszSourceCurr++))
        {
            if (ch == '%')
            {
                // check if it looks like an encoded symbol
                if (*pszSourceCurr)
                {
                    if ((isxdigit((ch2 = *pszSourceCurr++))))
                    {
                        if (*pszSourceCurr)
                        {
                            if (isxdigit((ch3 = *pszSourceCurr++)))
                            {
                                // it does looks like a valid encoded symbol
                                *pszDestCurr++ = ((isdigit(ch2) ? ch2 - '0' : 10 + (ch2 - 'A')) << 4) |
                                                 (isdigit(ch3) ? ch3 - '0' : 10 + (ch3 - 'A'));
                            }
                            else
                            {
                                // the second symbol following % is not an hex digit
                                *pszDestCurr++ = ch;
                                *pszDestCurr++ = ch2;
                                *pszDestCurr++ = ch3;
                            }
                        }
                        else
                        {
                            // the second symbol following % is nul
                            *pszDestCurr++ = ch;
                            *pszDestCurr++ = ch2;
                        }
                    }
                    else
                    {
                        // the first symbol following % is not an hex digit
                        *pszDestCurr++ = ch;
                        *pszDestCurr++ = ch2;
                    }
                }
                else
                    // the first symbol following % is nul
                    *pszDestCurr++ = ch;
            }
            else
                *pszDestCurr++ = ch;
        }
        *pszDestCurr = 0;
    }

    return (pszDest);
}

/*
 *@@ RwgtFree:
 *      free a string returned by either RwgtEncodeString or
 *      RwgtDecodeString.
 *
 *@@added V0.1.0 (2001-01-27) [lafaix]
 */

VOID RwgtFree(PSZ psz)
{
    free(psz);
}

/*@@ RwgtQueryHelpLibrary:
 *      returns PSZ of full help library path (in the same directory as
 *      the widget, with an extension of HLP).
 *
 *@@added V0.1.0 (2001-02-04) [lafaix]
 */

char G_szLibraryName[CCHMAXPATH] = {0};

const char *RwgtQueryHelpLibrary(VOID)
{
    if (G_szLibraryName[0] == 0)
    {
        ULONG ulLength;

        // help library name not already known
        DosQueryModuleName(G_hmodThis,
                           CCHMAXPATH,
                           G_szLibraryName);
        ulLength = strlen(G_szLibraryName);
        G_szLibraryName[ulLength-1] = 'P';
        G_szLibraryName[ulLength-3] = 'H';
    }

    return (G_szLibraryName);
}

/*
 *@@ RwgtPresParamChanged:
 *      implementation for WM_PRESPARAMCHANGED.
 *
 *      While this isn't exactly required, it's a nice
 *      thing for a widget to react to colors and fonts
 *      dropped on it. While we're at it, we also save
 *      these colors and fonts in our setup string data.
 */

VOID RwgtPresParamChanged(HWND hwnd,
                          ULONG ulAttrChanged,
                          PXCENTERWIDGET pWidget)
{
    PRPRIVATE pPrivate = (PRPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        BOOL fInvalidate = TRUE;
        switch (ulAttrChanged)
        {
            case 0:     // layout palette thing dropped
            {
                PSZ pszFont = NULL;

                free(pPrivate->Setup.pszFont);
                pPrivate->Setup.pszFont = NULL;

                pszFont = pwinhQueryWindowFont(hwnd);
                if (pszFont)
                {
                    // we must use local malloc() for the font;
                    // the winh* code uses a different C runtime
                    pPrivate->Setup.pszFont = strdup(pszFont);
                    pwinhFree(pszFont);
                }

                // update our setup data; the presparam has already
                // been changed, so we can just query it
                pPrivate->Setup.lcolBackground
                    = pwinhQueryPresColor(hwnd,
                                          PP_BACKGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_DIALOGBACKGROUND);
                pPrivate->Setup.lcolForeground
                    = pwinhQueryPresColor(hwnd,
                                          PP_FOREGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_WINDOWSTATICTEXT);
            break; }
            case PP_BACKGROUNDCOLOR:    // background color (no ctrl pressed)
                // update our setup data; the presparam has already
                // been changed, so we can just query it
                pPrivate->Setup.lcolBackground
                    = pwinhQueryPresColor(hwnd,
                                          PP_BACKGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_DIALOGBACKGROUND);
            break;
            case PP_FOREGROUNDCOLOR:    // foreground color (ctrl pressed)
                // update our setup data; the presparam has already
                // been changed, so we can just query it
                pPrivate->Setup.lcolForeground
                    = pwinhQueryPresColor(hwnd,
                                          PP_FOREGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_WINDOWSTATICTEXT);
            break;

            case PP_FONTNAMESIZE:       // font dropped:
            {
                PSZ pszFont = 0;

                free(pPrivate->Setup.pszFont);
                pPrivate->Setup.pszFont = NULL;

                pszFont = pwinhQueryWindowFont(hwnd);
                if (pszFont)
                {
                    // we must use local malloc() for the font;
                    // the winh* code uses a different C runtime
                    pPrivate->Setup.pszFont = strdup(pszFont);
                    pwinhFree(pszFont);
                }
            break; }

            default:
                fInvalidate = FALSE;
        }

        if (fInvalidate)
        {
            // something has changed:
            XSTRING strSetup;

            // repaint
            WinInvalidateRect(hwnd, NULL, FALSE);

            // recompose our setup string
            RwgtSaveSetup(&strSetup,
                          &pPrivate->Setup);
            if (strSetup.ulLength)
                // we have a setup string:
                // tell the XCenter to save it with the XCenter data
                WinSendMsg(pPrivate->pWidget->pGlobals->hwndClient,
                           XCM_SAVESETUP,
                           (MPARAM)hwnd,
                           (MPARAM)strSetup.psz);
            pxstrClear(&strSetup);
        }
    } // end if (pPrivate)
}

#ifndef INCL_NOICON
/*
 *@@ DrawIconTargetEmphasis:
 *      paint a black rectangle around the icon if fStatus is TRUE
 *      or a dialog-background--colored one otherwise.
 */

VOID DrawIconTargetEmphasis(HWND hwnd,
                            HPS hps,
                            BOOL fStatus)
{
    RECTL rclWin;
    LONG lColor;

    WinQueryWindowRect(hwnd, &rclWin);
    pgpihSwitchToRGB(hps);

    rclWin.xLeft += 22;
    rclWin.xRight -= 23;
    rclWin.yBottom += 20;
    rclWin.yTop -= 25;

    if (fStatus)
       lColor = CLR_BLACK;
    else
       lColor = pwinhQueryPresColor(hwnd,
                                    PP_BACKGROUNDCOLOR,
                                    FALSE,
                                    SYSCLR_DIALOGBACKGROUND);

    pgpihDraw3DFrame(hps, &rclWin, 1, lColor, lColor);
}

/*
 *@@ fnwpIconFile:
 *      window proc for the IconFile static control that
 *      handles drop messages.
 */

MRESULT EXPENTRY fnwpIconFile(HWND hwnd,
                              ULONG msg,
                              MPARAM mp1,
                              MPARAM mp2)
{
    MRESULT mrc = 0;

    switch (msg)
    {

        /*
         * WM_HITTEST:
         *      By default, static controls are transparent.  We must
         *      change that if we want to receive DM_ messages.
         */

        case WM_HITTEST:
            mrc = (MRESULT)HT_NORMAL;
            break;

        /*
         * DM_DRAGOVER and DM_DROP:
         *
         */

        case DM_DRAGOVER:
        case DM_DROP:
        {
            PDRAGINFO dragInfo = (PDRAGINFO)mp1;
            PDRAGITEM dragItem;
            USHORT usOp, usIndicator;

            DrgAccessDraginfo(dragInfo);
            switch (dragInfo->usOperation)
            {
                case DO_UNKNOWN:
                  DrgFreeDraginfo(dragInfo);
                  return (MRFROM2SHORT(DOR_NODROPOP, 0));
                case DO_DEFAULT:
                  usOp = DO_COPY;
                  break;
                case DO_COPY:
                case DO_MOVE:
                  usOp = dragInfo->usOperation;
                  break;
            }

            usIndicator = DOR_NEVERDROP;

            if (DrgQueryDragitemCount(dragInfo) == 1)
            {
                dragItem = DrgQueryDragitemPtr(dragInfo, 0);

                if (((dragItem->fsSupportedOps & DO_COPYABLE) &&
                     (usOp == (USHORT)DO_COPY))               ||
                    ((dragItem->fsSupportedOps & DO_MOVEABLE) &&
                     (usOp == (USHORT)DO_MOVE)))
                {
                    if (DrgVerifyRMF(dragItem, "DRM_OS2FILE", NULL))
                        usIndicator = DOR_DROP;
                    else
                        usIndicator = DOR_NODROP;

                    if (usIndicator == DOR_DROP)
                    {
                        HPS hps = DrgGetPS(hwnd);

                        DrawIconTargetEmphasis(hwnd, hps, (msg == DM_DRAGOVER));

                        DrgReleasePS(hps);

                        if (msg == DM_DROP)
                        {
                            // msg was WM_DROP; lets set the new icon
                            CHAR szTemp[CCHMAXPATH];
                            HWND hwndParent = WinQueryWindow(hwnd, QW_PARENT);
                            PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(hwndParent, QWL_USER);

                            if (pData)
                            {
                                PRSETUP pSetup = ((PSTORAGE)pData->pUser)->pSetup;
                                if (pSetup)
                                {
                                    DrgQueryStrName(dragItem->hstrContainerName, CCHMAXPATH, szTemp);
                                    DrgQueryStrName(dragItem->hstrSourceName, CCHMAXPATH, szTemp+strlen(szTemp));

                                    free(pSetup->pszIconFile);
                                    pSetup->pszIconFile = strdup(szTemp);
                                    if (pSetup->hIcon)
                                        WinFreeFileIcon(pSetup->hIcon);
                                    pSetup->hIcon = WinLoadFileIcon(pSetup->pszIconFile, FALSE);

                                    // Update the icon displayed in dialog
                                    WinSendDlgItemMsg(hwndParent,
                                                      ID_CRDI_SCRIPT_ICON,
                                                      SM_SETHANDLE,
                                                      (MPARAM)(pSetup->hIcon),
                                                      (MPARAM)0);
                                }
                            }
                        } // end if (msg == DM_DROP)
                    }
                }
            } // end if (DrgQueryDragitemCount(dragInfo) == 1)

            DrgFreeDraginfo(dragInfo);
            mrc = MRFROM2SHORT(usIndicator, usOp);
            break;
        }

        /*
         * DM_DRAGLEAVE:
         *
         */

        case DM_DRAGLEAVE:
        {
            HPS hps = DrgGetPS(hwnd);

            DrawIconTargetEmphasis(hwnd, hps, FALSE);

            DrgReleasePS(hps);
            break;
        }

        default:
            mrc = G_pfnwpOldIconFile(hwnd, msg, mp1, mp2);
    }

    return (mrc);
}
#endif
