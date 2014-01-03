/****************************************************************************/
/*! \file Global/Include/Commands/PendingCmdDescriptor.h
 *
 *  \brief Definition file for class PendingCmdDescriptor.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-09
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

#ifndef GLOBAL_PENDINGCMDDESCRIPTOR_H
#define GLOBAL_PENDINGCMDDESCRIPTOR_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/SharedPointer.h>

#include <QHash>
#include <QTimer>

namespace Global {

/****************************************************************************/
/**
 * \brief Pending command descriptor class.
 *
 * This class holds the description and timer for the a pending command.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class PendingCmdDescriptor : public QObject {
    Q_OBJECT
private:
    tRefType    m_Ref;      ///< Command reference.
    QString     m_Name;     ///< Name of command.
    QTimer      m_Timer;    ///< Timer for timeout signal.
    /****************************************************************************/
    PendingCmdDescriptor();                                                 ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(PendingCmdDescriptor)
private slots:
    /****************************************************************************/
    /**
     * \brief Slot for command timeout.
     *
     * It is called by own timer and emits a specififc signal containing the
     * command reference.
     */
    /****************************************************************************/
    void OnTimeout() {
//        qDebug() << "PendingCmdDescriptor::OnTimeOut" << m_Name << m_Timer.interval();
        emit TimeoutOccured(m_Ref, m_Name);
    }
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pParent     Pointer to parent object.
     * \param[in]   Ref         Command reference.
     * \param[in]   Name        Command name.
     * \param[in]   Timeout     Timeout for command [ms].
     */
    /****************************************************************************/
    PendingCmdDescriptor(QObject *pParent, tRefType Ref, const QString &Name, int Timeout);
    /****************************************************************************/
    /**
     * \brief Connect and start timer.
     *
     * On error, an exception is thrown!
     */
    /****************************************************************************/
    void ConnectAndStartTimer();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    inline ~PendingCmdDescriptor() {
        // stop the timer before deleting the objects, so that connected signals
        // will not work
        try {
            m_Timer.stop();
        }
        CATCHALL_DTOR();
    }
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command Name.
     */
    /****************************************************************************/
    inline QString GetName() const {
        return m_Name;
    }
signals:
    /****************************************************************************/
    /**
     * \brief Command timeout occured.
     *
     * We do not send name by ref since somebody may delete us!
     * \param[in]   Ref     Command reference.
     * \param[in]   Name    Command name.
     */
    /****************************************************************************/
    void TimeoutOccured(Global::tRefType Ref, QString Name);
}; // end class PendingCmdDescriptor

typedef SharedPointer<PendingCmdDescriptor>         PendingCmdDescriptorShP_t;      ///< Typedef for a SharedPointer of PendingCmdDescriptor.
typedef QHash<tRefType, PendingCmdDescriptorShP_t>  PendingCmdDescriptorPtrHash_t;  ///< Hash for identifying some PendingCmdDescriptor over its reference.

} // end namespace Global

#endif // GLOBAL_PENDINGCMDDESCRIPTOR_H
