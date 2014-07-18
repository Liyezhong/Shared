/******************************************************************/
/*! \file DeviceLifeCycleRecord.h.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.07.2014
 *   $Author:  $ A. Yang
 *
 *  \b Description:
 *
 *       This data structure contains the life cycle or life time for device
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *
 */
/******************************************************************/
#ifndef DEVICELIFECYCLERECORD_H
#define DEVICELIFECYCLERECORD_H

#include <QMap>
#include <QString>

namespace DeviceControl
{
    class PartLifeCycleRecord
    {
    public:
        QMap<QString, QString> m_ParamMap;
    };

    class ModuleLifeCycleRecord
    {
    public:
       QMap<QString, PartLifeCycleRecord*> m_PartLifeCycleMap;
    };


    class DeviceLifeCycleRecord
    {
    public:
        DeviceLifeCycleRecord();
        ~DeviceLifeCycleRecord();
        void ReadRecord();
        void WriteRecord();
        QMap<QString, ModuleLifeCycleRecord*> m_ModuleLifeCycleMap;
    private:
        QString m_DeviceLifeCycleRecordFileName;
    };
}
#endif // DEVICELIFECYCLERECORD_H
