/****************************************************************************/
/*! \file PositionParameter.h
 *
 *  \brief Definition file for class PositionParameter.
 *  This class provides functions to read Parameter attributes
 *  from the XML and writing data to the XML
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-01
 *  $Author:    $ Thirumalesha R
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

#ifndef DATAMANAGER_POSITIONPARAMETER_H
#define DATAMANAGER_POSITIONPARAMETER_H

#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"

namespace DataManager
{
class CProcessSettings;

/****************************************************************************/
/*!
 *  \brief  This class implements CPositionParameter to store positional parameters of
 *          devices
 */
/****************************************************************************/
class CPositionParameter
{
    friend class CProcessSettings;

private:
    qint16 m_Value;        //!< Default Value of Parameter
    qint16 m_Offset;       //!< Offset of Parameter

public:
    CPositionParameter(qint16 Value = 0, qint16 Offset = 0);
    ~CPositionParameter();

    /****************************************************************************/
    /*!
     *  \brief  To get position value (default value + offset)
     *  \return Parameter Value
     */
    /****************************************************************************/
     qint32 GetPosition() { return m_Value + m_Offset; }

    /****************************************************************************/
    /*!
     *  \brief  To set position value. Adjusts offset only.
     *  \iparam Parameter Value.
     */
    /****************************************************************************/
     void SetPosition(qint32 Position) { m_Offset = Position - m_Value; }

	/****************************************************************************/
	/*!
     *  \brief  To reset parameter offset
     */
    /****************************************************************************/
     void ResetOffset() { m_Offset = 0; }

private:
     qint16 GetValue() { return m_Value; }

     qint16 GetOffsetValue() { return m_Offset; }
};

}   // namespace DataManager
#endif // DATAMANAGER_PARAMETER_H
