/****************************************************************************/
/*! \file CmdScheduledStations.h
 *
 *  \brief Definition file for class CmdScheduledStations.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-06
 *  $Author:    $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef CMDSCHEDULEDSTATIONS_H
#define CMDSCHEDULEDSTATIONS_H


#include <Global/Include/Commands/Command.h>
#include <QHash>
#include <QStringList>
namespace NetCommands {
typedef QHash<qint16, QString> ScheduledStations_t;     ///< StepIdx, StationId
/****************************************************************************/
/**
 * \brief
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdScheduledStations : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdScheduledStations &);
    friend QDataStream & operator >> (QDataStream &, CmdScheduledStations &);
private:
    /****************************************************************************/

    CmdScheduledStations(const CmdScheduledStations &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdScheduledStations & operator = (const CmdScheduledStations &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    QHash<quint32, ScheduledStations_t> m_ScheduledStation;  //!< List of scheduled stations for a Rack

    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdScheduledStations();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TimeOut       timeout for Command.
     */
    /****************************************************************************/
    CmdScheduledStations(int TimeOut);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdScheduledStations();
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
}; // end class CmdScheduledStations

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdScheduledStations &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ScheduledStation;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdScheduledStations &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ScheduledStation;
    return Stream;
}
} // end namespace NetCommands

#endif // CMDSCHEDULEDSTATIONS_H
