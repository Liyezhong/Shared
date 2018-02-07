/****************************************************************************/
/*! \file CmdKeepCassetteCount.h
 *
 *  \brief CmdKeepCassetteCount command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.04.2013
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

#ifndef MSGCLASSES_CMDKEEPCASSETTECOUNT_H
#define MSGCLASSES_CMDKEEPCASSETTECOUNT_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdKeepCassetteCount command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdKeepCassetteCount : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdKeepCassetteCount &);
    friend QDataStream & operator >> (QDataStream &, CmdKeepCassetteCount &);
public:
    CmdKeepCassetteCount();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdKeepCassetteCount
     *
     *  \param Timeout = int type parameter
     *  \param CassetteCount =  int type parameter
     *
     *  \return from CmdKeepCassetteCount
     */
    /****************************************************************************/
    CmdKeepCassetteCount(int Timeout, int CassetteCount);
    ~CmdKeepCassetteCount();
    virtual QString GetName() const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CassetteCount
     *
     *  \return from CassetteCount
     */
    /****************************************************************************/
    inline int CassetteCount()const {return m_CassetteCount;}


private:
    CmdKeepCassetteCount(const CmdKeepCassetteCount &);                     ///< Not implemented.
    const CmdKeepCassetteCount & operator = (const CmdKeepCassetteCount &); ///< Not implemented.
private:
    int      m_CassetteCount;       ///<  Definition/Declaration of variable m_CassetteCount
}; // end class CmdKeepCassetteCount

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdKeepCassetteCount &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_CassetteCount;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdKeepCassetteCount &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_CassetteCount;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDKEEPCASSETTECOUNT_H
