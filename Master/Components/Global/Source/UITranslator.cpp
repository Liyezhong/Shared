/****************************************************************************/
/*! \file Global/Source/UITranslator.cpp
 *
 *  \brief Implementation file for class UITranslator.
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


#include <Global/Include/UITranslator.h>

namespace Global {

//lint -esym(1502, Global::UITranslator)
//     UITranslator has only static data
UITranslator    UITranslator::m_Instance;
Translator      UITranslator::m_TranslatorInstance;

/****************************************************************************/
UITranslator::UITranslator() {
}

/****************************************************************************/
UITranslator::~UITranslator() {
}

/****************************************************************************/
Translator &UITranslator::TranslatorInstance() {
    return m_TranslatorInstance;
}

} // end namespace Global

