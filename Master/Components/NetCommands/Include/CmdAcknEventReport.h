/****************************************************************************/
/*! \file NetCommands/Source/CmdAcknEventReport.h
 *
 *  \brief CmdAcknEventReport command definition
 *
 *   $Version: $ 0.1
 *   $Date:    $ 09.11.2012
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
#ifndef CMDACKNEVENTREPORT_H
#define CMDACKNEVENTREPORT_H
#include <Global/Include/Commands/Acknowledge.h>
#include <QDataStream>

namespace NetCommands {
enum ClickedButton_t {
    OK_BUTTON,
    YES_BUTTON,
    CONTINUE_BUTTON,
    NO_BUTTON,
    CANCEL_BUTTON,
    STOP_BUTTON
};

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdAcknEventReport : public Global::Acknowledge {
    friend QDataStream & operator << (QDataStream &, const CmdAcknEventReport &);
    friend QDataStream & operator >> (QDataStream &, CmdAcknEventReport &);
private:
    /****************************************************************************/

    CmdAcknEventReport(const CmdAcknEventReport &);                     ///< Not implemented.
    const CmdAcknEventReport & operator = (const CmdAcknEventReport &); ///< Not implemented.
    //quint64 m_EventID;      //!< 64 bit event id (4 bytes original event ID + lower four bytes for occurence cout)
    ClickedButton_t m_ClickedButton; //!< Button clicked by user in Msg Box
    quint64 m_EventKey;   //! Higher 4 bytes Event Id + Lower 4 Bytes Unique key

protected:
public:
    CmdAcknEventReport();
    static QString  NAME;           ///< Command name.

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   EventKey
     * \param[in]   ClickedBtn
     */
    /****************************************************************************/
    CmdAcknEventReport(quint64 EventKey, ClickedButton_t ClickedBtn);

    /****************************************************************************/

    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdAcknEventReport();
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }

    /****************************************************************************/
    /**
     * \brief Get Event Key
     *
     * \return  Event Key
     */
    /****************************************************************************/
    quint64 GetEventKey() const {
        return m_EventKey;
    }

    /****************************************************************************/
    /**
     * \brief Get Button Clicked
     *
     * \return  Clicked Button
     */
    /****************************************************************************/
    ClickedButton_t GetButtonClicked() const {
        return m_ClickedButton;
    }
}; // end class CmdAcknEventReport


/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAcknEventReport &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << (int)Cmd.m_ClickedButton << Cmd.m_EventKey;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAcknEventReport &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> (int &)Cmd.m_ClickedButton >> Cmd.m_EventKey;
    return Stream;
}

} // end namespace DeviceCommandProcessor
#endif // CMDACKNEVENTREPORT_H
