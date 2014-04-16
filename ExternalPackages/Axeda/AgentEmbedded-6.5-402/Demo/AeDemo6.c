/**************************************************************************
 *
 *  Copyright (c) 1999-2012 Axeda Corporation.  All rights reserved.
 *
 **************************************************************************
 *
 *  Filename   :  AeDemo6.c
 *
 *  Subsystem  :  Axeda Agent Embedded
 *
 *  Description:  Demo application: remote session example
 *
 **************************************************************************/


#ifdef WIN32
#if(_MSC_VER >= 1300)
	#define _CRT_SECURE_NO_DEPRECATE  // re:  Secure Template Overloads see MSDN
    #define _USE_32BIT_TIME_T       // otherwise time_t is 64 bits in .2005
#endif
#endif


#include <stdio.h>
#include <math.h>
#ifdef WIN32
#include <conio.h>
#else
#include <unistd.h>
#endif

#include "AeOSLocal.h"
#include "AeTypes.h"
#include "AeError.h"
#include "AeOS.h"
#include "AeInterface.h"
#include "AeDemoCommon.h"

#if (WIN32) && (_MSC_VER < 1300) && (_DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

/* Predefine the model/serial number. */
#define MODEL_NUMBER    "DemoModel1"
#define SERIAL_NUMBER   "DemoDevice1"

/* Define rate in seconds. */
#define PING_RATE       5

/******************************************************************************
 * Synopsis:
 *
 *     AeDemo6 [-o owner] [-s] [-g] [{-ph|-ps} proxy-host [-pp proxy-port]
 *             [-pu proxy-user] [-pw proxy-password]]
 *             [-rsname remote-session-name [-rstype remote-session-type]
 *              -rshost remote-session-host -rsport remote-session-port] url
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
 *     -rsname remote-session-name
 *         Specify the remote session name.
 *     -rstype remote-session-type
 *         Specify the remote session type.
 *     -rshost remote-session-host
 *         Specify the destination host for the remote session.
 *     -rsport remote-session-port
 *         Specify the destination port for the remote session.
 *
 * Description:
 *
 *     The program defines a device, configures the primary Enterprise server
 *     (using url argument) and defines a remote session, if specified. After
 *     that it calls AeDRMExecute() to process any pending tasks. Remote
 *     sessions, if started by Enterprise user, will be handled inside
 *     AeDRMExecute().
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
    rc = AeDRMAddDevice(AeDRMDeviceMaster,
                        config.pModelNumber ? config.pModelNumber : MODEL_NUMBER,
                        config.pSerialNumber ? config.pSerialNumber : SERIAL_NUMBER,
                        &iDeviceId);
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

	/* Add remote session if configured */
    if (config.pRemoteSessionName != NULL && config.pRemoteSessionType != NULL &&
        config.pRemoteSessionHost != NULL && config.iRemoteSessionPort != 0)
    {
	    rc = AeDRMAddRemoteSession(iDeviceId, config.pRemoteSessionName, "",
            config.pRemoteSessionType, config.pRemoteSessionHost, config.iRemoteSessionPort);
        if (rc != AeEOK)
        {
            fprintf(stderr, "Failed to add Remote session (%s)\n", AeGetErrorString(rc));
            return 1;
        }
    }

    /* execute demo until keystroke */
#ifdef WIN32
    while (!_kbhit())
#else
    while (1)
#endif
    {
        /* set time limit for the DRM execution. this is also the data
         * poll rate. */
        timeLimit.iSec = 30;
        timeLimit.iMicroSec = 0;

		/* this is the execution cycle */
		AeDRMExecute(&timeLimit);
    }

    /* shutdown Axeda Agent Embedded */
    AeShutdown();

#if defined(WIN32) && defined(_DEBUG)&& (_MSC_VER < 1300)
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
