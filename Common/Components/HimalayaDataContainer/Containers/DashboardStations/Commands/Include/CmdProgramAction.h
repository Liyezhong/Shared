/****************************************************************************/
/*! \file CmdProgramAction.h
 *
 *  \brief CmdProgramAction command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 22.03.2013
 *   $Author:  $ Abe Yang
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

#ifndef MSGCLASSES_CMDPROGRAMACTION_H
#define MSGCLASSES_CMDPROGRAMACTION_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>
#include <QDateTime>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProgramAction command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProgramAction : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdProgramAction &);
    friend QDataStream & operator >> (QDataStream &, CmdProgramAction &);
public:
	/**
	* 	\brief constructor
	*/
    CmdProgramAction();   ///< Not implemented.

    static QString NAME;  ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdProgramAction
     *
     *  \param timeout = int type parameter
     *  \param retortId = int type parameter
     *  \param programID =  const QString type parameter
     *  \param actionType =  DataManager::ProgramActionType_t type parameter
     *  \param delayTime =  delay time
     *  \param runDuration = the time duration from "start" to end datetime
     *  \param reagentExpiredFlag = reagentExpiredFlag for log file
     *  \return from CmdProgramAction
     */
    /****************************************************************************/
    CmdProgramAction(int retortId, int timeout, const QString& programID, DataManager::ProgramActionType_t actionType,
    int delayTime, int runDuration, const QString& reagentExpiredFlag);
    /*!
	* \brief destructor
	*/
    ~CmdProgramAction();
	/**
	* \brief get name
	* \return name
	*/
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetProgramID
     *
     *  \return from GetProgramID
     */
    /****************************************************************************/
    inline const QString& GetProgramID()const {return m_ProgramID;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ProgramActionType
     *
     *  \return from ProgramActionType
     */
    /****************************************************************************/
    inline DataManager::ProgramActionType_t ProgramActionType() const {return m_ActionType;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function DelayTime
     *
     *  \return from DelayTime
     */
    /****************************************************************************/
    inline int DelayTime()const {return m_DelayTime;}
    /****************************************************************************/
    /*!
     *  \brief  Definition of function ProgramRunDuration
     *
     *  \return from ProgramRunDuration
     */
    /****************************************************************************/
    inline int ProgramRunDuration()const {return m_ProgramRunDuration;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetReagentExpiredFlag
     *
     *  \return from GetReagentExpiredFlag
     */
    /****************************************************************************/
    inline const QString& GetReagentExpiredFlag()const {return m_ReagentExpiredFlag;}    

    inline int GetRetortId() const {return m_RetortId;}

virtual QString ToString()const
{
    return QString("RetortId{%1}ProgramID{%2}ActionType{%3}DelayTime{%4}ProgramRunDuration{%5}ReangentExpiredFlag{%6}").arg(m_RetortId).arg(m_ProgramID).arg(m_ActionType).arg(m_DelayTime).arg(m_ProgramRunDuration).arg(m_ReagentExpiredFlag);
}

private:
	/**
	* \brief constructor
	*/
    CmdProgramAction(const CmdProgramAction &);                     ///< Not implemented.
	/**
	* \brief operator = 
	* \iparam instance
	*/
    const CmdProgramAction & operator = (const CmdProgramAction &); ///< Not implemented.
private:    
    int m_RetortId;
    QString      m_ProgramID;       ///<  Definition/Declaration of variable m_ProgramID
    DataManager::ProgramActionType_t m_ActionType;       ///<  Definition/Declaration of variable m_ActionType
    int m_DelayTime;       ///<  Definition/Declaration of variable m_DelayTime
    int m_ProgramRunDuration;   ///<  Definition/Declaration of variable m_DelayTime
    QString      m_ReagentExpiredFlag;
}; // end class CmdProgramAction

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdProgramAction &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RetortId;
    Stream << Cmd.m_ProgramID;
    Stream << (int)Cmd.m_ActionType;
    Stream << Cmd.m_DelayTime;
    Stream << Cmd.m_ProgramRunDuration;
    Stream << Cmd.m_ReagentExpiredFlag;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProgramAction &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RetortId;
    Stream >> Cmd.m_ProgramID;
    int temp;
    Stream >> temp;
    Cmd.m_ActionType = (DataManager::ProgramActionType_t)temp;
    Stream >> Cmd.m_DelayTime;
    Stream >> Cmd.m_ProgramRunDuration;
    Stream >> Cmd.m_ReagentExpiredFlag;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDPROGRAMACTION_H
