/****************************************************************************/
/*! \file   AdjustmentVerifier.cpp
 *
 *  \brief  CAdjustmentVerifier implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-01
 *   $Author:  $ Stalin
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
#include <QFile>
#include <QStringList>

#include "../Include/Adjustment.h"
#include "../Include/AdjustmentVerifier.h"

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CAdjustmentVerifier::CAdjustmentVerifier() : mp_Adjusment(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief  Function to verify the data present in xml

 *  \iparam p_Adjustment
 *
 *  \return Changed something (true) or not (false)
 */
/****************************************************************************/
bool CAdjustmentVerifier::VerifyData(CDataContainerBase *p_Adjustment)
{
    mp_Adjusment = static_cast<CAdjustment*>(p_Adjustment);

    //! \brief Verify each station positions (x, y, z)

    if (mp_Adjusment != NULL)
    {
        quint8 Row;
        quint8 Column;
        PositionXYZ Position;

        for (Row = 0; Row < XYZ_MAX_ROWS; Row++)
        {
            for (Column = 0; Column < XYZ_MAX_COLS; Column++)
            {
                // Verify Left side values
                if (mp_Adjusment->GetXyzPosition(LEFT_XYZ, Row, Column, Position) == false)
                {
                    return false;
                }

                if ((Position.PositionX < MIN_X_AXIS_POS || Position.PositionX > MAX_X_AXIS_POS) ||
                        (Position.PositionY < MIN_Y_AXIS_POS || Position.PositionY > MAX_Y_AXIS_POS) ||
                        (Position.PositionZ < MIN_Z_AXIS_POS || Position.PositionZ > MAX_Z_AXIS_POS))
                {
                    return false;
                }

                // Verify Right side values
                if (mp_Adjusment->GetXyzPosition(RIGHT_XYZ, Row, Column, Position) == false)
                {
                    return false;
                }

                if ((Position.PositionX < MIN_X_AXIS_POS || Position.PositionX > MAX_X_AXIS_POS) ||
                        (Position.PositionY < MIN_Y_AXIS_POS || Position.PositionY > MAX_Y_AXIS_POS) ||
                        (Position.PositionZ < MIN_Z_AXIS_POS || Position.PositionZ > MAX_Z_AXIS_POS))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CAdjustmentVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CAdjustmentVerifier::ResetLastErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
bool CAdjustmentVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager

