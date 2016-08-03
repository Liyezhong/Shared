/******************************************************************/
/*! \file DeviceLifeCycleRecord.h
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
    /**
      * \brief PartLifeCycleRecord
    */
    class PartLifeCycleRecord
    {
    public:
        QMap<QString, QString> m_ParamMap; //!< param map
    };

    /**
      * \brief ModuleLifeCycleRecord
    */
    class ModuleLifeCycleRecord
    {
    public:
       QMap<QString, PartLifeCycleRecord*> m_PartLifeCycleMap; //!< life cycle map
    };


    /**
      * \brief DeviceLifeCycleRecord
    */
    class DeviceLifeCycleRecord
    {
    public:
        /**
         * \brief constructor
        */
        DeviceLifeCycleRecord();
        /**
         * \brief destructor
        */
        ~DeviceLifeCycleRecord();

        /**
         * \brief ReadRecord
        */
        bool ReadRecord();
        /**
         * \brief WriteRecord
        */
        bool WriteRecord();

        QMap<QString, ModuleLifeCycleRecord*> m_ModuleLifeCycleMap; //!< life cycle map

    private:
        /**
         * \brief free object
        */
        void FreeObjects();
        QString m_DeviceLifeCycleRecordFileName; //!< life cycle file name
    };
}
#endif // DEVICELIFECYCLERECORD_H
