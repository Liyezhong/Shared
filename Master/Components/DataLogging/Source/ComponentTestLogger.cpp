/****************************************************************************/
/*! \file DataLogging/Source/ComponentTestLogger.cpp
 *
 *  \brief Implementation file for class ComponentTestLogger.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#include <DataLogging/Include/ComponentTestLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <Global/Include/EventTranslator.h>
#include <Global/Include/Utils.h>

#include <QDir>

namespace DataLogging {

static const int COMPONENTTESTLOGGER_FORMAT_VERSION = 1;        ///< Format version.

/****************************************************************************/
ComponentTestLogger::ComponentTestLogger(LoggingObject *pParent, const Global::LoggingSource &TheLoggingSource) :
    BaseLogger(pParent, TheLoggingSource, COMPONENTTESTLOGGER_FORMAT_VERSION)
{
}

/****************************************************************************/
void ComponentTestLogger::Log(const ComponentTestEntry &Entry) {
    try {
        // compute file name
        QString FileName =  QString("Test_") + Entry.GetComponentName() + "_" +
                            Entry.GetBoardName() + "_" + Entry.GetBoardSerialNumber() + "_" +
                            GetTimeStampFileName()+".log";
        QDir Dir(m_Config.GetPath());
        QString CompleteFileName(QDir::cleanPath(Dir.absoluteFilePath(FileName)));

        // create file
        CreateNewFile(CompleteFileName);
        // write header
        WriteHeader(Entry);
        // trace that file was created.
        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_DATALOGGING_INFO_FILE_CREATE, CompleteFileName
                  , Global::NO_NUMERIC_DATA, false);
        // get entry
        const Global::tTranslatableStringList &Entries = Entry.GetTestResults();
        for(Global::tTranslatableStringList::const_iterator it = Entries.constBegin(); it != Entries.constEnd(); ++it) {
            const Global::TranslatableString & EntryLine = (*it);
            // translate entry
            QString TrData  = Global::EventTranslator::TranslatorInstance().Translate(EntryLine);
            // write
            AppendLine(TrData);
        }
        // and close file
        CloseFile();
    } catch(const Global::Exception &E) {
        // send error message
        SEND_EXCEPTION(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void ComponentTestLogger::Configure(const ComponentTestLoggerConfig &Config) {
    try {
        // set new configuration.
        m_Config = Config;
    } catch(const Global::Exception &E) {
        // send error message
        SEND_EXCEPTION(E);
    } catch(...) {
        // Send error message
        LOG_EVENT(Global::EVTTYPE_FATAL_ERROR, Global::LOG_ENABLED, Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION, FILE_LINE_LIST
                  , Global::NO_NUMERIC_DATA, false);
    }
}

/****************************************************************************/
void ComponentTestLogger::WriteHeader(const ComponentTestEntry &Entry) {
    QString Line =  Entry.GetTesterName() + ";" + Entry.GetDeviceSerialNumber() + ";" +
                    Entry.GetComponentSerialNumber() + ";" + Entry.GetComponentFirmwareVersion() + ";" +
                    Entry.GetComponentHardwareVersion() + ";" + Entry.GetMainboardSoftwareVersion();
    AppendLine(Line);
}

} // end namespace DataLogging
