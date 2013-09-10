/****************************************************************************/
/*! \file CmdSwitchToService.h
 *
 *  \brief CmdSWUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.04.2013
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
#ifndef CMDSWUPDATE_H
#define CMDSWUPDATE_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdSWUpdate
 *
 */
/****************************************************************************/
class CmdSWUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdSWUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdSWUpdate &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSWUpdate(int Timeout, const bool USBUpdate);
    CmdSWUpdate();
    ~CmdSWUpdate();
    virtual QString GetName() const;
    bool m_USBUpdate; //!< true - SW update from USB stick, false- SW update from RemoteCare

private:
    CmdSWUpdate(const CmdSWUpdate &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdSWUpdate & operator = (const CmdSWUpdate &); ///< Not implemented.

}; // end class CmdSWUpdate

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdSWUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_USBUpdate;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdSWUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_USBUpdate;
    return Stream;
}

} // end namespace NetCommands
#endif // CMDSWUPDATE_H
