/****************************************************************************/
/*! \file   AdjustmentVerifier.h
 *
 *  \brief  Definition of CAdjustmentVerifier
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

#ifndef DATAMANAGER_ADJUSTMENTVERIFIER_H
#define DATAMANAGER_ADJUSTMENTVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Adjustment.h"

namespace DataManager {

const int MIN_X_AXIS_POS = 0;       //! Minimum position allowed in X Axis
const int MAX_X_AXIS_POS = 4500;    //! Maximum position allowed in X Axis

const int MIN_Y_AXIS_POS = 0;       //! Minimum position allowed in Y Axis
const int MAX_Y_AXIS_POS = 3300;    //! Maximum position allowed in Y Axis

const int MIN_Z_AXIS_POS = 0;       //! Minimum position allowed in Z Axis
const int MAX_Z_AXIS_POS = 2000;    //! Maximum position allowed in Z Axis

/****************************************************************************/
/*!
 *  \brief  This class implements verifier for Adjustment module
 */
/****************************************************************************/
class CAdjustmentVerifier : public IVerifierInterface
{
public:
    CAdjustmentVerifier();

    bool VerifyData(CDataContainerBase* p_Adjustment);

    ErrorHash_t& GetErrors();

    void ResetLastErrors();

    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CAdjustmentVerifier() {}

private:
    CAdjustment* mp_Adjusment;//!< Pointer to program Sequence List
    ErrorHash_t m_ErrorsHash;          //!< To store Error ID and any arguments associated
};

} // namespace DataManager

#endif // DATAMANAGER_ADJUSTMENTVERIFIER_H
