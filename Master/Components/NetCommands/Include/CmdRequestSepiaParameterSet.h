/****************************************************************************/
/*! \file CmdRequestSepiaParameterSet.h
 *
 *  \brief CmdRequestSepiaParameterSet command definition.
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

#ifndef CMDREQUESTSEPIAPARAMETERSET_H
#define CMDREQUESTSEPIAPARAMETERSET_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdGenerateBathLayout command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRequestSepiaParameterSet : public Global::Command {
public:
    static QString NAME;    ///< Command name.

    friend QDataStream & operator << (QDataStream &, const CmdRequestSepiaParameterSet &);
    friend QDataStream & operator >> (QDataStream &, CmdRequestSepiaParameterSet &);
    /****************************************************************************/
    CmdRequestSepiaParameterSet(int Timeout, quint32 UniqueRFId);
    ~CmdRequestSepiaParameterSet();
    virtual QString GetName() const;

    CmdRequestSepiaParameterSet();
    quint32 GetRackRFId() const { return m_UniqueRFId;}

private:
    CmdRequestSepiaParameterSet(const CmdRequestSepiaParameterSet &);                       ///< Not implemented.
    const CmdRequestSepiaParameterSet & operator = (const CmdRequestSepiaParameterSet &);   ///< Not implemented.

private:
    quint32 m_UniqueRFId;
}; // end class CmdRequestSepiaParameterSet

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRequestSepiaParameterSet &Cmd)
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
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRequestSepiaParameterSet &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_UniqueRFId;
    return Stream;
}

} // end namespace NetCommands

#endif // CMDREQUESTSEPIAPARAMETERSET_H
