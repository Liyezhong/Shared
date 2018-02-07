/****************************************************************************/
/*! \file CmdUpdateProgramEndTime.h
 *
 *  \brief CmdUpdateProgramEndTime command definition.
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

#ifndef MSGCLASSES_CMDUPDATEPROGRAMENDTIME_H
#define MSGCLASSES_CMDUPDATEPROGRAMENDTIME_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>
#include <QTime>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdUpdateProgramEndTime command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdUpdateProgramEndTime : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdUpdateProgramEndTime &);
    friend QDataStream & operator >> (QDataStream &, CmdUpdateProgramEndTime &);
public:
    CmdUpdateProgramEndTime();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdUpdateProgramEndTime
     *
     *  \param timeout = int type parameter
     *  \param endTimeDiff =  time difference value
     *
     *  \return from CmdUpdateProgramEndTime
     */
    /****************************************************************************/
    CmdUpdateProgramEndTime(int timeout, int endTimeDiff);
    ~CmdUpdateProgramEndTime();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CurProgramStepIndex
     *
     *  \return from CurProgramStepIndex
     */
    /****************************************************************************/
    inline int GetEndTimeDiff() const {return m_EndTimeDiff;}
private:
    CmdUpdateProgramEndTime(const CmdUpdateProgramEndTime &);                     ///< Not implemented.
    const CmdUpdateProgramEndTime & operator = (const CmdUpdateProgramEndTime &); ///< Not implemented.
private:
    int   m_EndTimeDiff;       ///<  Definition/Declaration of variable m_EndTimeDiff
    
}; // end class CmdUpdateProgramEndTime

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdUpdateProgramEndTime &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_EndTimeDiff;

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
inline QDataStream & operator >> (QDataStream &Stream, CmdUpdateProgramEndTime &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_EndTimeDiff;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDUPDATEPROGRAMENDTIME_H
