/****************************************************************************/
/*! \file CANMessageConfiguration.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CANMessageConfiguration
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef CANMESSAGECONFIGURATION_H
#define CANMESSAGECONFIGURATION_H

#include <QHash>
#include <QDomNode>
#include <QDomElement>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

class QDomNode;

namespace DeviceControl
{

//! Type definition of the internal CAN message storage type
typedef QHash<QString, unsigned int> CANMessageList;

#define GET_CAN_MSG_RR_FLAG 0x20000000   //!< ID used for (xml-)undefined CAN-IDs

/****************************************************************************/
/*!
 *  \brief  This class reads the CAN message definition file
 *
 *      The class reads the CAN message definition file (fctmodule_spec.xml),
 *      parses the content and creates a QHash containing all message IDs.
 *      The CAN message IDs can be accessed by ModuleID and message key.
 *
 *      The class is for temporarily use
 */
/****************************************************************************/
class CANMessageConfiguration
{
public:
    CANMessageConfiguration();

    //! Read the CAN message configuration
    ReturnCode_t ReadCANMessageConfigurationFile();

    //! Return the CAN message ID from the specified parameters
    unsigned int GetCANMessageID(quint8 bModuleID, QString strMessageKey, quint8 bIfaceID, quint32 unNodeID) const;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetLastError
     *
     *  \param usErrorID = quint16 type parameter
     *  \param strErrorInfo =  QString type parameter
     *
     *  \return from GetLastError
     */
    /****************************************************************************/
    void GetLastError(quint16& usErrorID, QString& strErrorInfo);

private:

    ReturnCode_t ParseCANMessageElement(const QDomNode &element, quint8 bFctModuleIfaceID);

    CANMessageList m_CANMessageList;    //!< List of all CAN messages

    QString m_strErrorInfo; //!< Last error information string
    quint16 m_usErrorID;    //!< Last error ID
};

} //namespace

#endif /* CANMESSAGECONFIGURATION_H */
