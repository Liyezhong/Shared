/****************************************************************************/
/*! \file Global/Include/EventTranslator.h
 *
 *  \brief Definition file for class EventTranslator.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-05-10
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

#ifndef GLOBAL_EVENTTRANSLATOR_H
#define GLOBAL_EVENTTRANSLATOR_H

#include <Global/Include/Translator.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Singleton class for translating event strings.
 */
/****************************************************************************/
class EventTranslator {
private:
    static EventTranslator  m_Instance;             ///< Own instance.
    static Translator       m_TranslatorInstance;   ///< Instance of translator.
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(EventTranslator)
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    EventTranslator();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~EventTranslator();
protected:
public:
    /****************************************************************************/
    /**
     * \brief Get reference to translator instance.
     *
     * \return      Reference to translator instance.
     */
    /****************************************************************************/
    static Translator &TranslatorInstance();
}; // end class EventTranslator

} // end namespace Global

#endif // GLOBAL_EVENTTRANSLATOR_H
