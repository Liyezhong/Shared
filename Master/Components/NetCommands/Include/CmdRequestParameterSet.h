/****************************************************************************/
/*! \file CmdRequestParameterSet.h
 *
 *  \brief CmdRequestParameterSet command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-02
 *   $Author:  $ C.Adaragunchi
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

#ifndef CMDREQUESTPARAMETERSET_H
#define CMDREQUESTPARAMETERSET_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdGenerateBathLayout command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRequestParameterSet : public Global::Command {
public:
    static QString NAME;    ///< Command name.

    friend QDataStream & operator << (QDataStream &, const CmdRequestParameterSet &);
    friend QDataStream & operator >> (QDataStream &, CmdRequestParameterSet &);
    /****************************************************************************/
    CmdRequestParameterSet(int Timeout, quint32 UniqueRFId);
    ~CmdRequestParameterSet();
    virtual QString GetName() const;

    CmdRequestParameterSet();
    /****************************************************************************/
    /*!
     *  \brief   Function to get UniqueRFId of Rack.
     *
     * \return   UniqueRfId of Rack.
     */
    /****************************************************************************/
    quint32 GetRackRFId() const { return m_UniqueRFId;}

private:
    CmdRequestParameterSet(const CmdRequestParameterSet &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRequestParameterSet & operator = (const CmdRequestParameterSet &);   ///< Not implemented.

private:
    quint32 m_UniqueRFId; ///< Unique RFId of rack.
}; // end class CmdRequestParameterSet

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRequestParameterSet &Cmd)
{
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_UniqueRFId;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRequestParameterSet &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_UniqueRFId;
    return Stream;
}

} // end namespace NetCommands

#endif // CMDREQUESTPARAMETERSET_H
