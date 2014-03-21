/****************************************************************************/
/*! \file CmdRequestSepiaRack.h
 *
 *  \brief CmdRequestSepiaRack command definition.
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

#ifndef CMDREQUESTSEPIARACK_H
#define CMDREQUESTSEPIARACK_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdGenerateBathLayout command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRequestSepiaRack : public Global::Command {
public:
    static QString NAME;    ///< Command name.

    friend QDataStream & operator << (QDataStream &, const CmdRequestSepiaRack &);
    friend QDataStream & operator >> (QDataStream &, CmdRequestSepiaRack &);
    /****************************************************************************/
    CmdRequestSepiaRack(int Timeout);
    ~CmdRequestSepiaRack();
    virtual QString GetName() const;

    CmdRequestSepiaRack();
private:
    CmdRequestSepiaRack(const CmdRequestSepiaRack &);                       ///< Not implemented.
    const CmdRequestSepiaRack & operator = (const CmdRequestSepiaRack &);   ///< Not implemented.
}; // end class CmdGenerateBathLayout

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRequestSepiaRack &Cmd)
{
    Cmd.CopyToStream(Stream);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRequestSepiaRack &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace NetCommands


#endif // CMDREQUESTSEPIARACK_H
