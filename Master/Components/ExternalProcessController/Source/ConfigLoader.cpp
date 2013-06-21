/****************************************************************************/
/*! \file ConfigLoader.cpp
 *
 *  \brief This Module loads External Process's configuration from
 *         non-volatile Memory.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.02.2011
 *   $Author:  $ Y.Novak
 *
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

#include <ExternalProcessController/Include/ConfigLoader.h>
#include <Global/Include/SystemPaths.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
 *  \brief    Constructor.
 *
 *  \param    processName = the name of the process to fetch data for
 *
 ****************************************************************************/
ConfigLoader::ConfigLoader(const QString &processName) :
        m_ProcessName(processName),
        m_StartCommand(""),
        m_RemoteLoginEnabled(""),
        m_RemoteLoginTimeout(0)
{
}

/****************************************************************************/
/*!
 *  \brief    This function sets all configurable Process parameters.
 *
 *
 *  \param    startCommand  = pointer to start command variable
 *  \param    remoteFlag  = pointer to remote mode status variable
 *  \param    timeOut = pointer to remote login timeout variable
 *
 *  \return   CLD_ALL_OK if reading parameters succeeded, ERROR otherwise
 *
 ****************************************************************************/
ConfigLoaderErrorType_t ConfigLoader::ReadSettings(QString *startCommand, QString *remoteFlag, int *timeOut)
{
    const QString fileName = Global::SystemPaths::Instance().GetSettingsPath() + CLD_PROCESS_CONFIG_FILE;
    QString tagName = CLD_PROCESS_CONFIG_TAG;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return CLD_CANNOT_OPEN_CONFIG_FILE;
    }

    ConfigLoaderErrorType_t err = LoadFile(file, tagName);
    file.close();
    if (err != CLD_ALL_OK) {
        qDebug() << "\nConfigLoader::ReadSettings:  cannot load config file " << fileName << "\nError: " << err;
        return err;
    }

    *startCommand = m_StartCommand;
    *remoteFlag = m_RemoteLoginEnabled;
    *timeOut = m_RemoteLoginTimeout;

    return CLD_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses the network configuration file.
 *
 *
 *  \param    device = device from which data shall be read
 *  \param    tag    = tag name of the configuration section
 *
 *  \return   CLD_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
ConfigLoaderErrorType_t ConfigLoader::LoadFile(QIODevice &device, const QString &tag)
{

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument domDoc;

    if (!domDoc.setContent(&device, true, &errorStr, &errorLine, &errorColumn)) {
        return CLD_CONFIG_FILE_PARSING_FAILED;
    }

    QDomElement root = domDoc.documentElement();
    if (root.tagName() != tag) {
        return CLD_CONFIG_ELEMENT_TAG_MISMATCH;
    }

    if (root.hasAttribute(CLD_ATTR_VERSION)
               && root.attribute(CLD_ATTR_VERSION) != CLD_CURRENT_VERSION) {
        return CLD_CONFIG_FILE_VERSION_MISMATCH;
    }

    ConfigLoaderErrorType_t err = ParseDocument(m_ProcessName, root);
    if (err != CLD_ALL_OK) {
        return err;
    }

    return CLD_ALL_OK;
}

/****************************************************************************/
/*!
 *  \brief    This function loads and parses the Process section of
 *            the configuration file.
 *
 *  \param    processName = name of the process, which settings shall be read
 *  \param    rootElement = root element of the XML document
 *
 *  \return   CLD_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
ConfigLoaderErrorType_t ConfigLoader::ParseDocument(const QString &processName, const QDomElement &rootElement)
{
    QDomElement processSection = rootElement.firstChildElement(CLD_PROCESS_SECTION);
    QDomElement sectionChild = processSection.firstChildElement(CLD_PROCESS_NAME);
    QString name = sectionChild.text();
    while (!processSection.isNull()) {
        // check if we found the needed section
        if (name == processName) {
            return ParseProcessSection(processSection);
        }
        processSection = processSection.nextSiblingElement();
        sectionChild = processSection.firstChildElement(CLD_PROCESS_NAME);
        name = sectionChild.text();
    }

    return CLD_PROCESSNAME_NOT_FOUND;
}

/****************************************************************************/
/*!
 *  \brief    This function extracts Process settings from XML Element
 *
 *  \param    processSection = XML section to parce
 *
 *  \return   CLD_ALL_OK if successful, otherwise error.
 *
 ****************************************************************************/
ConfigLoaderErrorType_t ConfigLoader::ParseProcessSection(const QDomElement &processSection)
{
    QDomElement cmdElement = processSection.firstChildElement(CLD_STARTCOMMAND);
    QDomElement rmlgElement = processSection.firstChildElement(CLD_REMOTE_FLAG);
    QDomElement rmtmElement = processSection.firstChildElement(CLD_REMOTETIMEOUT);

    m_StartCommand = cmdElement.text();
    m_RemoteLoginEnabled = rmlgElement.text();
    if (m_RemoteLoginEnabled == "") {
        m_RemoteLoginEnabled = "No";
    }
    bool ok = false;
    m_RemoteLoginTimeout = (rmtmElement.text()).toInt(&ok, 10);
    if ((ok == false) && (m_RemoteLoginEnabled == "Yes")) {
        /// \todo log error
        return CLD_STRINGTOINT_CONVERSION_FAILED;
    }

    // check if start command was extracted.
    if (m_StartCommand.isEmpty() && (m_RemoteLoginEnabled == "No")) {
        return CLD_STARTCOMMAND_EMPTY;
    }

    return CLD_ALL_OK;
}

} // end namespace
