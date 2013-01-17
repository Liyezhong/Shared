//****************************************************************************/
/*! \file CANMessageConfiguration.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANMessageConfiguration
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

#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "DeviceControl/Include/Global/DeviceControlEventCodes.h"
#include "Global/Include/SystemPaths.h"

#include <QDomDocument>
#include <QFile>

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
CANMessageConfiguration::CANMessageConfiguration() : m_EventCode(0)
{
}

/****************************************************************************/
/*!
 *  \brief  Returns the last error ID and information string
 *
 *  \xparam usErrorID = error ID
 *  \xparam strErrorInfo = error information string
 */
/****************************************************************************/
void CANMessageConfiguration::GetLastError(quint32 &EventCode, QString& strErrorInfo)
{
    strErrorInfo = m_strErrorInfo;
    EventCode = m_EventCode;
}

/****************************************************************************/
/*!
 *  \brief  Read the CAN message configuration file
 *
 *      When this function has finished, the m_CANMessageList is filled with all
 *      CAN-message information from the configuration file
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise the error return code
 */
/****************************************************************************/
ReturnCode_t CANMessageConfiguration::ReadCANMessageConfigurationFile()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QString errorStr;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/fctmodule_spec.xml";
    FILE_LOG_L(laCONFIG_FILE, llINFO) <<  "  read configuration from '" << fileName.toStdString() << "'";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        errorMsg = QObject::tr("Cannot read file %1:\n%2.")
                                                     .arg(fileName)
                                                     .arg(file.errorString());
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_OPEN_FAILED;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn))
    {
        errorMsg = QObject::tr("Parse error at line %1, column %2:\n%3")
                                                         .arg(errorLine)
                                                         .arg(errorColumn)
                                                         .arg(errorStr);
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_PARSE;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "colorado_fct_module_spec")
    {
        errorMsg = QObject::tr("The file is not a colorado function module specification file. Tag 'colorado_fct_module_spec' missed!");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT;
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
                       && root.attribute("version") != "1.0")
    {
        errorMsg = QObject::tr("The colorado hardware config files' version is not valid.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_VERSION;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    //##############################
    // base modul
    //##############################
    QDomElement child = root.firstChildElement("base_modules");
    if(child.isNull())
    {
        errorMsg = QObject::tr("  child 'base_modules' not found.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_BASE;
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomNode node = child.firstChildElement();
        if(node.nodeName() == "base_module")
        {
            QDomNode node1 = node.firstChildElement();
            FILE_LOG_L(laCONFIG_FILE, llDEBUG) <<  "node base_module found.";
            if(node1.nodeName() == "CAN_messages")
            {
                ReturnCode_t RetCode;
                QDomNode node2 = node1.firstChildElement();

                while (!node2.isNull())
                {
                    RetCode = ParseCANMessageElement(node2, 0);
                    if(RetCode != DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_MSG;
                        return DCL_ERR_FCT_CALL_FAILED;
                    }
                    node2 = node2.nextSiblingElement("CAN_message");
                }
            }
            else
            {
                errorMsg = QObject::tr("  child 'CAN_messages' not found.");
                FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

                m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_MSG;
                return DCL_ERR_FCT_CALL_FAILED;
            }
        }
        else
        {
            errorMsg = QObject::tr("  child 'base_module' not found.");
            FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

            m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_BASE;
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    //##############################
    // function modules
    //##############################
    child = root.firstChildElement("function_modules");
    if(child.isNull())
    {
        errorMsg = QObject::tr("  child 'function_modules' not found.");
        FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_FCT;
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomNamedNodeMap attrmapFctNode;
        QDomNode nodeModuleID, nodeFctModuleType;
        QString strModuleID, strFctModuleType;
        quint8 bModuleID;
        bool ok;

        QDomNode nodeFctModule = child.firstChildElement();
        FILE_LOG_L(laCONFIG_FILE, llDEBUG) <<  "function_modules " << nodeFctModule.nodeName().toStdString() << " found.";
        while(!nodeFctModule.isNull())
        {
            if(nodeFctModule.nodeName() == "function_module")
            {
                // extract the modules' type and interface ID
                attrmapFctNode = nodeFctModule.attributes();
                nodeFctModuleType = attrmapFctNode.namedItem("type");
                strFctModuleType = nodeFctModuleType.nodeValue();
                nodeModuleID = attrmapFctNode.namedItem("moduleID");
                strModuleID = nodeModuleID.nodeValue();
                bModuleID = strModuleID.toShort(&ok, 10);

                QDomNode node1 = nodeFctModule.firstChildElement();
                if(node1.nodeName() == "CAN_messages")
                {
                    ReturnCode_t RetCode;
                    QDomNode node2 = node1.firstChildElement();
                    while (!node2.isNull())
                    {
                        RetCode = ParseCANMessageElement(node2, bModuleID);
                        if(RetCode != DCL_ERR_FCT_CALL_SUCCESS)
                        {
                            m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_MSG;
                            return DCL_ERR_FCT_CALL_FAILED;
                        }
                        node2 = node2.nextSiblingElement("CAN_message");
                    }
                }
                else
                {
                    errorMsg = QObject::tr("  child 'CAN_messages' not found.");
                    FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

                    m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_MSG;
                    return DCL_ERR_FCT_CALL_FAILED;
                }
            }
            else
            {
                errorMsg = QObject::tr("  child 'function_module' not found.");
                FILE_LOG_L(laCONFIG_FILE, llERROR) << errorMsg.toStdString();

                m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_FCT;
                return DCL_ERR_FCT_CALL_FAILED;
            }
            nodeFctModule = nodeFctModule.nextSiblingElement("function_module");
        }
    }

    FILE_LOG_L(laCONFIG_FILE, llINFO) <<  " read configuration... success.";

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Parse the can message's structure information from xml
 *
 *  \iparam element = the xml-node containing the message information
 *  \iparam bModuleID = interface  module ID
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise the error return code
 */
/****************************************************************************/
ReturnCode_t CANMessageConfiguration::ParseCANMessageElement(const QDomNode &element, quint8 bModuleID)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;
    QString strCANMsgKeyComplete;
    QString strCANMsgKey, strCANMsgCmdClass, strCANMsgCmdCode, strCANMsgMS;
    QString strModuleID;
    unsigned int unCANMsgID, unCmdClass, unCmdCode = 0 , unCmdMS = 0;
    bool bOK = false;

    // access the nodes' attributes
    QDomNamedNodeMap attrmap = element.attributes();
    QDomNode nodeKey = attrmap.namedItem("key");
    QDomNode nodeCmdType = attrmap.namedItem("commandclass");
    QDomNode nodeCmdCode = attrmap.namedItem("commandcode");
    QDomNode nodeCmdDir = attrmap.namedItem("ms");

    strCANMsgKey = nodeKey.nodeValue();// element.attribute("key");
    strCANMsgCmdClass = nodeCmdType.nodeValue();
    strCANMsgCmdCode = nodeCmdCode.nodeValue();
    strCANMsgMS = nodeCmdDir.nodeValue();

    // extract the numeric information (cut the preceding "0x")
    strCANMsgCmdClass = strCANMsgCmdClass.mid(2, 1);
    strCANMsgCmdCode = strCANMsgCmdCode.mid(2, 2);
    strCANMsgMS = strCANMsgMS.mid(2, 1);
    // convert the strings to numeric values
    unCmdClass = strCANMsgCmdClass.toInt(&bOK, 16);
    if(bOK == true)
    {
        unCmdCode = strCANMsgCmdCode.toInt(&bOK, 16);
    }
    if(bOK == true)
    {
        unCmdMS = strCANMsgMS.toInt(&bOK, 16);
    }

    //check for conversation errors
    if(bOK == false)
    {
        m_EventCode = EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_MSG;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    strModuleID = QString("%1").arg(bModuleID, 2, 10, QChar('0'));
    // generate the CAN-messages' key by adding the interface ID module and the message key
    strCANMsgKeyComplete = strModuleID + "_" + strCANMsgKey;

    // the CAN-ID will be stored with command type and command index offset
    // to calculate the finally ID, the command index offset must be added to the messages' base command indes
    unCANMsgID = unCmdClass << 26;
    unCANMsgID |= unCmdCode << 19;
    unCANMsgID |= (unCmdMS & 0x0001);
    //if(unCmdMS == 0)
    //    unCANMsgID |= (0x00000001);

    // add it to the list
    m_CANMessageList.insert(strCANMsgKeyComplete, unCANMsgID);

    FILE_LOG_L(laCONFIG_FILE, llDEBUG2) << "added CAN message " << strCANMsgKeyComplete.toStdString() << ": 0x" << std::hex << unCANMsgID;

    return retCode;
}


/****************************************************************************/
/*!
 *  \brief  Calculate a CAN-message ID according to the specified parameters
 *
 *  \iparam bModuleID = The xml-node containing the message information
 *  \iparam strMessageKey = The CAN-message's unique identifier string
 *  \iparam channel = The channel of the module which belongs to the message
 *  \iparam unNodeID = The CAN-node id, composed by type and index
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successful, otherwise the error return code
 */
/****************************************************************************/
unsigned int CANMessageConfiguration::GetCANMessageID(quint8 bModuleID, QString strMessageKey, quint8 channel, quint32 unNodeID) const
{
    unsigned int unCANMsgID;
    QString strCANMsgKeyCompl, strModuleID;

    strModuleID = QString("%1").arg(bModuleID, 2, 10, QChar('0'));
    strCANMsgKeyCompl = strModuleID + "_" + strMessageKey;
    if(m_CANMessageList.contains(strCANMsgKeyCompl))
    {
        unCANMsgID = m_CANMessageList[strCANMsgKeyCompl];

        unCANMsgID |= ((quint32) channel) << 12;
        unCANMsgID |= (unNodeID & 0x00000F00);
        unCANMsgID |= (unNodeID & 0x000000FF) << 1;
    }
    else
    {
        FILE_LOG_L(laCOMM, llWARNING) << "Requested msg id not found: (ModulID/MsgKey/IFace/nodeID)";
        FILE_LOG_L(laCOMM, llWARNING) << "  0x" << std::hex << (int) bModuleID << "/" << strMessageKey.toStdString() << " / 0x" << std::hex << (int)channel << " / 0x" << std::hex << unNodeID;
        unCANMsgID = GET_CAN_MSG_RR_FLAG;
    }

    return unCANMsgID;
}

} //namespace
