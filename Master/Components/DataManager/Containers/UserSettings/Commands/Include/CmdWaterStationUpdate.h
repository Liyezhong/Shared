/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Commands/Include/CmdWaterStationUpdate.h
 *
 *  \brief CmdWaterStationUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 04.04.2012
 *   $Author:  $ V.Raghunath
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
#ifndef MSGCLASSES_CMDWATERSTAIONUPDATE_H
#define MSGCLASSES_CMDWATERSTAIONUPDATE_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {
/****************************************************************************/
/*!
 *  \brief  This class implements a CmdWaterUpdate command.
 */
/****************************************************************************/
class CmdWaterStationUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdWaterStationUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdWaterStationUpdate &);
public:
    static QString NAME;    ///< Command name.
    CmdWaterStationUpdate(int TimeOut, QString StationID,
                          const bool &WaterFlowState);
    CmdWaterStationUpdate();
    ~CmdWaterStationUpdate();
    virtual QString GetName() const;
    QString GetWaterStationID() const;
    bool GetWaterFlowState() const;

private:
    CmdWaterStationUpdate(const CmdWaterStationUpdate &);                       ///< Not implemented.
    const CmdWaterStationUpdate & operator = (const CmdWaterStationUpdate &);   ///< Not implemented.
    QString m_StationID;
    bool m_WaterFlowState;   ///< Water Flow State - True if water source is running, else false
};


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdWaterStationUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_StationID;
    Stream << Cmd.m_WaterFlowState;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdWaterStationUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StationID;
    Stream >> Cmd.m_WaterFlowState;
    return Stream;
}

} // End of namespace MsgClasses
#endif // MSGCLASSES_CMDWATERSTATIONUPDATE_H
