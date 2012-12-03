/****************************************************************************/
/*! \file Global/Include/UITranslator.h
 *
 *  \brief Definition file for class UITranslator.
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

#ifndef GLOBAL_UITRANSLATOR_H
#define GLOBAL_UITRANSLATOR_H

#include <Global/Include/Translator.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Singleton class for translating UI strings.
 */
/****************************************************************************/
class UITranslator {
private:
    static UITranslator     m_Instance;             ///< Own instance.
    static Translator       m_TranslatorInstance;   ///< Instance of translator.
    /****************************************************************************/
    UITranslator(const UITranslator &);                     ///< Not implemented.
    const UITranslator & operator = (const UITranslator &); ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    UITranslator();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    ~UITranslator();
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
}; // end class UITranslator

} // end namespace Global

#endif // GLOBAL_UITRANSLATOR_H
