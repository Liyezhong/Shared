/****************************************************************************/
/*! \file DispatcherFunctor.h
 *
 *  \brief Definition file for functors needed by command classes.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 15.09.2011
 *  $Author:    $ Y.Novak
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

#ifndef RCAGENTNAMESPACE_DISPATCHERFUNCTOR_H
#define RCAGENTNAMESPACE_DISPATCHERFUNCTOR_H

#include <Global/Include/Commands/Command.h>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Base functor class for specific dispatching.
 */
/****************************************************************************/
class DispatcherFunctor {
private:
    /****************************************************************************/
    DispatcherFunctor(const DispatcherFunctor &);                     ///< Not implemented.
    const DispatcherFunctor & operator = (const DispatcherFunctor &); ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline DispatcherFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~DispatcherFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \param[in] Instance = the command instance to dispatch.
     */
    /****************************************************************************/
    virtual void Dispatch(const Global::CommandShPtr_t &Instance) = 0;
}; // end class DispatcherFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific dispatching.
 *
 * This type of functor is used to "link" instantiated commands to specific methods.
 */
/****************************************************************************/
template<class TheClass, class TheProcessor> class TemplateDispatcherFunctor : public DispatcherFunctor {
private:
    TheProcessor            *m_pTheProcessor;                                            ///< Pointer to processor class.
    void (TheProcessor::    *m_FunctionPointer)(const Global::CommandShPtr_t &Instance); ///< Pointer to member function.
    /****************************************************************************/
    TemplateDispatcherFunctor();                                                      ///< Not implemented.
    TemplateDispatcherFunctor(const TemplateDispatcherFunctor &);                     ///< Not implemented.
    const TemplateDispatcherFunctor & operator = (const TemplateDispatcherFunctor &); ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pTheProcessor       Processor class.
     * \param[in]   FunctionPointer     The function pointer.
     */
    /****************************************************************************/
    TemplateDispatcherFunctor(TheProcessor *pTheProcessor,
                           void(TheProcessor::*FunctionPointer)(const Global::CommandShPtr_t &Instance)) :
        m_pTheProcessor(pTheProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateDispatcherFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Dispatch the command to the processing function.
     *
     * \param[in] Instance = the command instance to dispatch.
     */
    /****************************************************************************/
    virtual void Dispatch(const Global::CommandShPtr_t &Instance)
    {
        // check if this Instance is what we really expect
        const TheClass *pTheClass = dynamic_cast<const TheClass*>(Instance.GetPointerToUserData());
        if (pTheClass == NULL) {
            // no luck
            /// \todo: handle error
            qDebug() << "*** TemplateDispatcherFunctor: Dispatch failed to dynamic_cast the Instance !";
            return;
        }
        // pass instance to consumer
        (*m_pTheProcessor.*m_FunctionPointer)(Instance);
    }

}; // end class TemplateDispatcherFunctor

} // namespace

#endif
