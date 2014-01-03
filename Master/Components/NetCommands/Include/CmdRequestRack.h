/****************************************************************************/
/*! \file CmdRequestRack.h
 *
 *  \brief CmdRequestRack command definition.
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

#ifndef CMDREQUESTRACK_H
#define CMDREQUESTRACK_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRequestRack command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRequestRack : public Global::Command {
public:
    static QString NAME;    ///< Command name.

    friend QDataStream & operator << (QDataStream &, const CmdRequestRack &);
    friend QDataStream & operator >> (QDataStream &, CmdRequestRack &);
    /****************************************************************************/
    CmdRequestRack(int Timeout);
    ~CmdRequestRack();
    virtual QString GetName() const;

    CmdRequestRack();
private:
    CmdRequestRack(const CmdRequestRack &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRequestRack & operator = (const CmdRequestRack &);   ///< Not implemented.
}; // end class CmdGenerateBathLayout

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRequestRack &Cmd)
{
    Cmd.CopyToStream(Stream);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRequestRack &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace NetCommands


#endif // CMDREQUESTRACK_H
