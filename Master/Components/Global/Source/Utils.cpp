/****************************************************************************/
/*! \file Global/Source/Utils.cpp
 *
 *  \brief Some utility functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-18
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

#include <Global/Include/Utils.h>
#include <Global/Include/SystemPaths.h>
#include <QHash>
#include <QStringList>
#include <QProcess>
#include <QtGlobal>

#include <errno.h>
#include <iostream> // ofstream
#include <fstream>  //redirecting output to /dev/tty
#include <unistd.h> //for fsync
namespace Global {

const QString QT_QWS_FONTDIR_STRING = "QT_QWS_FONTDIR";   //!< command option

/****************************************************************************/
QLocale::Language StringToLanguage(const QString &LanguageName) {
    // takes lowercase, two-letter, ISO 639 language code if it does not find then return QlOcale::c
    return QLocale(LanguageName).language();
}

/****************************************************************************/
QString LanguageToString(const QLocale::Language TheLanguage) {
    // returns lowercase, two-letter, ISO 639 language code for english it is en and for german de
    return LanguageToLanguageCode(TheLanguage);
}

/****************************************************************************/
QString LanguageToLanguageCode(const QLocale::Language TheLanguage) {
    QLocale loc(TheLanguage, QLocale::AnyCountry);
    QStringList List = loc.name().split('_');
    if(List.size() != 2) {
        // some error occured
        return "";
    }
    return List[0];
}

/****************************************************************************/
QString GUIMessageTypeToString(GUIMessageType TheGUIMessageType) {
    QString Result = "undefined";
    switch(TheGUIMessageType) {
        case GUIMSGTYPE_UNDEFINED:  Result = "Undefined"; break;
        case GUIMSGTYPE_ERROR:      Result = "Error"; break;
        case GUIMSGTYPE_WARNING:    Result = "Warning"; break;
        case GUIMSGTYPE_INFO:       Result = "Info"; break;
    };
    return Result;
}

/****************************************************************************/
GUIMessageType StringToGUIMessageType(const QString &GUIMessageTypeString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    GUIMessageType Result = GUIMSGTYPE_UNDEFINED;
    if(GUIMessageTypeString.compare("Undefined", cs) == 0) {
        Result = GUIMSGTYPE_UNDEFINED;
    } else if(GUIMessageTypeString.compare("Error", cs) == 0) {
        Result = GUIMSGTYPE_ERROR;
    } else if(GUIMessageTypeString.compare("Warning", cs) == 0) {
        Result = GUIMSGTYPE_WARNING;
    } else if(GUIMessageTypeString.compare("Info", cs) == 0) {
        Result = GUIMSGTYPE_INFO;
    }
    return Result;
}

/****************************************************************************/
QString TemperatureFormatToString(TemperatureFormat TheTemperatureFormat) {
    QString Result = "undefined";
    switch(TheTemperatureFormat) {
        case TEMP_FORMAT_UNDEFINED:     Result = "Undefined"; break;
        case TEMP_FORMAT_CELSIUS:       Result = "C"; break;
        case TEMP_FORMAT_FAHRENHEIT:    Result = "F"; break;
    };
    return Result;
}

/****************************************************************************/
TemperatureFormat StringToTemperatureFormat(const QString &TemperatureFormatString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    TemperatureFormat Result = TEMP_FORMAT_UNDEFINED;
    if(TemperatureFormatString.compare("C", cs) == 0) {
        Result = TEMP_FORMAT_CELSIUS;
    } else if(TemperatureFormatString.compare("F", cs) == 0) {
        Result = TEMP_FORMAT_FAHRENHEIT;
    }
    return Result;
}

/****************************************************************************/
QString OnOffStateToString(OnOffState TheState) {
    QString Result = "undefined";
    switch(TheState) {
        case ONOFFSTATE_UNDEFINED:  Result = "Undefined"; break;
        case ONOFFSTATE_ON:         Result = "On"; break;
        case ONOFFSTATE_OFF:        Result = "Off"; break;
    };
    return Result;
}

/****************************************************************************/
OnOffState StringToOnOffState(const QString &OnOffStateString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    OnOffState Result = ONOFFSTATE_UNDEFINED;
    if(OnOffStateString.compare("On", cs) == 0) {
        Result = ONOFFSTATE_ON;
    } else if(OnOffStateString.compare("Off", cs) == 0) {
        Result = ONOFFSTATE_OFF;
    }
    return Result;
}

/****************************************************************************/
QString DateFormatToString(DateFormat TheFormat) {
    QString Result = "Undefined";
    switch(TheFormat) {
        case DATE_UNDEFINED:        Result = "Undefined"; break;
        case DATE_INTERNATIONAL:    Result = "International"; break;
        case DATE_ISO:              Result = "ISO"; break;
        case DATE_US:               Result = "US"; break;
    };
    return Result;
}

/****************************************************************************/
DateFormat StringToDateFormat(const QString &DateFormatString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    DateFormat Result = DATE_UNDEFINED;
    if(DateFormatString.compare("International", cs) == 0) {
        Result = DATE_INTERNATIONAL;
    } else if(DateFormatString.compare("ISO", cs) == 0) {
        Result = DATE_ISO;
    } else if(DateFormatString.compare("US", cs) == 0) {
        Result = DATE_US;
    }
    return Result;
}

/****************************************************************************/
QString TimeFormatToString(TimeFormat TheFormat) {
    QString Result = "Undefined";
    switch(TheFormat) {
        case TIME_UNDEFINED:    Result = "Undefined"; break;
        case TIME_24:           Result = "24"; break;
        case TIME_12:           Result = "12"; break;
    };
    return Result;
}

/****************************************************************************/
TimeFormat StringToTimeFormat(const QString &TimeFormatString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    TimeFormat Result = TIME_UNDEFINED;
    if(TimeFormatString.compare("24", cs) == 0) {
        Result = TIME_24;
    } else if(TimeFormatString.compare("12", cs) == 0) {
        Result = TIME_12;
    }
    return Result;
}

/****************************************************************************/
QString OvenStartModeToString(OvenStartMode TheMode) {
    QString Result = "Undefined";
    switch(TheMode) {
        case OVENSTART_UNDEFINED:       Result = "Undefined"; break;
        case OVENSTART_AFTER_STARTUP:   Result = "AfterStartup"; break;
        case OVENSTART_BEFORE_PROGRAM:  Result = "BeforeProgram"; break;
    };
    return Result;
}

/****************************************************************************/
OvenStartMode StringToOvenStartMode(const QString &OvenStartModeString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    OvenStartMode Result = OVENSTART_UNDEFINED;
    if(OvenStartModeString.compare("AfterStartup", cs) == 0) {
        Result = OVENSTART_AFTER_STARTUP;
    } else if(OvenStartModeString.compare("BeforeProgram", cs) == 0) {
        Result = OVENSTART_BEFORE_PROGRAM;
    }
    return Result;
}

/****************************************************************************/
QString WaterTypeToString(WaterType TheType) {
    QString Result = "Undefined";
    switch(TheType) {
        case WATER_TYPE_UNDEFINED:  Result = "Undefined"; break;
        case WATER_TYPE_TAP:        Result = "Tap"; break;
        case WATER_TYPE_DISTILLED:  Result = "Distilled"; break;
    };
    return Result;
}

/****************************************************************************/
WaterType StringToWaterType(const QString &WaterTypeString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    WaterType Result = WATER_TYPE_UNDEFINED;
    if(WaterTypeString.compare("Tap", cs) == 0) {
        Result = WATER_TYPE_TAP;
    } else if(WaterTypeString.compare("Distilled", cs) == 0) {
        Result = WATER_TYPE_DISTILLED;
    }
    return Result;
}

/****************************************************************************/
QString BoolToStringYesNo(bool YesNo) {

    QString Result = "No";
    if(YesNo) {
         Result = "Yes";
    }
    return Result;
}
/****************************************************************************/
bool StringToTrueFalse(const QString &YesNoStateString, bool CaseSensitive) {
    Qt::CaseSensitivity cs = CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool Result = false;
    if(YesNoStateString.compare("Yes", cs) == 0) {
        Result = true;
    }
    return Result;
}
/****************************************************************************/
quint32 GetButtonCountFromButtonType(GuiButtonType ButtonType) {
    switch (ButtonType) {
    case Global::OK:
        return 1;
    case Global::OK_CANCEL:
    case Global::YES_NO:
    case Global::CONTINUE_STOP:
        return 2;
    case Global::WITHOUT_BUTTONS:
        return 0;
    case Global::INVISIBLE:
        return 0;
    case Global::YES_NO_CANCEL:
        return 3;

    default:
        return 1;
    }
    //return 1;
}

/****************************************************************************/
Global::GuiButtonType StringToGuiButtonType(QString ButtonTypeString) {
    ButtonTypeString = ButtonTypeString.simplified().toUpper();
    if (ButtonTypeString ==  "YES+NO") {
        return Global::YES_NO;
    }
    else if (ButtonTypeString == "OK+CANCEL") {
        return Global::OK_CANCEL;
    }
    else if (ButtonTypeString == "CONTINUE+STOP") {
        return Global::CONTINUE_STOP;
    }
    else if (ButtonTypeString == "INVISIBLE") {
        return Global::INVISIBLE;
    }
    else if (ButtonTypeString == "OK") {
        return Global::OK;
    }
    else if (ButtonTypeString == "WB") {
        return Global::WITHOUT_BUTTONS;
    }
    return Global::NOT_SPECIFIED;
}

/****************************************************************************/
bool CompareDate(QDate CurrentDate, QDate DateToBeCompared) {
    if (DateToBeCompared.year() > CurrentDate.year())
    {
        return true;
    }
    else if (DateToBeCompared.year() == CurrentDate.year()) {
        if (DateToBeCompared.month() > CurrentDate.month())
        {
            return true;
        }
        else if (DateToBeCompared.month() == CurrentDate.month())
        {
            if (DateToBeCompared.day() >= CurrentDate.day())
            {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

/****************************************************************************/
qint32 MountStorageDevice(QString Name) {
    // create the QProcess
    Q_UNUSED(Name);
#if defined(__arm__) //Target
    QProcess ProcToMountDevice;
    QStringList CmdLineOptions;

    CmdLineOptions << "mount_usb";
    // check whether anything needs to be  searched in the mounted device
    if (Name.compare("") != 0) {
        CmdLineOptions << "search" << Name;
    }

    ProcToMountDevice.start(MNT_SCRIPT, CmdLineOptions);
    // check for the process finished
    if (ProcToMountDevice.waitForFinished()) {
        return ProcToMountDevice.exitCode();
    }
    return 1;
#endif
    if(!QFile(DIRECTORY_MNT_STORAGE).exists()){
        return 1;
    }
    return 0;
}

/****************************************************************************/
void UnMountStorageDevice() {
    QProcess ProcToMountDevice;
    ProcToMountDevice.start(MNT_SCRIPT, QStringList() << "umount_usb");
    (void)ProcToMountDevice.waitForFinished();
}

void DumpToConsole(const QString StringToDump) {
#if defined(__arm__) //Target
    (void)system("./clearframebuffer &");
    (void)system("lcd on");
    /* Why do we use "ofstream" instead of std::cout/printf ?
     * unfortunately, when main software is started from init scripts, it doesnt get a controlling
     * terminal, hence the printf/cout doesnt print it out on the LCD .
     * so as a workaround , we dump strings to tty1 which is associated with LCD.
     */
    std::ofstream Tty("/dev/tty1");
    Tty << StringToDump.toStdString();
#else
    Global::ToConsole(StringToDump);
#endif
}

void SetThreadPriority(const ThreadPrio_t ThreadPriority)
{
    int ReturnVal;
    // We'll operate on the currently running thread.
    pthread_t this_thread = pthread_self();

    // struct sched_param is used to store the scheduling priority
    struct sched_param params;

    if (ThreadPriority == LOW_PRIO) {
        //For schedule idle, priority must be set to 0, else
        //setting priority will fail.
        params.__sched_priority = 0;

        // Attempt to set thread priority to the SCHED_IDLE policy
        ReturnVal = pthread_setschedparam(this_thread, SCHED_IDLE | SCHED_RESET_ON_FORK, &params);
    }
    else if (ThreadPriority == HIGH_PRIO) {
        // We'll set the priority to the maximum.
        params.sched_priority = sched_get_priority_min(SCHED_RR);

        // Attempt to set thread real-time priority to the SCHED_FIFO policy
        //SCHED_RESET_ON_FORK -> Reset prio to SCHED_OTHER for child processes and threads.
        ReturnVal = pthread_setschedparam(this_thread, SCHED_RR | SCHED_RESET_ON_FORK, &params);
    }
    else if (ThreadPriority == BATCH_PRIO) {
        params.sched_priority = 0;

        // Attempt to set thread priority to the SCHED_BATCH policy
        //SCHED_RESET_ON_FORK -> Reset prio to SCHED_OTHER for child processes and threads.
        ReturnVal = pthread_setschedparam(this_thread, SCHED_BATCH | SCHED_RESET_ON_FORK, &params);
    }
    else {
        params.sched_priority = 0;

        // Attempt to set thread priority to the SCHED_OTHER policy
        //SCHED_RESET_ON_FORK -> Reset prio to SCHED_OTHER for child processes and threads.
        ReturnVal = pthread_setschedparam(this_thread, SCHED_OTHER | SCHED_RESET_ON_FORK, &params);
    }
    if (ReturnVal != 0) {
        // Print the error
        std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
    }

}
void UpdateRebootFile(const QMap<QString, QString> RebootFileContent)
{
    const QString RebootPath = Global::SystemPaths::Instance().GetSettingsPath() + "/BootConfig.txt";
    QFile RebootFile(RebootPath);
    if(!RebootFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
        //!< todo raise event.
        qDebug()<<"UpdateRebootFile: Boot file open failed";
    }

    WriteBootVariable(RebootFileContent, RebootFile);
}

void WriteBootVariable(const QMap<QString, QString> content, QFile &file)
{
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug()<<"WriteBootVariable: Boot file open failed";
    }

    QTextStream fstream(&file);
    fstream.setFieldAlignment(QTextStream::AlignLeft);
    QMapIterator<QString, QString> iter(content);
    while (iter.hasNext()) {
        iter.next();
        QString key = iter.key();
        QString val = content.value(key);
        fstream << key << ":" << val << "\n" << left;
    }
    file.flush();
    fsync(file.handle());
    file.close();
}

bool Workaroundchecking(const QString& Type)
{
    if (false == QFile::exists("TEST_BEAN"))
    {
        return false;
    }
    QFile file("TEST_BEAN");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        QStringList list = line.split("=");
        if (list.size() != 2)
        {
            continue;
        }
        QString leftStr = static_cast<QString>(list.at(0));
        leftStr = leftStr.trimmed();
        if (Type == leftStr)
        {
            QString rightStr = static_cast<QString>(list.at(1));
            rightStr = rightStr.trimmed();
            if ("1" == rightStr)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    file.close();
    return false;
}

/****************************************************************************/
void CreateSymbolicLinkToFonts()
{
    QDir FontsDirectory(Global::SystemPaths::Instance().GetFontsPath());
    if (Global::SystemPaths::Instance().GetFontsPath().length() > 0 && FontsDirectory.exists()) {
        QStringList EnvironmentVarList = QProcess::systemEnvironment();
        /// QT_QWS_FONTDIR is set in target system not on the desktop.
        /// QT_QWS_FONTDIR variable stores the font folder location.
        /// so create sysmbolic links in the system fonts folder.
        foreach (QString EnvVariable, EnvironmentVarList) {
            if (EnvVariable.contains(QT_QWS_FONTDIR_STRING)) {
                QString SystemFontPath = EnvVariable.replace(QT_QWS_FONTDIR_STRING + "=", "");
                /// create symbolic links which are there in Fonts directory.
                foreach (QString FileName, FontsDirectory.entryList(QStringList() << "*.*")) {
                    /// creates link for the file name and if link already available then this function returns false
                    (void)QFile::link(FontsDirectory.absolutePath() + QDir::separator() + FileName, SystemFontPath + QDir::separator() + FileName);
                }
                /// Exit from the loop
                break;
            }
        }
    }
}

} // end namespace Global
