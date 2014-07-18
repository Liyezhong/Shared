/****************************************************************************/
/*! \file NetCommands/Include/CmdRequestShutDown.h
 *
 *  \brief Implementation file for class CmdRequestShutDown. This command
 *          is used to ask user to shutdown the system on system error.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-11-02
 *  $Author:    $ Ramya GJ
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
#ifndef CmdRequestShutDown_H
#define CmdRequestShutDown_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief Class for telling colorado is shutting down.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdRequestShutDown : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRequestShutDown &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRequestShutDown &);
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \iparam   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const CmdRequestShutDown &rOther);
protected:
public:
    static QString  NAME;                   //!< Command name.
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CmdRequestShutDown();
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CmdRequestShutDown(int TimeOut, bool ShutDown=false);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \iparam   rOther  Instance to copy from.
     */
    /****************************************************************************/
    CmdRequestShutDown(const CmdRequestShutDown &rOther);//!< Not implemented
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \iparam   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const CmdRequestShutDown & operator = (const CmdRequestShutDown &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdRequestShutDown();
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
     * \brief Get shut down status
     *
     * \return  true if system will be shutdown, false if asking user to shutdown
     */
    /****************************************************************************/
    bool GetShutDownStatus() const {
        return m_ShutDown;
    }

private:
    bool m_ShutDown;        //!< system will be shut down or ask user to shutdown

}; // end class CmdShutDown

/****************************************************************************/
/**
 * \brief Output Stream Operator which streams data
 *
 * \oparam   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return   Output Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRequestShutDown &Cmd) {
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // copy internal data
    StreamRef << Cmd.m_ShutDown;
    return StreamRef;
}

/****************************************************************************/
/**
 * \brief Input Stream Operator which streams data
 *
 * \oparam   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return   Input Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRequestShutDown &Cmd) {
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRequestShutDown& CmdRef = const_cast<CmdRequestShutDown &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // copy to internal data
    StreamRef >> CmdRef.m_ShutDown;
    return StreamRef;
}
}// end of namespace NetCommands
#endif // CmdRequestShutDown_H
