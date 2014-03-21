/****************************************************************************/
/*! \file CmdDataUpdate.h
 *
 *  \brief CmdDataUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-29
 *   $Author:  $ Michael Thiel
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

#ifndef NETCOMMANDS_CMDDATAUPDATE_H
#define NETCOMMANDS_CMDDATAUPDATE_H

#include <QByteArray>
#include "Global/Include/Commands/Command.h"

namespace NetCommands {


/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDataUpdate command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdDataUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDataUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdDataUpdate &);
public:
    enum DataUpdateType {
        UndefinedCmdType,
        StationBase,
        RackUpdateRequest,
        Rack,
        RackDurationRequest,
        RackDuration,
        ParameterSetRequest,
        ParameterSet,
        AllParameterSetsRequest,
        AllParameterSets,
        AssociateRackToParameterSet
    };

    static QString NAME;    ///< Command name.
//    QString m_RelatedClass;
    DataUpdateType GetCmdType() { return (DataUpdateType)m_CmdType; }
    void SetCmdType(DataUpdateType CmdType) { m_CmdType = (quint16)CmdType; }

    /****************************************************************************/
    CmdDataUpdate(int Timeout, const QDataStream &StationUpdateDataStream, DataUpdateType CmdType);
    CmdDataUpdate();
    ~CmdDataUpdate();
    virtual QString GetName() const;
    QByteArray const & GetDataUpdate() const { return m_DataUpdateByteArray;}
private:
    CmdDataUpdate(const CmdDataUpdate &);                       ///< Not implemented.
    const CmdDataUpdate & operator = (const CmdDataUpdate &);   ///< Not implemented.
    QByteArray m_DataUpdateByteArray; //!< Contains station update
    quint16 m_CmdType;
}; // end class CmdDataUpdate

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDataUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
//    Stream << Cmd.m_RelatedClass;
    Stream << Cmd.m_CmdType;
    Stream << Cmd.m_DataUpdateByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDataUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
//    Stream >> Cmd.m_RelatedClass;
    Stream >> Cmd.m_CmdType;
    Stream >> Cmd.m_DataUpdateByteArray;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDATAUPDATE_H
