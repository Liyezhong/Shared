/****************************************************************************/
/*! \file CmdGuiEventReport.h
 *
 *  \brief CmdGuiEventReport command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-23
 *   $Author:  $ Raju
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

#ifndef NETCOMMANDS_CmdGuiEventReportREPORT_H
#define NETCOMMANDS_CmdGuiEventReportREPORT_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QByteArray>

namespace NetCommands {

//!< Data Structure containing EventReportData
struct GuiEventReportStruct{
    quint32 EventCode; //!< Event ID
    QList<QString> EventStringList; ///< EventString List
    Global::AlternateEventStringUsage AltStringUsage;  //!< Event String
};


/****************************************************************************/
/*!
 *  \brief  This class implements a CmdGuiEventReport command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdGuiEventReport : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdGuiEventReport &);
    friend QDataStream & operator >> (QDataStream &, CmdGuiEventReport &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdGuiEventReport(int TimeOut, GuiEventReportStruct &EventReportData);
    CmdGuiEventReport();                                                    ///< Not implemented.
    ~CmdGuiEventReport();

    virtual QString GetName() const;

    /****************************************************************************/
    /**
     * \brief Get the EventReport Data
     * \return Byte array.
     */
    /****************************************************************************/
    QByteArray const & GetEventData() const { return m_GuiEventReportByteArray;}

private:
    CmdGuiEventReport(const CmdGuiEventReport &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdGuiEventReport & operator = (const CmdGuiEventReport &);   ///< Not implemented.
    QByteArray m_GuiEventReportByteArray; ///< Event Report Data ByteArray

}; // end class CmdGuiEventReport

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdGuiEventReport &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_GuiEventReportByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdGuiEventReport &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_GuiEventReportByteArray;
    return Stream;
}


} // end namespace NetCommands

#endif // NETCOMMANDS_CmdGuiEventReportREPORT_H
