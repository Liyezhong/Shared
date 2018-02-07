/****************************************************************************/
/*! \file CmdRecoveryFromPowerFailure.h
 *
 *  \brief CmdRecoveryFromPowerFailure command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.09.2014
 *   $Author:  $ Logan Yang
 *
 *  \b Company:
 *
  *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef MSGCLASSES_CMDRECOVERYFROMPOWERFAILURE_H
#define MSGCLASSES_CMDRECOVERYFROMPOWERFAILURE_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>
#include <QTime>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRecoveryFromPowerFailure command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRecoveryFromPowerFailure : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRecoveryFromPowerFailure &);
    friend QDataStream & operator >> (QDataStream &, CmdRecoveryFromPowerFailure &);
public:
    CmdRecoveryFromPowerFailure();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdRecoveryFromPowerFailure
     *
     *  \param timeout = int type parameter
     *  \param ProgramID =  program id
     *  \param StepIndex =  step index
     *  \param Scenario  =  Scenario
     *  \param RemainingTime = remain time
     *  \param ReagentGroupID    =   color id
     *  \param StationList = station list
     *  \param IsErrorHandlingFailed = ErrorHandlingFailed flag
     *  \param StationID = station id
     *
     *  \return from CmdCurrentProgramStepInfor
     */
    /****************************************************************************/
    CmdRecoveryFromPowerFailure(int timeout, const QString& ProgramID, int StepIndex,
                                int Scenario, int RemainingTime, const QString& ReagentGroupID,
                                const QList<QString>& StationList, bool IsErrorHandlingFailed,
                                const QString& StationID);
    ~CmdRecoveryFromPowerFailure();
    virtual QString GetName() const;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ProgramID
     *
     *  \return from ProgramID
     */
    /****************************************************************************/
    inline QString GetProgramID() const {return m_ProgramID;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StepIndex
     *
     *  \return from StepIndex
     */
    /****************************************************************************/
    inline int GetStepIndex() const {return m_StepIndex;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CurRemainingTime
     *
     *  \return from CurRemainingTime
     */
    /****************************************************************************/
    inline int GetRemainingTime()const {return m_RemainingTime;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CurProgramStepIndex
     *
     *  \return from CurProgramStepIndex
     */
    /****************************************************************************/
    inline int GetScenario() const {return m_Scenario;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetLastReagentGroupID
     *
     *  \return QString of LastReagentGroupID
     */
    /****************************************************************************/
    inline QString GetLastReagentGroupID() const {return m_LastReagentGroupID;}

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetLastReagentGroupID
     *
     *  \return QString of LastReagentGroupID
     */
    /****************************************************************************/
    inline const QList<QString>& GetStationList() const {return m_StationList;}
    /****************************************************************************/
    /*!
     *  \brief  Definition of function IsErrorHandlingFailed
     *
     *  \return the status of ErrorHandlingFailed
     */
    /****************************************************************************/
    inline bool IsErrorHandlingFailed() const {return m_IsErrorHandlingFailed;}
    /****************************************************************************/
    /*!
     *  \brief  Definition of function GetStationID
     *
     *  \return station Id
     */
    /****************************************************************************/
    inline QString GetStationID() const {return m_StationID;}


private:
    CmdRecoveryFromPowerFailure(const CmdRecoveryFromPowerFailure &);                     ///< Not implemented.
    const CmdRecoveryFromPowerFailure & operator = (const CmdRecoveryFromPowerFailure &); ///< Not implemented.
private:
    QString m_ProgramID;       ///<  Definition/Declaration of variable m_ProgramID
    int m_StepIndex;       ///<  Definition/Declaration of variable m_StepIndex
    int m_Scenario;            ///<  Definition/Declaration of variable m_Scenario
    int m_RemainingTime;       ///<  Definition/Declaration of variable m_RemainingTime
    QString m_LastReagentGroupID; ///< the last reagent groupid if normal program.
    QList<QString> m_StationList; ///< station id list for the program!
    bool m_IsErrorHandlingFailed; ///<ErrorHandling is Failed or not?
    QString m_StationID; ///< the current station id
    
}; // end class CmdCurrentProgramStepInfor

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRecoveryFromPowerFailure &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ProgramID;
    Stream << Cmd.m_StepIndex;
    Stream << Cmd.m_Scenario;
    Stream << Cmd.m_RemainingTime;
    Stream << Cmd.m_LastReagentGroupID;
    Stream << Cmd.m_StationList;
    Stream << Cmd.m_IsErrorHandlingFailed;
    Stream << Cmd.m_StationID;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRecoveryFromPowerFailure &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ProgramID;
    Stream >> Cmd.m_StepIndex;
    Stream >> Cmd.m_Scenario;
    Stream >> Cmd.m_RemainingTime;
    Stream >> Cmd.m_LastReagentGroupID;
    Stream >> Cmd.m_StationList;
    Stream >> Cmd.m_IsErrorHandlingFailed;
    Stream >> Cmd.m_StationID;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDCURRENTPROGRAMSTEPINFOR_H
