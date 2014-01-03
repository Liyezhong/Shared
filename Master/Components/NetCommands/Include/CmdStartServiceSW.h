/****************************************************************************/
/*! \file CmdStartServiceSW.h
 *
 *  \brief CmdStartServiceSW command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 09.08.2013
 *   $Author:  $ N.Kamath
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
#ifndef CMDSTARTSERVICESW_H
#define CMDSTARTSERVICESW_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdStartServiceSW. Command sent GUI
 *          to Main S/W to initiate shutdown and start service software.
 */
/****************************************************************************/
class CmdStartServiceSW : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdStartServiceSW &);
    friend QDataStream & operator >> (QDataStream &, CmdStartServiceSW &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdStartServiceSW(int Timeout);
    CmdStartServiceSW();
    ~CmdStartServiceSW();
    virtual QString GetName() const;

private:
    CmdStartServiceSW(const CmdStartServiceSW &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdStartServiceSW & operator = (const CmdStartServiceSW &); ///< Not implemented.

}; // end class CmdStartServiceSW

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdStartServiceSW &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdStartServiceSW &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace NetCommands
#endif // CMDSTARTSERVICESW_H
