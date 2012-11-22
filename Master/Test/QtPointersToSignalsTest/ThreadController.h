/****************************************************************************/
/*! \file ThreadController.h
 *
 *  \brief Definition file for class ThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#ifndef _THREADCONTROLLER_H_
#define _THREADCONTROLLER_H_

#include <DataLogging/Include/LoggingObject.h>
#include <DataLogging/Include/EventEntry.h>
#include <EventHandler/Include/ActionEntry.h>

using namespace DataLogging;
using namespace EventHandler;

/****************************************************************************/
/**
 * \brief This namespace contains thread related classes.
 *
 * See http://labs.trolltech.com/blogs/2010/06/17/youre-doing-it-wrong/
 * and
 * http://labs.trolltech.com/blogs/2006/12/04/threading-without-the-headache/
 * for the reason we start threads like we do and not how stated in the
 * official Qt documentation.
 */
/****************************************************************************/
namespace Threads {

/****************************************************************************/
/**
 * \brief This class is the base class for thread controlling classes.
 *
 * This class is used instead of subclassing from QThread, which has to be
 * used only to provide an event loop. Instead of subclassing, we use this
 * controller which will "live" in stated thread. This controller is
 * responsible for creating, configuring and destroying all objects that "live"
 * in this thread.\n
 * See http://labs.trolltech.com/blogs/2010/06/17/youre-doing-it-wrong/
 * and
 * http://labs.trolltech.com/blogs/2006/12/04/threading-without-the-headache/
 * for more details.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ThreadController : public LoggingObject {
    Q_OBJECT
private:
    /****************************************************************************/
    ThreadController();                                             ///< Not implemented.
    ThreadController(const ThreadController &);                     ///< Not implemented.
    const ThreadController & operator = (const ThreadController &); ///< Not implemented.
protected:
    /****************************************************************************/
    /**
     * \brief This method is called when we received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     * Do whatever you must do when normal operation started.\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void OnGoReceived() = 0;
    /****************************************************************************/
    /**
     * \brief This method is called when we received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * Do whatever you must do before normal operation ends.\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void OnStopReceived() = 0;
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Although this class is indirectly derived from QObject, we don't provide a
     * parent Object in the constructor, but set it to NULL. This allows calls to
     * moveToThread without reparenting us.
     *
     * \param[in]   LoggingSource   Source to set in log entry.
     */
    /****************************************************************************/
    ThreadController(tSourceType LoggingSource);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~ThreadController();
    /****************************************************************************/
    /**
     * \brief Create and initialize used objects.
     *
     * Create and configure all needed objects. <b>If something goes wrong,
     * cleanup yourself to avoid memory leaks!</b> Don't forget to register created
     * components for logging (if logging is needed). See RegisterFor*Logging
     * for details.\n
     * <b>Remember, you are still in the thread that called this method. No own thread
     * running now.</b>\n
     * Has to be implemented in derived classes.
     *
     * \return      True on success.
     */
    /****************************************************************************/
    virtual bool CreateAndInitializeObjects() = 0;
    /****************************************************************************/
    /**
     * \brief Cleanup and destroy used objects.
     *
     * Destroy all created objects.
     * <b>Remember, you are not running in your own thread, but in the thread that called
     * this method.</b>\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void CleanupAndDestroyObjects() = 0;
    /****************************************************************************/
    /**
     * \brief Register an object for event logging mechanism.
     *
     * This ensures that the object's signal for event logging are forwarded to
     * the correct destination.
     * \warning An \ref Global::Exception may be thrown!
     *
     * \param[in]   pObject     The object to connect to our own signals.
     */
    /****************************************************************************/
    void RegisterForDirectEventLogging(QObject *pObject);
    /****************************************************************************/
    /**
     * \brief Register an object for event logging mechanism.
     *
     * This ensures that the object's signal for event logging is forwarded to
     * our \ref ReceiveEventEntry signal.
     * \warning An \ref Global::Exception may be thrown!
     *
     * \param[in]   pObject     The object to connect to our own signals.
     */
    /****************************************************************************/
    void RegisterForProcessedEventLogging(QObject *pObject);
    /****************************************************************************/
    /**
     * \brief Register an object for day operation logging mechanism.
     *
     * This ensures that the object's signal for day operation logging are forwarded to
     * the correct destination.
     * \warning An \ref Global::Exception may be thrown!
     *
     * \param[in]   pObject     The object to connect to our own signals.
     */
    /****************************************************************************/
    void RegisterForDirectDayOperationLogging(QObject *pObject);
    /****************************************************************************/
    /**
     * \brief Register an object for component test logging mechanism.
     *
     * This ensures that the object's signal for component test logging are forwarded to
     * the correct destination.
     * \warning An \ref Global::Exception may be thrown!
     *
     * \param[in]   pObject     The object to connect to our own signals.
     */
    /****************************************************************************/
    void RegisterForDirectComponentTestLogging(QObject *pObject);

    //TODO: doxy
    tSourceType GetLoggingSource();

public slots:
    /****************************************************************************/
    /**
     * \brief Normal multithreaded operation started. We are ready for signal / slot operation.
     *
     * Basicly we call \ref OnGoReceived.
     */
    /****************************************************************************/
    void Go();
    /****************************************************************************/
    /**
     * \brief Normal multithreaded operation will stop after processing.
     *
     * Basicly we call \ref OnStopReceived.
     */
    /****************************************************************************/
    void Stop();
    /****************************************************************************/
    /**
     * \brief API for receiving local system events.
     *
     *        Signal Emitter of the Thread Controller's local object shall be
     *        connected to this slot if Thread Controller needs to re-map
     *        outgoing local event to global event or if some action shall be
     *        triggered internally before forwarding outgoing event to
     *        System's global event handler. This is don by registering the object
     *        through a call of \ref RegisterForProcessedEventLogging.
     *
     * \param[in]  TheEventEntry = event from one of local objects
     */
    /****************************************************************************/
    virtual void ReceiveEventEntry(const EventEntry & TheEventEntry) = 0;


    /****************************************************************************/
    /**
     * \brief API for receiving global system action.
     *
     *        System actions are normally associated with power down and error
     *        handling events. Each ThreadController shall be able to receive
     *        these actions from the system EventHandler and to process them
     *        according to internal requirements of ThreadController's subsystem.
     *
     * \param[in]  ActionEntry = action from the system event handler
     */
    /****************************************************************************/
    virtual void ReceiveAction(const ActionEntry &) = 0;
};

} // namespace Threads

#endif // _THREADCONTROLLER_H_
