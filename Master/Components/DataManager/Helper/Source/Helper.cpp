/****************************************************************************/
/*! \file Master/Components/DataManager/Helper/Source/Helper.cpp
 *
 *  \brief Implementation file for general purpose functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth
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

#include <QStringList>
#include <QDebug>
#include <QXmlStreamWriter>
#include <math.h>
#include "Global/Include/GlobalDefines.h"
#include "Global/Include/UITranslator.h"
#include "DataManager/Helper/Include/Helper.h"
//#include <ColoradoDataContainer/Helper/Include/ColoradoDataManagerEventCodes.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/EventObject.h>
#include <qdatetime.h>

namespace DataManager {


/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
Helper::Helper()
{
}

/****************************************************************************/
/**
 * \brief Convert seconds to time string.
 *
 * The Leica configuration files use a specific format for time durations,
 * e.g. 3d 4h 12m 45s.
 * Internally seconds are used to define time durations.
 * This method converts a given number of seconds into an string value
 * containing the corresponding formated string for this time duration.
 *
 * \param[in]   TimeDurationInSeconds       Time duration given in seconds.
 *
 * \return  QString     Leica specific format for time durations.
 */
/****************************************************************************/
QString Helper::ConvertSecondsToTimeString(int TimeDurationInSeconds)
{
    const float SecondsPerDay = 60*60*24.0;
    const float SecondsPerHour = 60*60.0;
    const float SecondsPerMinute = 60.0;
    QString Result = "";
    QString StringValue = "";

    // handle zero value directly
    if (TimeDurationInSeconds == 0) {
        return "0s";
    }

    // days
    int ValueD = (int)trunc(TimeDurationInSeconds / SecondsPerDay);
    if (ValueD > 0) {
        TimeDurationInSeconds -=(int) (ValueD * SecondsPerDay);
        (void)StringValue.setNum(ValueD); //to suppress lint-534
        Result.append(StringValue);
        Result.append("d");
    }

    // hours
    int ValueH = (int)trunc(TimeDurationInSeconds / SecondsPerHour);
    if (ValueH > 0) {
        TimeDurationInSeconds -= (int) (ValueH * SecondsPerHour);
        if (Result.size() > 0) Result.append(" ");
        (void)StringValue.setNum(ValueH); //to suppress lint-534
        Result.append(StringValue);
        Result.append("h");
    }

    // minutes
    int ValueM = (int)trunc(TimeDurationInSeconds / SecondsPerMinute);
    if (ValueM > 0) {
        TimeDurationInSeconds -= (int) (ValueM * SecondsPerMinute);
        if (Result.size() > 0) Result.append(" ");
        (void)StringValue.setNum(ValueM); //to suppress lint-534
        Result.append(StringValue);
        Result.append("m");
    }

    // seconds
    int ValueS = TimeDurationInSeconds;
    if (ValueS > 0) {
        if (Result.size() > 0) Result.append(" ");
        (void)StringValue.setNum(ValueS); //to suppress lint-534
        Result.append(StringValue);
        Result.append("s");
    }

    Result = Result.trimmed();

    return Result;
}

/****************************************************************************/
/**
 * \brief Convert time string to seconds.
 *
 * The Leica configuration files use a specific format for time durations,
 * e.g. 3d 4h 12m 45s.
 * Internally seconds are used to define time durations.
 * This method converts a given string into an integer value containing
 * the corresponding number of seconds.
 *
 * \param[in]   TimeDuration       Time duration given in seconds.
 *
 * \return  int     Time duration in seconds.
 */
/****************************************************************************/
// for time strings like "4d 1h 30m 15s"
int Helper::ConvertTimeStringToSeconds(QString TimeDuration)
{
    const float SecondsPerDay = 60*60*24.0;
    const float SecondsPerHour = 60*60.0;
    const float SecondsPerMinute = 60.0;

    // handle empty string directly
    if (TimeDuration == "") {
        return 0;
    }

    QStringList SplitList = TimeDuration.split(" ");
    int Result = 0;
    int Value = 0;
    bool ok = true;
    foreach(QString SplitString, SplitList)
    {
        if (SplitString.endsWith("d")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&ok);
            if(!ok) qDebug() << "##### CProgramStep::ConvertTimeStringToSeconds failed ";
            Result += (int) (Value * SecondsPerDay);
        } else if (SplitString.endsWith("h")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&ok);
            if(!ok) qDebug() << "##### CProgramStep::ConvertTimeStringToSeconds failed ";
            Result += (int) (Value * SecondsPerHour);
        } else if (SplitString.endsWith("m")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&ok);
            if(!ok) qDebug() << "##### CProgramStep::ConvertTimeStringToSeconds failed ";
            Result += (int) (Value * SecondsPerMinute);
        } else if (SplitString.endsWith("s")) {
            SplitString.chop(1);
            Value = SplitString.toInt(&ok);
            if(!ok) qDebug() << "##### CProgramStep::ConvertTimeStringToSeconds failed ";
            Result += Value;
        }
    }

    return Result;
}

/****************************************************************************/
/**
 * \brief convert QString of form YYYY-MM-DD(2011-11-01) to QDate format
 *
 * The Leica configuration files use a specific format for date,
 * e.g. 2011-11-01.
 * This method converts a given string into a QDate format
 *
 * \param[in]   Date       date string.
 *
 * \return  QDate     QDate format.
 */
/****************************************************************************/
QDate Helper::ConvertDateStringToQDate(QString Date)
{
    //    qDebug() << "Entering ConvertDateStringToQDate";
    // handle empty string directly
    if ((Date == "") || (Date == "UNDEFINED")) {
        QDate Result;
        return Result;
    }
    QString Temp1;
    QString Temp2;
    QString Temp3;
    int Year;
    int Month;
    int Day;
    bool Ok = true;

    QStringList SplitList = Date.split("-");
    //    qDebug() << "Date is" << Date;
    //    qDebug() << "Came till here -1";
    QStringList::Iterator it = SplitList.begin();
    //    qDebug() << "Came till here -2";
    Temp1 = *it;
    it++;
    //    qDebug() << "Came till here -3";
    Temp2 = *it;
    it++;
    Temp3 = *it;
    //    qDebug() << "Came till here -4";
    //    qDebug() << "Temp1 is" << Temp1;
    //    qDebug() << "Temp2 is" << Temp2;
    //    qDebug() << "Temp3 is" << Temp3;
    Year = Temp1.toInt(&Ok);
    //    qDebug() << "Came till here -5";
    Month = Temp2.toInt(&Ok);
    Day = Temp3.toInt(&Ok);
    //    qDebug() << "Date is" << Year << "-" << Month << "-" << Day;
    QDate Result(Year, Month, Day);
    //    qDebug() << "Result is" << Result;
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Converts QString of form YYYY-MM-DDTHH:MM:SS to QDateTime format
 *
 *  \iparam DateTime = Date Time String
 *
 *  \return QDateTime
 */
/****************************************************************************/
// converting QString of form 2011-11-01 14:54:43 to QDateTime format
QDateTime Helper::ConvertDateTimeStringToQDateTime(QString DateTime)
{
    // handle empty string directly
    if ((DateTime == "") || (DateTime == "UNDEFINED")) {
        QDateTime Result;
        return Result;
    }
    QDate Date;
    QString TempDate;
    QString TempTime = "";
    QString Temp4;
    QString Temp5;
    QString Temp6 = "";
    int second = 0;
    int minute = 0;
    int hour = 0;
    bool Ok = true;

    QStringList SplitList = DateTime.split("T"); // this separator according to ISO8601
    QStringList::Iterator it = SplitList.begin();
    TempDate = *it;
    // checking for Time format this has to be present
    if (SplitList.count() > 1) {
        it++;
        TempTime = *it;
    }
    Date = ConvertDateStringToQDate(TempDate);

    if (!TempTime.isEmpty()) {
        SplitList = TempTime.split(":");
        it = SplitList.begin();
        Temp4 = *it;
        it++;
        Temp5 = *it;
        // Seconds could be optional not necessary to be provided
        if(SplitList.count() > 2) {
            it++;
            Temp6 = *it;
        }
        hour = Temp4.toInt(&Ok);
        minute = Temp5.toInt(&Ok);
        second = Temp6.toInt(&Ok);
    }
    QTime Time(hour, minute, second);
    QDateTime Result(Date, Time, Qt::UTC);   
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Searches for the node and returns true if found
 *
 *  \iparam XmlStreamReader = Instance of the QXmlStreamReader
 *  \iparam NodeName = Name of the node in the Xml file
 *
 *  \return True or False
 */
/****************************************************************************/
bool Helper::ReadNode(const QXmlStreamReader& XmlStreamReader, QString NodeName)
{
    // look for node <NodeName>
    if (XmlStreamReader.atEnd())
    {
        qDebug() << "Helper::ReadNode, nothing to read since we are at the end already" << NodeName;
    }

    bool Result = false;
    while (!XmlStreamReader.atEnd())
    {
        if (const_cast<QXmlStreamReader &>(XmlStreamReader).readNext() == QXmlStreamReader::Invalid) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }
//        qDebug() << "Helper::ReadNode" << XmlStreamReader.name();
        if (XmlStreamReader.isStartElement())
        {
            if (XmlStreamReader.name() == NodeName) {
                Result = true;
                break;
            }
            else {
                // Assert has to be called here
                Q_ASSERT(Result == false);
            }
        }
    }
    if (!Result) {
        qDebug() << "Helper::ReadNode failed. Node not found: " << NodeName;
    }
    return Result;
}


/****************************************************************************/
/*!
 *  \brief Generate Error string for the list of errors passed.
 *         The erros would be formatted one below the other.
 *  \iparam ErrorList = Reference to the list of errors
 *  \iparam ErrorString = Generated Error string
 */
/****************************************************************************/
void Helper::ErrorIDToString(ListOfErrors_t &ErrorList, QString &ErrorString)
{
    while(!ErrorList.isEmpty()) {
        qint32 ErrorCount = 0;
        ErrorMap_t *p_ErrorMap = ErrorList.first();
        QMapIterator<quint32, Global::tTranslatableStringList> It(*p_ErrorMap);
        bool MaxErrors = false;
        while(It.hasNext()) {
            //this is Java style iterator , hence points inbetween items in list!
            It.next();
            if (Q_LIKELY(ErrorCount < 5)) {
                ErrorString += Global::UITranslator::TranslatorInstance().Translate(Global::TranslatableString(It.key(), It.value()), true);
            }
            else {
                // When error count is more than 5 , we append a string in the end requesting user to check user log for
                // more info
                ErrorString += Global::UITranslator::TranslatorInstance().Translate(Global::TranslatableString(EVENT_DM_CHECK_USER_LOG_FOR_MORE_INFO, Global::tTranslatableStringList()), true);
                MaxErrors = true;
                break;
            }
            ErrorString += "\n";
            ErrorCount++;
        }
        // clear the hash
        p_ErrorMap->clear();
        //! \warning dont delte below statement, else will lead to forever loop !
        ErrorList.removeFirst();
        if (MaxErrors) {
            break;
        }
    }
    //Clear the error list
    ErrorList.clear();
}

}  // namespace DataManager
