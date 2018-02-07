/****************************************************************************/
/*! \file CmdProgramAborted.h
 *
 *  \brief CmdProgramAborted command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.04.2013
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

#ifndef MSGCLASSES_CMDPROGRAMABORTED_H
#define MSGCLASSES_CMDPROGRAMABORTED_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProgramAborted command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProgramAborted : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdProgramAborted &);
    friend QDataStream & operator >> (QDataStream &, CmdProgramAborted &);
public:
    CmdProgramAborted();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdProgramAborted
     *
     *  \param Timeout = int type parameter
     *  \param IsContaminated =  Retort is Contaminated?
     *
     *  \return from CmdProgramAborted
     */
    /****************************************************************************/
    CmdProgramAborted(int Timeout, bool IsContaminated);
    ~CmdProgramAborted();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsRetortContaminated
     *
     *  \return from AcknownedgeType
     */
    /****************************************************************************/
    bool IsRetortContaminated() const {return m_IsRetortContaminated;}
 private:
    CmdProgramAborted(const CmdProgramAborted &);                     ///< Not implemented.
    const CmdProgramAborted & operator = (const CmdProgramAborted &); ///< Not implemented.
private:
    bool m_IsRetortContaminated;       ///<  Definition/Declaration of variable m_IsRetortContaminated
    
}; // end class CmdProgramAborted

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdProgramAborted &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_IsRetortContaminated;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProgramAborted &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_IsRetortContaminated;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDPROGRAMABORTED_H
