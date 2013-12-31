/****************************************************************************/
/*! \file ServicePassword.cpp
 *
 *  \brief Implementation file for class CServicePassword.
 *
 *  \b Description:
 *         Reads the "LBSaccess.key" file, computes the MD5
 *         checksum for the key file and entered PIN.
 *         Mounts and unmounts the USB.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-15
 *  $Author:    $ Raju
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

#include "PasswordManager/Include/ServicePassword.h"
#include "DataManager/Helper/Include/Helper.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/EventObject.h"
#include "Global/Include/Utils.h"
#include "PasswordManager/Include/PasswordManagerEventCodes.h"


namespace PasswordManager {

// constant for secret key
const QString STRING_SECRET_KEY             = "BiosystemsSecret"; ///< Secret key

// constants for system directory paths
const QString DIRECTORY_MNT                 = "/mnt"; ///< constant for the mnt directory
const QString DIRECTORY_DEV                 = "/dev"; ///< constant for the dev directory
const QString DIRECTORY_SH                  = "/bin/sh"; ///< constant for the shell directory
const QString DIRECTORY_SD                  = "sd"; ///< constant for the sd directory

// constants for commands
const QString COMMAND_MOUNT                 = "mount "; ///< constant string for the command 'mount'
const QString COMMAND_UNMOUNT               = "umount "; ///< constant string for the command 'umount'
const QString COMMAND_LS                    = "ls "; ///< constant string for the command 'ls'
const QString COMMAND_ARGUMENT_C            = "-c"; ///< constant string for the command argument for shell '-c'
// constants for wildcharacters
const QString WILDCHAR_ASTRIK               = "*"; ///< constant for wild char

const QString XML_TAG_ACCESSFILE            = "<accessfile>"; ///< accessfile tag
const QString XML_TAG_CHECK                 = "<check>"; ///< check tag
const QString XML_NODE_CHECK                = "check"; ///< check node string
const QString XML_NODE_BASIC                = "basic"; ///< basic node string
const QString XML_NODE_INSTRUMENT           = "instrument"; ///< instrument node string
const QString XML_NODE_INSTRUMENTS          = "instruments"; ///< instruments node string
const QString XML_NODE_NAME                 = "name"; ///< name node string
const QString XML_NODE_ADVANCED             = "advanced"; ///< advanced node string
const QString XML_NODE_VALIDTO              = "validto"; ///< validto node string
const QString XML_NODE_HASH                 = "hash"; ///< hash node string
const QString XML_NODE_MOBILEID             = "mobileID"; ///< mobile ID node string

const QString STRING_SPACE                  = " "; ///< constant string for space
const QString STRING_NEWLINE                = "\n"; ///< constant string for new line

const QString FILE_LBSACCESS                = "LBSaccess.xml"; ///< service key file name

/****************************************************************************/
CServicePassword::CServicePassword(const QString &PinValue, const QString &DeviceName):
    m_ServicePin(PinValue),
    m_DeviceName(DeviceName) {

    m_ServiceFileContent.clear();
    m_Authentication = AUTHENTICATION_VALID;
}

/****************************************************************************/
bool CServicePassword::ReadTheServiceKeyFile() {

    // check for the file existence in the mounted device.
    qint32 MountedValue = Global::MountStorageDevice(FILE_LBSACCESS);

    switch (MountedValue) {
        default:
            Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_NO_USB_DEVICE_EXISTS);
            break;

        case 3:
            Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_KEY_FILE_NOT_FOUND);
            break;

        case 0:
            QFile ServiceFile(Global::DIRECTORY_MNT_STORAGE + QDir::separator() + FILE_LBSACCESS);
            if (ServiceFile.open(QIODevice::ReadOnly)) {
                m_ServiceFileContent.clear();
                while(!ServiceFile.atEnd()) {
                    m_ServiceFileContent.append(ServiceFile.readLine().trimmed());
                }
                ServiceFile.close();
            }
            // unmount the device once the task is done
            Global::UnMountStorageDevice();
            return true;
    }
    m_Authentication = NO_SERVICE_KEY_FILE;
    return false;
}

/****************************************************************************/
QString CServicePassword::ComputeCheckSum() {
    if (m_ServiceFileContent.length() > 0) {
        QString ServiceFileContent(m_ServiceFileContent);

        qint32 StringIndexValue = ServiceFileContent.indexOf(XML_TAG_ACCESSFILE);
        if (StringIndexValue != -1) {
            // removes the characters from the index of "<access>" tag
            ServiceFileContent = ServiceFileContent.mid(StringIndexValue + XML_TAG_ACCESSFILE.length());

            StringIndexValue = ServiceFileContent.lastIndexOf(XML_TAG_CHECK);
            if (StringIndexValue != -1) {
                // removes the characters from the index of "<check>" tag
                ServiceFileContent.chop(ServiceFileContent.length() - StringIndexValue);

                // compute the checksum
                QByteArray Md5CheckSum(QCryptographicHash::hash(ServiceFileContent.toUtf8(),
                                                             QCryptographicHash::Md5).toHex().toUpper());
                qDebug() << Md5CheckSum;
                return Md5CheckSum;
            }
        }
    }
    return "";
}

/****************************************************************************/
QString CServicePassword::ComputeHash() {

    QString Value = STRING_SECRET_KEY + m_ServicePin;
    qDebug() << QCryptographicHash::hash(Value.toUtf8(), QCryptographicHash::Md5).toHex().toUpper();
    return QCryptographicHash::hash(Value.toUtf8(), QCryptographicHash::Md5).toHex().toUpper();
}

/****************************************************************************/
bool CServicePassword::CompareTheCheckSum() {

    QXmlStreamReader ServiceStreamReader(m_ServiceFileContent);
    (void)ServiceStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534
    if (DataManager::Helper::ReadNode(ServiceStreamReader, XML_NODE_CHECK)) {
        if (ServiceStreamReader.readElementText().toUpper().compare(ComputeCheckSum()) == 0) {            
            return true;
        }
    }
    m_Authentication = CHECKSUM_NOT_MATCHING;
    Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_CHECKSUM_NOT_MATCHED);
    return false;
}

/****************************************************************************/
bool CServicePassword::ReadDeviceNameTagsExistence() {

    QXmlStreamReader ServiceStreamReader(m_ServiceFileContent);

    bool InstrumentNode = false;
    bool DeviceNameExists = false;
    bool LogDeviceNameExistence = false;

    (void)ServiceStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534

    if (DataManager::Helper::ReadNode(ServiceStreamReader, XML_NODE_INSTRUMENTS)) {

        while (!ServiceStreamReader.atEnd()) {
            if (ServiceStreamReader.readNext() == QXmlStreamReader::Invalid) {
                qDebug() << "Reading " << ServiceStreamReader.name() << " at line number: " << ServiceStreamReader.lineNumber();
                qDebug() << "Invalid Token. Error: " << ServiceStreamReader.errorString();
            }
            if (ServiceStreamReader.isStartElement()) {
                // check for the node name <instrument>
                if (ServiceStreamReader.name().compare(XML_NODE_INSTRUMENT) == 0) {
                    InstrumentNode = true;
                }

                if (InstrumentNode) {
                    // check for the node name <name>
                    if (ServiceStreamReader.name().compare(XML_NODE_NAME) == 0) {
                        // check the device name existence
                        if (ServiceStreamReader.readElementText().compare(m_DeviceName) == 0) {
                            DeviceNameExists = true;
                            LogDeviceNameExistence = true;
                        }
                    }
                    if (DeviceNameExists) {
                        // check for the node name <basic>
                        if (ServiceStreamReader.name().compare(XML_NODE_BASIC) == 0) {
                            if (ServiceStreamReader.readElementText().compare("1") == 0) {
                                return true;
                            }
                        }
                    }
                }
            }
            // check for the end tag
            else if (ServiceStreamReader.isEndElement()) {
                // check for the node name </instrument>
                if (ServiceStreamReader.name() == XML_NODE_INSTRUMENT) {
                    InstrumentNode = false;
                    DeviceNameExists = false;
                }
            }
        }
    }

    if (!LogDeviceNameExistence) {
        m_Authentication = NO_DEVICE_TAG;
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_DEVICENAME_NOT_EXISTS);
    }
    else {
        m_Authentication = BASIC_TAG_NOT_MATCHING;
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_BASIC_TAG_VALUE_IS_WRONG);
    }

    return false;
}

/****************************************************************************/
bool CServicePassword::CompareDate() {

    QXmlStreamReader ServiceStreamReader(m_ServiceFileContent);
    (void)ServiceStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534
    if (DataManager::Helper::ReadNode(ServiceStreamReader, XML_NODE_VALIDTO)) {
        QString DateValue = ServiceStreamReader.readElementText();
        QDateTime ValidDateFromFile(QDate::fromString(DateValue, "yyyyMMdd"));
        // add one day to consider proper time, because it considers time as 00:00:00 instead of 23:59:59
        // for example if the date is 01.11.2012 then the date time will be '01.11.2012 00:00:00'
        // but it should consider '01.11.2012 23:59:59'
        // always date is stored in the format of yyyyMMdd
        qint32 TimeDifference = ValidDateFromFile.addDays(1).toTime_t()
                - Global::AdjustedTime::Instance().GetCurrentDateTime().toTime_t();
        if (TimeDifference >= 0) {            
            return true;
        }
    }
    m_Authentication = DATE_EXPIRED;
    Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_DATE_IS_EXPIRED);
    return false;
}

/****************************************************************************/
bool CServicePassword::CompareHash() {

    QXmlStreamReader ServiceStreamReader(m_ServiceFileContent);
    (void)ServiceStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534
    if (DataManager::Helper::ReadNode(ServiceStreamReader, XML_NODE_HASH)) {
        if (ServiceStreamReader.readElementText().toUpper().compare(ComputeHash()) == 0) {
            return true;
        }
    }
    m_Authentication = HASH_NOT_MATCHING;
    Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_HASH_NOT_MATCHED);
    return false;
}


/****************************************************************************/
QString CServicePassword::ReadServiceID() {

    QXmlStreamReader ServiceStreamReader(m_ServiceFileContent);
    (void)ServiceStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534
    if (DataManager::Helper::ReadNode(ServiceStreamReader, XML_NODE_MOBILEID)) {
        return ServiceStreamReader.readElementText();
    }

    return "";
}

/****************************************************************************/
bool CServicePassword::ValidateAuthentication() {

    bool CheckSumMatched = false;
    if (ReadTheServiceKeyFile()) {
        if (CompareHash()) {
            if (CompareTheCheckSum()) {
                CheckSumMatched = true;
            }
        }
    }

    if (CheckSumMatched && CompareDate()) {
        if (ReadDeviceNameTagsExistence()) {
            // log the service ID
            Global::EventObject::Instance().RaiseEvent
                    (EVENT_PASSWORDMANAGER_LOG_SERVICE_ID, Global::FmtArgs() << ReadServiceID(), true);
            m_Authentication = AUTHENTICATION_VALID;
            return true;
        }
    }

    return false;
}


}
