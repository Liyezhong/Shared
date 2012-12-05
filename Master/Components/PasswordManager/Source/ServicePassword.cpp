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

const QString FILE_LBSACCESS                = "LBSaccess.key"; ///< service key file name

/****************************************************************************/
CServicePassword::CServicePassword(const QString &PinValue, const QString &DeviceName):
    m_ServicePin(PinValue),
    m_DeviceName(DeviceName) {

    m_ServiceFileContent.clear();
}

/****************************************************************************/
bool CServicePassword::SearchAndMountTheDevice() {
    return true;
    // create the QProcess
    QProcess ProcToMountUSB;
    bool DevicesExists = false;
    // set the working directory "/dev"
    (void)ProcToMountUSB.setWorkingDirectory(DIRECTORY_DEV); //to avoid lint-534

    QStringList Options;
    // -c option for shell and ls is to search for the sda, sdb devices "ls sd*"
    Options << COMMAND_ARGUMENT_C << COMMAND_LS + DIRECTORY_SD + WILDCHAR_ASTRIK;
    // start the process with grep command  "/bin/sh"
    ProcToMountUSB.start(DIRECTORY_SH, Options);
    // check for the process finished
    if (ProcToMountUSB.waitForFinished()) {
        // store all the device names from the standard input
        QString Devices(ProcToMountUSB.readAllStandardOutput());
        if (Devices.length() > 0) {
            DevicesExists = true;
            if (Devices.split(STRING_NEWLINE).length() > 0) {
                for (int DeviceCount = 0; DeviceCount < Devices.split(STRING_NEWLINE).length(); DeviceCount++) {
                    if (ProcToMountUSB.waitForFinished()) {
                        // mount one device at a time
                        if (MountTheSpecificDevice(ProcToMountUSB, Devices.split(STRING_NEWLINE).value(DeviceCount))) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    if (!DevicesExists) {
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_NO_USB_DEVICE_EXISTS);
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_KEY_FILE_NOT_FOUND);
    }
    return false;
}

/****************************************************************************/
bool CServicePassword::MountTheSpecificDevice(const QProcess &Process,
                                                          QString DeviceName) {
    /// uncomment the first line of the code to test in VMWare
    return true;
    bool ReadTheFile = false;
    QProcess& ProcToMount = const_cast<QProcess&>(Process);
    // remove the const cast
    // "mount /dev/sda /mnt"
    QString CommandName = COMMAND_MOUNT + DIRECTORY_DEV + QDir::separator()
            + DeviceName + STRING_SPACE
            + DIRECTORY_MNT;
    // merge the channels so that all the data on the stdout can be read easily
    ProcToMount.setProcessChannelMode(QProcess::MergedChannels);
    // mount the device
    ProcToMount.start(CommandName);

    if (ProcToMount.waitForFinished()) {
        // save the data in temporay string
        QString StdOutData(ProcToMount.readAllStandardOutput());
        // if the mount is successful then standrad output will be empty string
        if (StdOutData.length() == 0) {
            if (QFile::exists(DIRECTORY_MNT + QDir::separator() + FILE_LBSACCESS)) {

                QFile ServiceFile(DIRECTORY_MNT + QDir::separator() + FILE_LBSACCESS);
                if (ServiceFile.open(QIODevice::ReadOnly)) {
                    m_ServiceFileContent.clear();
                    while(!ServiceFile.atEnd()) {
                        m_ServiceFileContent.append(ServiceFile.readLine().trimmed());
                    }
                    ServiceFile.close();
                    ReadTheFile = true;
                }
            }
        }
    }
    // unmount the device - most of the times it unmounts the device  "umount /mnt"
    ProcToMount.start(COMMAND_UNMOUNT + DIRECTORY_MNT);

    return ReadTheFile;
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
    Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_CHECKSUM_NOT_MATCHED);
    return false;
}

/****************************************************************************/
bool CServicePassword::ReadDeviceNameTagsExistence() {

    QXmlStreamReader ServiceStreamReader(m_ServiceFileContent);

    bool InstrumentNode = false;
    bool DeviceNameExists = false;
    bool LogDeviveNameExistence = false;

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
                            LogDeviveNameExistence = false;
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

    if (LogDeviveNameExistence) {
        Global::EventObject::Instance().RaiseEvent(EVENT_PASSWORDMANAGER_DEVICENAME_NOT_EXISTS);
    }
    else {
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
        // always date is stored in the format of yyyyMMdd
        qint32 TimeDifference = ValidDateFromFile.toTime_t()
                - Global::AdjustedTime::Instance().GetCurrentDateTime().toTime_t();
        if (TimeDifference >= 0) {            
            return true;
        }
    }
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

    QFile ServiceFile(DIRECTORY_MNT + QDir::separator() + FILE_LBSACCESS);

    ServiceFile.open(QIODevice::ReadOnly);
    m_ServiceFileContent.clear();
    while(!ServiceFile.atEnd()) {
        m_ServiceFileContent.append(ServiceFile.readLine().trimmed());
    }
    ServiceFile.close();
    m_DeviceName = "ST8200";

    bool CheckSumMatched = false;
    if (SearchAndMountTheDevice()) {
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

            return true;
        }
    }

    return false;
}


}
