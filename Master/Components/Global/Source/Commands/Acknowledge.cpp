/****************************************************************************/
/*! \file Global/Source/Commands/Acknowledge.cpp
 *
 *  \brief Implementation file for class Acknowledge.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-01
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

#include <Global/Include/Commands/Acknowledge.h>

namespace Global {

const QString Acknowledge::SERIALIZERSTRING = "Acknowledge";

/****************************************************************************/
Acknowledge::Acknowledge()
    : m_Ref(0)
{
}

/****************************************************************************/
Acknowledge::~Acknowledge() {
}

} // end namespace Global
