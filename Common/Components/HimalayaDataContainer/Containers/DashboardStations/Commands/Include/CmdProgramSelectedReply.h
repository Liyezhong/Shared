/****************************************************************************/
/*! \file CmdProgramSelectedReply.h
 *
 *  \brief CmdProgramSelectedReply command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27.03.2013
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

#ifndef MSGCLASSES_CMDPROGRAMSELECTEDREPLY_H
#define MSGCLASSES_CMDPROGRAMSELECTEDREPLY_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>


namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProgramSelectedReply command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProgramSelectedReply : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdProgramSelectedReply &);
    friend QDataStream & operator >> (QDataStream &, CmdProgramSelectedReply &);
public:
    CmdProgramSelectedReply();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdProgramSelectedReply
     *
     *  \param timeout
     *  \param timeProposed
     *  \param paraffinMeltCostedtime
     *  \param costedTimeBeforeParaffin
     *  \param whichStepHasNoSafeReagent
     *  \param SecondsMeltParaffin
     *  \param stationList
     *  \param firstProgramStepIndex
     *
     *  \return from CmdProgramSelectedReply
     */
    /****************************************************************************/
    CmdProgramSelectedReply(int timeout, int timeProposed,
                      int paraffinMeltCostedtime,
                      int costedTimeBeforeParaffin,
                      int whichStepHasNoSafeReagent,
                      quint32 SecondsMeltParaffin,
                      QList<QString>& stationList,
                      int firstProgramStepIndex);
    ~CmdProgramSelectedReply();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TimeProposed
     *
     *  \return from TimeProposed
     */
    /****************************************************************************/
    inline int TimeProposed()const {return m_TimeProposed;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ParaffinMeltCostedTime
     *
     *  \return from ParaffinMeltCostedTime
     */
    /****************************************************************************/
    inline int ParaffinMeltCostedTime()const {return m_ParaffinMeltCostedtime;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CostedTimeBeforeParaffin
     *
     *  \return from CostedTimeBeforeParaffin
     */
    /****************************************************************************/
    inline int CostedTimeBeforeParaffin()const {return m_CostedTimeBeforeParaffin;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function WhichStepHasNoSafeReagent
     *
     *  \return from WhichStepHasNoSafeReagent
     */
    /****************************************************************************/
    inline int WhichStepHasNoSafeReagent() const {return m_WhichStepHasNoSafeReagent;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StationList
     *
     *  \return from StationList
     */
    /****************************************************************************/
    inline const QList<QString>& StationList()const {return m_StationList;}

    /****************************************************************************/
    /*!
     *  \brief  Get sceconds for melting paraffin
     *
     *  \return seconds
     */
    /****************************************************************************/
    inline int GetSecondsForMeltingParaffin()const {return m_SecondsForMelting;}

    /****************************************************************************/
    /*!
     *  \brief  Get the first program step index
     *
     *  \return step index
     */
    /****************************************************************************/
    inline int GetFirstProgramStepIndex()const {return m_firstProgramStepIndex;}

    
private:
    CmdProgramSelectedReply(const CmdProgramSelectedReply &);                     ///< Not implemented.
    const CmdProgramSelectedReply & operator = (const CmdProgramSelectedReply &); ///< Not implemented.
private:
    int m_TimeProposed;///< the costed time for selected program, seconds
    int m_CostedTimeBeforeParaffin; ///< seconds
    int m_ParaffinMeltCostedtime; ///< seconds
    int m_WhichStepHasNoSafeReagent;       ///<  Definition/Declaration of variable m_WhichStepHasNoSafeReagent
    int m_SecondsForMelting;   ///< total seconds for melting paraffin.
    QList<QString> m_StationList;       ///<  Definition/Declaration of variable m_StationList
    int m_firstProgramStepIndex;        ///< for show the detail of CleaningPropram
}; // end class CmdProgramSelectedReply

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdProgramSelectedReply &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_TimeProposed;
    Stream << Cmd.m_ParaffinMeltCostedtime;
    Stream << Cmd.m_CostedTimeBeforeParaffin;
    Stream << Cmd.m_WhichStepHasNoSafeReagent;
    Stream << Cmd.m_SecondsForMelting;
    Stream << Cmd.m_StationList.count();
    for (int i = 0; i < Cmd.m_StationList.count(); i++)
    {
        Stream << Cmd.m_StationList.at(i);
    }
    Stream << Cmd.m_firstProgramStepIndex;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProgramSelectedReply &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_TimeProposed;
    Stream >> Cmd.m_ParaffinMeltCostedtime;
    Stream >> Cmd.m_CostedTimeBeforeParaffin;
    Stream >> Cmd.m_WhichStepHasNoSafeReagent;
    Stream >> Cmd.m_SecondsForMelting;
    int stationCount = 0;
    Stream >> stationCount;
    QString stationID("");
    for (int i = 0; i < stationCount; i++)
    {
        Stream >> stationID;
        Cmd.m_StationList.push_back(stationID);
    }
    Stream >> Cmd.m_firstProgramStepIndex;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDPROGRAMSELECTEDREPLY_H
