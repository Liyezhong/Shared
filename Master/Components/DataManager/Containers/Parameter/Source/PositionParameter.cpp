/****************************************************************************/
/*! \file PositionParameter.cpp
 *
 *  \brief CPositionParameter implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-01
 *   $Author:  $ Thirumalesha R
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

#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/Parameter/Include/Parameter.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Constructor
 *  \iparam Value = Parameter Default Value
 *  \iparam Offset = Parameter Offset;
 */
/****************************************************************************/
CPositionParameter::CPositionParameter(qint16 Value, qint16 Offset) :
    m_Value(Value), m_Offset(Offset)
{
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CPositionParameter::~CPositionParameter()
{
}

}//End of namespace DataManager
