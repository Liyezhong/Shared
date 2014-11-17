#include <QtGlobal>

#ifndef PLATFORM_THREAD_IDS_H
#define PLATFORM_THREAD_IDS_H

namespace Threads {
    const quint32 THREAD_ID_MASTERTHREAD   = 0x0001;  ///< Colorado master thread
    const quint32 THREAD_ID_DATALOGGING    = 0x0002;  ///< Data logging
    const quint32 THREAD_ID_EVENTHANDLER   = 0x0003;  ///< Event handler
    const quint32 THREAD_ID_GPIO_MANAGER   = 0x0004;  ///< GPIO Manager
    const quint32 THREAD_ID_REMOTECARE     = 0x0005;  ///< Remote Care
    const quint32 THREAD_ID_HEARTBEAT      = 0x0006;  ///< Heart beat
    const quint32 THREAD_ID_GUI            = 0x0007;  ///< Gui
    const quint32 THREAD_ID_IMPORTEXPORT   = 0x0008;  ///< ImportExport Thread
    const quint32 THREAD_ID_EXPORT         = 0x0009;  ///< Export
    const quint32 THREAD_ID_LEICAAPPS      = 0x000A;  ///< Leica Apps
}

#endif // PLATFORM_THREAD_IDS_H

