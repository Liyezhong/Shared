/****************************************************************************/
/*! \file Global/Include/Commands/Command.h
 *
 *  \brief Definition file for class Command.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-02-22
 *  $Author:    $ J.Bugariu
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

#ifndef GLOBAL_COMMAND_H
#define GLOBAL_COMMAND_H

#include <Global/Include/SharedPointer.h>

#include <QString>
#include <QDataStream>
#include <QBitArray>

namespace Global {

/****************************************************************************/
/**
 * \brief Base class for commands.
 *
 * All commands exchanged between thread controllers must be subclasses of
 * this class.
 * \warning Although NOTIMEOUT is defined it should be used only for commands
 * that really do not need an acknowledge, because they are broadcasted around.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class Command {
private:
    int     m_Timeout;  ///< Command timeout [ms]. NOTIMEOUT means that no acknowledge is required for this command!
    /****************************************************************************/
    Command();          ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Copy method
     *
     * \param[in]   rOther      Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const Command &rOther);

    QBitArray m_stateGuard;
protected:
    /****************************************************************************/
    /**
     * \brief Copy to some stream.
     *
     * \param[in,out]   Stream      Stream to stream into.
     */
    /****************************************************************************/
    inline void CopyToStream(QDataStream &Stream) const {
        // copy internal data without name
        Stream << m_Timeout;
    }
    /****************************************************************************/
    /**
     * \brief Get from some stream.
     *
     * \param[in,out]   Stream      Stream to stream from.
     */
    /****************************************************************************/
    inline void CopyFromStream(QDataStream &Stream) {
        // copy internal data without name
        Stream >> m_Timeout;
    }
public:
    static const QString    SERIALIZERSTRING;   ///< String used to detect a command when serializing.
    static const int        NOTIMEOUT;          ///< Value for no timeout.
    /****************************************************************************/
    /**
     * \brief The only allowed constructor.
     *
     * The only allowed constructor.
     *
     * \param[in]   Timeout     Timeout for command.
     */
    /****************************************************************************/
    Command(int Timeout);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther      Instance to copy from.
     */
    /****************************************************************************/
    Command(const Command &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~Command();
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther      Instance to copy from.
     */
    /****************************************************************************/
    const Command & operator = (const Command &rOther);
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const = 0;
    /****************************************************************************/
    /**
     * \brief Get command timeout.
     *
     * \return  Command timeout.
     */
    /****************************************************************************/
    inline int GetTimeout() const {
        return m_Timeout;
    }

    inline bool isErrorStateAllowed() { return m_stateGuard.at(3); }
    inline bool isBusyStateAllowed() { return m_stateGuard.at(2); }
    inline bool isIdleStateAllowed() { return m_stateGuard.at(1); }

    inline void setErrorStateAllowed(bool allowed) { m_stateGuard.setBit(3, allowed); }
    inline void setBusyStateAllowed(bool allowed) { m_stateGuard.setBit(2, allowed); }
    inline void setIdleStateAllowed(bool allowed) { m_stateGuard.setBit(1, allowed); }

    bool isStateAllowed(QString state);

}; // end class Command

typedef Global::SharedPointer<Global::Command>  CommandShPtr_t;   ///< Typedef for shared pointer of command.

} // end namespace Global

#endif // GLOBAL_COMMAND_H
