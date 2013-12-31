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
    /****************************************************************************/
    /**
     * \brief Enum for
     */
    /****************************************************************************/
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
        AssociateRackToParameterSet,
        TransferStationUpdate,
        RackTransferToSepia,    // 12
        RackTransferToColorado,
        Event                       ///< Display event on other end

        //RackInsertRequest
    };

    static QString NAME;    ///< Command name.
//    QString m_RelatedClass;
    /****************************************************************************/
    /**
     * \brief This function gets the DataUpdate Command Type.
     *
     * \return DataUpdate Command type
     */
    /****************************************************************************/
    DataUpdateType GetCmdType() { return (DataUpdateType)m_CmdType; }

    /****************************************************************************/
    /**
     * \brief
     *
     * \iparam  CmdType     Type of Command.
     *
     */
    /****************************************************************************/
    void SetCmdType(DataUpdateType CmdType) { m_CmdType = (quint16)CmdType; }

    /****************************************************************************/
    CmdDataUpdate(int Timeout, const QDataStream &StationUpdateDataStream, DataUpdateType CmdType, bool DlgStatus = true);
    CmdDataUpdate();
    ~CmdDataUpdate();
    virtual QString GetName() const;
    bool GetDialogStatus() const;


    /****************************************************************************/
    /**
     * \brief
     *
     * \return
     */
    /****************************************************************************/
    QByteArray const & GetDataUpdate() const { return m_DataUpdateByteArray;}
private:
    CmdDataUpdate(const CmdDataUpdate &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdDataUpdate & operator = (const CmdDataUpdate &);   ///< Not implemented.
    QByteArray m_DataUpdateByteArray; //!< Contains station update
    quint16 m_CmdType;                //!< Type of Command
    bool    m_DlgStatus;              //!< true - Open Parameter list dialog, false -close dialog
}; // end class CmdDataUpdate

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
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
    Stream << Cmd.m_DlgStatus;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDataUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
//    Stream >> Cmd.m_RelatedClass;
    Stream >> Cmd.m_CmdType;
    Stream >> Cmd.m_DataUpdateByteArray;
    Stream >> Cmd.m_DlgStatus;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDATAUPDATE_H
