/****************************************************************************/
/*! \file CmdBathlayoutGenerationFailed.h
 *
 *  \brief CmdBathlayoutGenerationFailed command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 06.11.2012
 *   $Author:  $ Ningu
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

#ifndef NETCOMMANDS_CMDBATHLAYOUTGENERATIONFAILED_H
#define NETCOMMANDS_CMDBATHLAYOUTGENERATIONFAILED_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdBathlayoutGenerationFailed command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdBathlayoutGenerationFailed : public Global::Command {
public:
    static QString NAME;    ///< Command name.
    friend QDataStream & operator << (QDataStream &, const CmdBathlayoutGenerationFailed &);
    friend QDataStream & operator >> (QDataStream &, CmdBathlayoutGenerationFailed &);
    /****************************************************************************/
    CmdBathlayoutGenerationFailed(int Timeout, const QString &BathlayoutGenStatus);
    ~CmdBathlayoutGenerationFailed();
    virtual QString GetName() const;
    QString GetBathLayoutStatus() const;

    CmdBathlayoutGenerationFailed();
private:
    CmdBathlayoutGenerationFailed(const CmdBathlayoutGenerationFailed &);                       ///< Not implemented.
    const CmdBathlayoutGenerationFailed & operator = (const CmdBathlayoutGenerationFailed &);   ///< Not implemented.
private:
    QString      m_BathLayoutGenStatus;       ///< The bath layout.
}; // end class CmdBathlayoutGenerationFailed
/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdBathlayoutGenerationFailed &Cmd)
{
    Q_UNUSED(Cmd);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdBathlayoutGenerationFailed &Cmd)
{
    Q_UNUSED(Cmd);
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDBATHLAYOUTGENERATIONFAILED_H
