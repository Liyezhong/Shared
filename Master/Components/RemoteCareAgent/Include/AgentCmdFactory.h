/****************************************************************************/
/*! \file AgentCmdFactory.h
 *
 *  \brief Definition file for commands creation.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 16.09.2011
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

#ifndef RCAGENTNAMESPACE_AGENTCMDFACTORY_H
#define RCAGENTNAMESPACE_AGENTCMDFACTORY_H

#include <Global/Include/Commands/Command.h>

namespace RCAgentNamespace {

/****************************************************************************/
/**
 * \brief Base factory class for command creation.
 */
/****************************************************************************/
class AgentCmdFactory {
private:
    /****************************************************************************/
    AgentCmdFactory(const AgentCmdFactory &);                     ///< Not implemented.
    const AgentCmdFactory & operator = (const AgentCmdFactory &); ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline AgentCmdFactory() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AgentCmdFactory() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \param[in] Instance = the command instance to dispatch.
     */
    /****************************************************************************/
    virtual Global::CommandShPtr_t CreateInstance() = 0;
}; // end class AgentCreatorFunctor

/****************************************************************************/
/**
 * \brief Template class for command creation.
 *
 * This type of class is used to instantiate commands from class names.
 */
/****************************************************************************/
template<class TheClass> class TemplateAgentCmdFactory : public AgentCmdFactory {
private:
    TheClass            *m_ClassInstance;                                           ///< the class to reproduce.
    /****************************************************************************/
    TemplateAgentCmdFactory(const TemplateAgentCmdFactory &);                       ///< Not implemented.
    const TemplateAgentCmdFactory & operator = (const TemplateAgentCmdFactory &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    TemplateAgentCmdFactory() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateAgentCmdFactory() {
    }
    /****************************************************************************/
    /**
     * \brief Create the command and return the shared pointer.
     */
    /****************************************************************************/
    virtual Global::CommandShPtr_t CreateInstance()
    {
        try {
            m_ClassInstance = new TheClass();
        }
        catch (const std::bad_alloc &) {
            /// \todo log error
            return Global::CommandShPtr_t(NULL);
        }
        // pass instance to consumer
        return Global::CommandShPtr_t(m_ClassInstance);
    }

}; // end class TemplateAgentCmdFactory

} // namespace

#endif
