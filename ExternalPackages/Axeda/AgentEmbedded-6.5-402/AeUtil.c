/* $Id: AeUtil.c,v 1.11 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

static AeUInt8 *pAlphabet =
    (AeUInt8 *) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static AeUInt8 pCodes[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 0 - 15    */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 16 - 31   */
/*                                               +               /                  */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,    /* 32 - 47   */
/*   0   1   2   3   4   5   6   7   8   9                                          */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,    /* 48 - 63   */
/*       A   B   C   D   E   F   G   H   I   J   K   L   M   N   O                  */
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,    /* 64 - 79   */
/*   P   Q   R   S   T   U   V   W   X   Y   Z                                      */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,    /* 80 - 95   */
/*       a   b   c   d   e   f   g   h   i   j   k   l   m   n   o                  */
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,    /* 96 - 111  */
/*   p   q   r   s   t   u   v   w   x   y   z                                      */
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,    /* 112 - 127 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 128 - 143 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 144 - 159 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 160 - 175 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 176 - 191 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 192 - 207 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 208 - 223 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    /* 224 - 239 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1     /* 240 - 255 */
};

/******************************************************************************/
void AeSetString(AeChar **ppString, AeChar *pValue, AeInt iLength)
{
    if (*ppString)
        AeFree(*ppString);

    if (pValue)
    {
        if (iLength < 0)
            iLength = strlen(pValue);

        *ppString = (AeChar *) AeAlloc(iLength + 1);
        if (!(*ppString))
            return;

        strncpy(*ppString, pValue, iLength);
        (*ppString)[iLength] = '\0';
    }
    else
        *ppString = NULL;
}

/******************************************************************************/
void AeTrimRight(AeChar *pString, AeChar cCh)
{
    AeChar *pCh;

    pCh = pString + strlen(pString) - 1;
    while (pCh >= pString && *pCh == cCh)
    {
        *pCh = '\0';
        pCh -= 1;
    }
}

/******************************************************************************/
void AeFileStatCopy(AeFileStat *pDst, AeFileStat *pSrc)
{
    memset(pDst, 0, sizeof(AeFileStat));
    AeSetString(&pDst->pName, pSrc->pName, -1);
    pDst->iType = pSrc->iType;
    pDst->iSize = pSrc->iSize;
    pDst->iMTime = pSrc->iMTime;
}

/******************************************************************************/
void AeFileStatDestroy(AeFileStat *pFile)
{
    AeSetString(&pFile->pName, NULL, -1);
    AeFree(pFile);
}

/******************************************************************************/
AeInt32 AeBase64Encode(AeChar *pInput, AeInt32 iLength, AeChar *pOutput)
{
    AeInt32 i, iIndex, iVal;
    AeBool bQuad, bTrip;

    for (i = 0, iIndex=0; i < iLength; i += 3, iIndex += 4)
    {
        bQuad = AeFalse;
        bTrip = AeFalse;
        iVal = (0xFF & (AeInt32) pInput[i]);
        iVal <<= 8;
        if ((i + 1) < iLength)
        {
            iVal |= (0xFF & (AeInt32) pInput[i+1]);
            bTrip = AeTrue;
        }
        iVal <<= 8;
        if ((i + 2) < iLength)
        {
            iVal |= (0xFF & (AeInt32) pInput[i+2]);
            bQuad = AeTrue;
        }
        pOutput[iIndex+3] = pAlphabet[(bQuad? (iVal & 0x3F): 64)];
        iVal >>= 6;
        pOutput[iIndex+2] = pAlphabet[(bTrip? (iVal & 0x3F): 64)];
        iVal >>= 6;
        pOutput[iIndex+1] = pAlphabet[iVal & 0x3F];
        iVal >>= 6;
        pOutput[iIndex+0] = pAlphabet[iVal & 0x3F];
    }

    pOutput[iIndex] = '\0';

    return iIndex;
}

/******************************************************************************/
AeInt32 AeBase64Decode(AeChar *pInput, AeInt32 iLength, AeChar *pOutput)
{
    AeInt32 iUseLen, iShift, iAccum, iIndex, iValue, ix; 

    iUseLen = iLength;
    if (pInput[iLength - 1] == '=') 
        --iUseLen;
    if (pInput[iLength - 2] == '=') 
        --iUseLen;

    iShift = 0; /* # of excess bits stored in iAccum */
    iAccum = 0; /* excess bits */
    iIndex = 0; 

    for (ix=0; ix < iUseLen; ix++) 
    { 
        iValue = pCodes[(int) pInput[ix]];
        if (iValue >= 0) 
        { /* skip over non-code */
            iAccum <<= 6; /* bits iShift up by 6 each time thru */
            iShift += 6; /* loop, with new bits being put in */
            iAccum |= iValue; /* at the bottom. */
            if ( iShift >= 8 ) 
            { /* whenever there are 8 or more shifted in, */
                iShift -= 8; /* write them pOutput (from the top, leaving any */
                pOutput[iIndex++] = /* excess at the bottom for next iteration. */
                    (AeUInt8) ((iAccum >> iShift) & 0xff); 
            } 
        } 
    } 

    return iIndex;
}

/******************************************************************************/
void AeBinToHex(AeUInt8 *pBin, AeInt32 iLength, AeChar *pHex)
{
    AeInt32 i;
    AeUInt8 j;

    for (i = 0; i < iLength; i++)
    {
        j = (pBin[i] >> 4) & 0xf;
        if (j <= 9)
            pHex[i * 2] = (j + '0');
        else
            pHex[i * 2] = (j + 'a' - 10);
        j = pBin[i] & 0xf;
        if (j <= 9)
            pHex[i * 2 + 1] = (j + '0');
        else
            pHex[i * 2 + 1] = (j + 'a' - 10);
    }

    pHex[iLength * 2] = '\0';
}

/******************************************************************************/
void AeGetISO8601Time(AeTimeValue *pTime, AeChar *pOutput, AeInt iMaxLen)
{
    struct tm *pTm;

    pTm = gmtime((time_t *) &pTime->iSec);
    if (!pTm)
        return;

    snprintf(pOutput, iMaxLen, "%d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
        pTm->tm_year + 1900,
        pTm->tm_mon + 1,
        pTm->tm_mday,
        pTm->tm_hour,
        pTm->tm_min,
        pTm->tm_sec,
        pTime->iMicroSec / 1000);
}

/******************************************************************************/
AeURL *AeURLNew(void)
{
    AeURL *pURL;

    pURL = (AeURL *) AeAlloc(sizeof(AeURL));
    if (!pURL)
        return NULL;
    memset(pURL, 0, sizeof(AeURL));

    pURL->pBuffer = AeBufferNew(NULL, 0, 256);
    if (!pURL->pBuffer)
        return NULL;

    pURL->bSecure = AeFalse;
    pURL->pHost = NULL;
    pURL->iPort = 0;
    pURL->pAbsPath = NULL;
    pURL->pUser = NULL;
    pURL->pPassword = NULL;

    return pURL;
}

/******************************************************************************/
void AeURLDestroy(AeURL *pURL)
{
    if (pURL->pHost)
        AeFree(pURL->pHost);
    if (pURL->pAbsPath)
        AeFree(pURL->pAbsPath);
    if (pURL->pUser)
        AeFree(pURL->pUser);
    if (pURL->pPassword)
        AeFree(pURL->pPassword);

    if (pURL->pBuffer)
        AeBufferDestroy(pURL->pBuffer);

    AeFree(pURL);
}

/******************************************************************************/
void AeURLSetHost(AeURL *pURL, AeChar *pHost)
{
    AeSetString(&pURL->pHost, pHost, -1);
}

/******************************************************************************/
void AeURLSetAbsPath(AeURL *pURL, AeChar *pAbsPath)
{
    AeSetString(&pURL->pAbsPath, pAbsPath, -1);
}

/******************************************************************************/
void AeURLSetUser(AeURL *pURL, AeChar *pUser)
{
    AeSetString(&pURL->pUser, pUser, -1);
}

/******************************************************************************/
void AeURLSetPassword(AeURL *pURL, AeChar *pPassword)
{
    AeSetString(&pURL->pPassword, pPassword, -1);
}

/******************************************************************************/
AeBool AeURLSet(AeURL *pURL, AeChar *pString)
{
    AeChar *pDelim1, *pDelim2, *pStart, *pPath;

    pURL->bSecure = AeFalse;
    AeSetString(&pURL->pHost, NULL, -1);
    pURL->iPort = 0;
    AeSetString(&pURL->pAbsPath, NULL, -1);
    AeSetString(&pURL->pUser, NULL, -1);
    AeSetString(&pURL->pPassword, NULL, -1);

    pStart = pString;

    /* parse protocol */
    if (!strncasecmp(pStart, "http://", 7))
        pStart += 7;
    else if (!strncasecmp(pStart, "https://", 8))
    {
        pURL->bSecure = AeTrue;
        pStart += 8;
    }

    /* try to find absolute path start */
    pPath = strchr(pStart, '/');
    if (!pPath)
        pPath = pStart + strlen(pStart);

    /* network address must be present */
    if (pPath == pStart)
        return AeFalse;

    /* parse authentication information */
    if ((pDelim1 = strchr(pStart, '@')) &&
        pDelim1 < pPath)
    {
        if ((pDelim2 = strchr(pStart, ':')) &&
            pDelim2 < pDelim1)
        {
            AeSetString(&pURL->pUser, pStart, pDelim2 - pStart);
            AeSetString(&pURL->pPassword, pDelim2 + 1, pDelim1 - pDelim2 - 1);
        }
        else
            AeSetString(&pURL->pUser, pStart, pDelim1 - pStart);

        pStart = pDelim1 + 1;
    }

    /* parse network address */
    if ((pDelim1 = strchr(pStart, ':')) &&
        pDelim1 < pPath)
    {
        pURL->iPort = (AeUInt16) strtol(pDelim1 + 1, &pDelim2, 10);
        if (pDelim2 == pDelim1 + 1)
            return AeFalse;
        AeSetString(&pURL->pHost, pStart, pDelim1 - pStart);
    }
    else
    {
        if (!pURL->bSecure)
            pURL->iPort = HTTP_DEFAULT_PORT;
        else
            pURL->iPort = HTTPS_DEFAULT_PORT;
        
        AeSetString(&pURL->pHost, pStart, pPath - pStart);
    }
        
    /* store absolute path (if any) */
    AeURLSetAbsPath(pURL, pPath);

    return AeTrue;
}

/******************************************************************************/
AeChar *AeURLGet(AeURL *pURL)
{
    AeChar *pTmp;
    AeInt32 iSize;

    AeBufferReset(pURL->pBuffer);

    if (!pURL->bSecure)
        pTmp = "http://";
    else
        pTmp = "https://";
    AeBufferWrite(pURL->pBuffer, pTmp, strlen(pTmp));

    if (pURL->pUser)
    {
        AeBufferWrite(pURL->pBuffer, pURL->pUser, strlen(pURL->pUser));
        if (pURL->pPassword)
        {
            AeBufferWrite(pURL->pBuffer, ":", 1);
            AeBufferWrite(pURL->pBuffer, pURL->pPassword, strlen(pURL->pPassword));
        }
        AeBufferWrite(pURL->pBuffer, "@", 1);
    }

    if (pURL->pHost)
    {
        AeBufferWrite(pURL->pBuffer, pURL->pHost, strlen(pURL->pHost));
        if (!((!pURL->bSecure && pURL->iPort == HTTP_DEFAULT_PORT) ||
              ( pURL->bSecure && pURL->iPort == HTTPS_DEFAULT_PORT)))
        {
            AeChar pTmp2[6];

            AeBufferWrite(pURL->pBuffer, ":", 1);
            snprintf(pTmp2, sizeof(pTmp2), "%d", pURL->iPort);
            AeBufferWrite(pURL->pBuffer, pTmp2, strlen(pTmp2));
        }
    }

    if (pURL->pAbsPath)
        AeBufferWrite(pURL->pBuffer, pURL->pAbsPath, strlen(pURL->pAbsPath));

    AeBufferGetReadArea(pURL->pBuffer, &pTmp, &iSize);

    return pTmp;
}

/******************************************************************************/
AeBool AeConvertPath(AeChar *pPath)
{
	/* convert path delimiter to platform specific format */
	AeInt32 i;
	AeInt32 iLast;
	AeInt32 iLen = strlen(pPath);

	iLast = -2;
	for(i = 0; i < iLen; i ++)
	{
		if(pPath[i] == '/' || pPath[i] == '\\')
		{
			/* 2 consecutive path chars, bad format */
			if(iLast + 1 == i)
				return AeFalse;

			iLast = i;
			pPath[i] = PATH_CHAR;
		}
	}
	return AeTrue;
}

/******************************************************************************/
AeBool AeMakeDirHierarchy(AeChar *pPath)
{
    AeBool bOK;
    AeChar *ptr;

    bOK = AeTrue;
    ptr = pPath;
    while (bOK && (ptr = strchr(ptr, PATH_CHAR)))
    {
        if (ptr != pPath)
        {
            *ptr = 0;
            bOK = AeMakeDir(pPath);
            *ptr = PATH_CHAR;
        }

        ptr += 1;
    }

    if (bOK)
        bOK = AeMakeDir(pPath);

    return bOK;
}
