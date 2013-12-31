/****************************************************************************/
/*! \file Global/Include/SignalHandler.h
 *
 *  \brief Definition file for class SignalHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-08-08
 *  $Author:    $ Michael Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef GLOBAL_SIGNALHANDLER_H
#define GLOBAL_SIGNALHANDLER_H

#include <QStringList>

namespace Global {

/****************************************************************************/
/**
 * \brief SignalHandler class.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class SignalHandler : public QObject {

    Q_OBJECT

public:
    SignalHandler();
//    ~SignalHandler();
    /****************************************************************************/
    /**
     * \brief initialization method.
     */
    /****************************************************************************/
    void init();

    /****************************************************************************/
    /**
     * \brief handler for system crash
     * \iparam  sig = signal received
     */
    /****************************************************************************/
    static void crashHandler(int sig);

    /****************************************************************************/
    /**
     * \brief gets back trace
     * \iparam depth = number of backtrace function calls
     * \return back trace string
     */
    /****************************************************************************/
    static QStringList getBackTrace(int depth = 20);

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(SignalHandler)
};

}

#endif // GLOBAL_SIGNALHANDLER_H
