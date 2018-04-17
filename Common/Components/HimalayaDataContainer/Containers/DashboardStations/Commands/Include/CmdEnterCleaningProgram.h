/****************************************************************************/
/*! \file CmdEnterCleaningProgram.h
 *
 *  \brief CmdEnterCleaningProgram command definition.
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

#ifndef MSGCLASSES_CMDENTERCLEANINGPROGRAM_H
#define MSGCLASSES_CMDENTERCLEANINGPROGRAM_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdEnterCleaningProgram command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdEnterCleaningProgram : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdEnterCleaningProgram &);
    friend QDataStream & operator >> (QDataStream &, CmdEnterCleaningProgram &);
public:
    CmdEnterCleaningProgram();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdEnterCleaningProgram
     *
     *  \param Timeout = int type parameter
     *  \param lastReagentGroupID =  last reagent group ID
     *
     *  \return from CmdEnterCleaningProgram
     */
    /****************************************************************************/
    CmdEnterCleaningProgram(const QString& RetortID, int Timeout, const QString& lastReagentGroupID);
    ~CmdEnterCleaningProgram();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function LastReagentGroupID
     *
     *  \return from Last reagent group ID
     */
    /****************************************************************************/
    inline QString LastReagentGroupID() const {return m_LastReagentGroupID;}

    const QString& GetRetortID() const {return m_RetortID;}
 private:
    CmdEnterCleaningProgram(const CmdEnterCleaningProgram &);                     ///< Not implemented.
    const CmdEnterCleaningProgram & operator = (const CmdEnterCleaningProgram &); ///< Not implemented.
private:
    QString m_LastReagentGroupID;       ///<  Definition/Declaration of variable m_LastReagentGroupID
    QString m_RetortID;
    
}; // end class CmdEnterCleaningProgram

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdEnterCleaningProgram &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_LastReagentGroupID;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdEnterCleaningProgram &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_LastReagentGroupID;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDENTERCLEANINGPROGRAM_H
