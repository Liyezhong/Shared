/****************************************************************************/
/*! \file Global/Source/Commands/AckOKNOK.cpp
 *
 *  \brief Implementation file for class AckOKNOK.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-23
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

#include <Global/Include/Commands/AckOKNOK.h>

namespace Global {

QString AckOKNOK::NAME = "Global::AckOKNOK";

/****************************************************************************/
AckOKNOK::AckOKNOK() :
    m_Status(false),
    m_Type(GUIMSGTYPE_UNDEFINED)
{
}

/****************************************************************************/
AckOKNOK::AckOKNOK(bool Status, const QString &Text, Global::GUIMessageType Type) :
    m_Status(Status),
    m_Type(Type),
    m_Text(Text)
{
}

/****************************************************************************/
AckOKNOK::~AckOKNOK() {
}

} // end namespace Global
