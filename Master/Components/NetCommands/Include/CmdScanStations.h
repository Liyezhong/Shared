/****************************************************************************/
/*! \file CmdScanStations.h
 *
 *  \brief CmdScanStations command definition
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30-09-2013
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
#ifndef CMDSCANSTATIONS_H
#define CMDSCANSTATIONS_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdScanStations
 *
 */
/****************************************************************************/
class CmdScanStations : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdScanStations &);
    friend QDataStream & operator >> (QDataStream &, CmdScanStations &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdScanStations(int Timeout);
    CmdScanStations();
    ~CmdScanStations();
    virtual QString GetName() const;

private:
    CmdScanStations(const CmdScanStations &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdScanStations & operator = (const CmdScanStations &); ///< Not implemented.

}; // end class CmdScanStations

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdScanStations &Cmd)
{
    // copy base class data
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
inline QDataStream & operator >> (QDataStream &Stream, CmdScanStations &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace NetCommands

#endif // CMDSCANSTATIONS_H
