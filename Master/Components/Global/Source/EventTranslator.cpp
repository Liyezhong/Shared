/****************************************************************************/
/*! \file Global/Source/EventTranslator.cpp
 *
 *  \brief Implementation file for class EventTranslator.
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


#include <Global/Include/EventTranslator.h>

namespace Global {

//lint -esym(1502,Global::EventTranslator)
//     EventTranslator has only static data
EventTranslator EventTranslator::m_Instance;
Translator      EventTranslator::m_TranslatorInstance;

/****************************************************************************/
EventTranslator::EventTranslator() {
}

/****************************************************************************/
EventTranslator::~EventTranslator() {
}

/****************************************************************************/
Translator &EventTranslator::TranslatorInstance() {
    return m_TranslatorInstance;
}

} // end namespace Global

