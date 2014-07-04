/* $Id: AeDemo4.c,v 1.10 2008/05/21 18:24:36 dkhodos Exp $ */

/**************************************************************************
 *
 *  Copyright (c) 1999-2007 Axeda Corporation.  All rights reserved.
 *
 **************************************************************************
 *
 *  Filename   :  AeDemo4.c
 *
 *  Subsystem  :  Axeda Agent Embedded
 *
 *  Description:  Demo application: custom file download example
 *
 **************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#else
#include <unistd.h>
#endif
#include <errno.h>

#include "AeOSLocal.h"
#include "AeTypes.h"
#include "AeError.h"
#include "AeOS.h"
#include "AeInterface.h"
#include "AeDemoCommon.h"

/* Predefine the model/serial number. */
#define MODEL_NUMBER    "DemoModel1"
#define SERIAL_NUMBER   "DemoDevice1"

/* Define rate in seconds. */
#define PING_RATE       5

typedef struct _AeDemoDownload AeDemoDownload;

struct _AeDemoDownload
{
    AeFileHandle iCurFileHandle;
};

/* function prototypes  */
static AeBool OnFileDownloadBegin(AeInt32 iDeviceId, AePointer *ppUserData);
static AeBool OnFileDownloadData(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData);
static void OnFileDownloadEnd(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);

/******************************************************************************
 * Synopsis:
 *
 *     AeDemo4 [-o owner] [-s] [-g] [{-ph|-ps} proxy-host [-pp proxy-port]
 *             [-pu proxy-user] [-pw proxy-password]] [-o owner] url
 *
 * url:
 *        Use the following syntax:
 *            http://<server-name>:<port>/eMessage (non-secure communications) or
 *            https://<server-name>:<port>/eMessage  (secure communications)
 *          where <server-name> is replaced with the Enterprise server name.
 *            and <port> is replaced with the port number.  (Use 443 for SSL).
 *        For example, http://drm.axeda.com/eMessage.
 *
 * Options:
 *
 *     -o owner
 *         Use specified owner (database name).
 *     -s
 *         Use secure communication (SSL).
 *     -g
 *         Enable debug messages.
 *     -ph proxy-host
 *         Use HTTP proxy at proxy-host. Default port is 80.
 *     -ps proxy-host
 *         Use SOCKS proxy at proxy-host. Default port is 1080.
 *     -pp proxy-port
 *         Override default port for proxy.
 *     -pu proxy-user
 *         Use proxy-user as user name for proxy authentication.
 *     -pw proxy-password
 *         Use proxy-password as password for proxy authentication.
 *
 * Description:
 *
 *     The program defines a device, configures the primary Enterprise server
 *     (using url argument) and installs command callbacks. After that it loops
 *     and waits for callback invocation.
 ******************************************************************************/
int main(int argc, char *argv[])
{
    AeDemoConfig config;
    AeInt32 iDeviceId, iServerId;
    AeTimeValue pingRate, timeLimit;
    AeError rc;

    /* process options */
    if (!AeDemoProcessOptions(&config, &argc, argv, 1))
    {
        AeDemoUsage(argv[0], "url");
        return 1;
    }

    /* initialize Axeda Agent Embedded */
    AeInitialize();

    /* apply options */
    AeDemoApplyConfig(&config);

    /* configure master device */
    rc = AeDRMAddDevice(AeDRMDeviceMaster, MODEL_NUMBER, SERIAL_NUMBER, &iDeviceId);
    if (rc != AeEOK)
    {
        fprintf(stderr, "Failed to add device (%s)\n", AeGetErrorString(rc));
        return 1;
    }

    /* configure primary DRM server */
    pingRate.iSec = PING_RATE;
    pingRate.iMicroSec = 0;
    rc = AeDRMAddServer(AeDRMServerConfigPrimary, argv[argc], config.pOwner,
        &pingRate, &iServerId);
    if (rc != AeEOK)
    {
        fprintf(stderr, "Failed to add server (%s)\n", AeGetErrorString(rc));
        return 1;
    }

    /* install command callbacks */
    AeDRMSetOnFileDownloadBegin(OnFileDownloadBegin);
    AeDRMSetOnFileDownloadData(OnFileDownloadData);
    AeDRMSetOnFileDownloadEnd(OnFileDownloadEnd);


    /* execute demo until keystroke */
#ifdef WIN32
    while (!_kbhit())
#else
    while (1)
#endif
    {
        /* set time limit for the DRM execution. this is also the data
         * poll rate. */
        timeLimit.iSec = 1;
        timeLimit.iMicroSec = 0;

		/* this is the execution cycle */
		AeDRMExecute(&timeLimit);
    }

    /* shutdown Axeda Agent Embedded */
    AeShutdown();

    return 0;
}

/******************************************************************************
 * Callbacks
 ******************************************************************************/

/******************************************************************************/
static AeBool OnFileDownloadBegin(AeInt32 iDeviceId, AePointer *ppUserData)
{
    AeDemoDownload *pDownload;

    /* allocate image descriptor */
    pDownload = (AeDemoDownload *) malloc(sizeof(AeDemoDownload));
    if (!pDownload)
        return AeFalse;

    /* initialize */
    memset(pDownload, 0, sizeof(AeDemoDownload));
    pDownload->iCurFileHandle = AeFileInvalidHandle;

    *ppUserData = pDownload;

    return AeTrue;
}

/******************************************************************************/
static AeBool OnFileDownloadData(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData)
{
    AeDemoDownload *pDownload;

    pDownload = (AeDemoDownload *) pUserData;
    if (!pDownload)
        return AeFalse;

    /* zero size indicates end-of-file */
    if (iSize == 0)
    {
        AeFileClose(pDownload->iCurFileHandle);
        pDownload->iCurFileHandle = AeFileInvalidHandle;
        return AeTrue;
    }

    /* initialize next file */
    if (pDownload->iCurFileHandle == AeFileInvalidHandle)
    {
        /* open file */
        pDownload->iCurFileHandle =
            AeFileOpen(pFile->pName, AE_OPEN_WRITE_ONLY | AE_OPEN_CREATE | AE_OPEN_TRUNCATE);
        if (pDownload->iCurFileHandle == AeFileInvalidHandle)
        {
            AeLog(AeLogError, "Could not create file for download: %s", pFile->pName);
            return AeFalse;
        }
    }

    /* write to file */
    if (AeFileWrite(pDownload->iCurFileHandle, pData, iSize) != iSize)
        return AeFalse;

    return AeTrue;
}

/******************************************************************************/
static void OnFileDownloadEnd(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData)
{
    AeDemoDownload *pDownload;

    pDownload = (AeDemoDownload *) pUserData;
    if (!pDownload)
        return;

    if (pDownload->iCurFileHandle != AeFileInvalidHandle)
        AeFileClose(pDownload->iCurFileHandle);

    AeFree(pDownload);
}
