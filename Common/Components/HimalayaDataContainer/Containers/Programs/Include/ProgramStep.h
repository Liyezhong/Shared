/****************************************************************************/
/*! \file ProgramStep.h
 *
 *  \brief Definition file for class CProgramStep.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-08-01
 *  $Author:    $ F. Toth, N.Kamath
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

#ifndef DATAMANAGER_PROGRAMSTEP_H
#define DATAMANAGER_PROGRAMSTEP_H
#include <QStringList>
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"

namespace DataManager {
//!< Forward declartion
class CProgram;
/****************************************************************************/
/*!
 *  \brief  This class implements program step
 */
/****************************************************************************/
class CProgramStep
{
    friend class CProgram;

private:
    QString m_StepID;               //!< Step Id
    QString m_ReagentID;            //!< Reagent ID
    QString m_Duration;             //!< duration of the step
    QString m_Temperature;              //!< temperature of the step
    QString m_Pressure;             //!< pressure status: on or off
    QString m_Vacuum;               //!< vacuum status: on or off
    int m_NumberOfSteps;            //!< Number of Steps field only for Leica Programs
    QStringList m_StationIDList;    //!< Stations IDs list for scheduler

    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);

    /****************************************************************************/
    /*!
     *  \brief Sets Number OF steps for a Leica program
     *
     *  \iparam Value = Number Of Steps value
     */
    /****************************************************************************/
    void SetNumberOfSteps(const int Value){m_NumberOfSteps = Value;}

public:
    CProgramStep();
    CProgramStep(const QString StepID);
    CProgramStep(const QString StepID, const QString ReagentID, const QString Duration, const QString Temperature,
                 const QString Pressure, const QString Vacuum);
    CProgramStep(const CProgramStep&);
    friend QDataStream& operator << (QDataStream& OutDataStream, const CProgramStep& ProgramStep);
    friend QDataStream& operator >> (QDataStream& InDataStream, CProgramStep& ProgramStep);
    CProgramStep& operator = (const CProgramStep&);

    /****************************************************************************/
    /*!
     *  \brief Retrieves Step ID of the program step
     *
     *  \return Step ID
     */
    /****************************************************************************/
    QString  GetStepID() const {return m_StepID;}
    /****************************************************************************/
    /*!
     *  \brief Sets Step ID
     *
     *  \iparam Value = Step ID
     */
    /****************************************************************************/
    void SetStepID(const QString Value){m_StepID = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves reagent id of the reagent used for the program step
     *
     *  \return Reagent ID
     */
    /****************************************************************************/
    QString  GetReagentID() const {return m_ReagentID;}
    /****************************************************************************/
    /*!
     *  \brief Sets Reagent ID
     *
     *  \iparam Value = Reagent ID
     */
    /****************************************************************************/
    void SetReagentID(const QString Value){m_ReagentID = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves duration of the step
     *
     *  \return duration string
     */
    /****************************************************************************/
    QString GetDuration() const {return m_Duration;}

    /****************************************************************************/
    /*!
     *  \brief Sets  duration of the program step
     *
     *  \iparam Value =  duration value
     */
    /****************************************************************************/
    void SetDuration(const QString Value){m_Duration = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves duration of the step in seconds
     *
     *  \return duration string
     */
    /****************************************************************************/
    int GetDurationInSeconds() const {return Helper::ConvertTimeStringToSeconds(m_Duration);}

    /****************************************************************************/
    /*!
     *  \brief Sets  duration of the program step in seconds
     *
     *  \iparam Value =  duration value
     */
    /****************************************************************************/
    void SetDurationInSeconds(const int Value){m_Duration = Helper::ConvertSecondsToTimeString(Value);}

    /****************************************************************************/
    /*!
     *  \brief Retrieves Temperature of the step
     *
     *  \return Temperature
     */
    /****************************************************************************/
    QString GetTemperature() const {return m_Temperature;}

    /****************************************************************************/
    /*!
     *  \brief Sets  Temperature of the program step
     *
     *  \iparam Value =  Temperature
     */
    /****************************************************************************/
    void SetTemperature(const QString Value){m_Temperature = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves Pressure of the step
     *
     *  \return Pressure string
     */
    /****************************************************************************/
    QString GetPressure() const {return m_Pressure;}

    /****************************************************************************/
    /*!
     *  \brief Sets  Pressure of the program step
     *
     *  \iparam Value =  Pressure value
     */
    /****************************************************************************/
    void SetPressure(const QString Value){m_Pressure = Value;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves Vacuum of the step
     *
     *  \return Vacuum string
     */
    /****************************************************************************/
    QString GetVacuum() const {return m_Vacuum;}

    /****************************************************************************/
    /*!
     *  \brief Sets  Vacuum of the program step
     *
     *  \iparam Value =  Vacuum value
     */
    /****************************************************************************/
    void SetVacuum(const QString Value){m_Vacuum = Value;}


    /****************************************************************************/
    /*!
        *  \brief Retrieves Number of Steps of the reagent used for the Leica Program
        *
        *  \return Number of Steps
        */
    /****************************************************************************/
    int GetNumberOfSteps() const {return m_NumberOfSteps;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves the station id ist
     *
     *  \return Station Id list
     */
    /****************************************************************************/
    QStringList GetStationIDList() const { return m_StationIDList; }
    /****************************************************************************/
    /*!
     *  \brief Sets the list of station ID's
     *
     *  \iparam StationIDList = List of Station IDs
     */
    /****************************************************************************/
    void SetStationIDList(const QStringList StationIDList) {
        m_StationIDList = StationIDList;
    }
};

} // namespace DataManager

#endif // DATAMANAGER_PROGRAMSTEP_H
