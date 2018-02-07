/****************************************************************************/
/*! \file HimalayaDataContainer/Helper/Include/VerifierInterface.h
 *
 *  \brief Definition file for interface IVerifierInterface.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-07
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_VERIFIERINTERFACE_H
#define DATAMANAGER_VERIFIERINTERFACE_H

#include "DataManager/Templates/Include/Types.h"


namespace DataManager {

class DataContainerBase;

/****************************************************************************/
/*!
 *  \brief  interface for all Container Verifiers.
 */
/****************************************************************************/
class VerifierInterface
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Virtual destructor
     */
    /****************************************************************************/
    virtual ~VerifierInterface() { }

    /****************************************************************************/
    /*!
     *  \brief  Verifies the input data container.
     *
     *  \param[in] p_DataContainer pointer to the data container which will be
     *                             verified.
     *
     *  \return true, if the data container is valid; false, if the data
     *          container is not acceptable, or the data container type is
     *          not compatible with the Verifier.
     */
    /****************************************************************************/
    virtual bool VerifyData(DataContainerBase* p_DataContainer) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Resets the last error which is done by verifier.
     */
    /****************************************************************************/
    virtual void ResetLastErrors() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Returns errors which occured during verification
     *
     *  \return Hash of Error ID's and arguments
     */
    /****************************************************************************/
    virtual ErrorHash_t& GetErrors() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Gets the whether verifier is a local or not
     *
     *  \return true, local; false, not-local.
     */
    /****************************************************************************/
    virtual bool IsLocalVerifier() = 0;

};
} // namespace DataManager

#endif // DATAMANAGER_VERIFIERINTERFACE_H
