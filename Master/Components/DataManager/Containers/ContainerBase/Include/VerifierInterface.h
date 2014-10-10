/****************************************************************************/
/*! \file DataManager/Containers/ContainerBase/Include/VerifierInterface.h
 *
 *  \brief Definition file for interface IVerifierInterface.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth, M.Scherer, Ramya GJ
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

#ifndef DATAMANAGER_VERIFIERINTERFACE_H
#define DATAMANAGER_VERIFIERINTERFACE_H

#include <QWriteLocker>
#include <QReadWriteLock>
#include <QStringList>
#include "DataManager/Helper/Include/Types.h"

namespace DataManager {
class CDataContainerBase;

/****************************************************************************/
/*!
 *  \brief  Used for verifiers
 */
/****************************************************************************/
class IVerifierInterface
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Verifies the container data
     *
     *  \iparam p_DataContainer = pointer to the data container to be verified
     *
     *  \return true or false verification status
     */
    /****************************************************************************/
    virtual bool VerifyData(CDataContainerBase* p_DataContainer) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Resets the errors occured during verification
     */
    /****************************************************************************/
    virtual void ResetErrors() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Returns Error ID's for errors which occured during verification
     *
     *  \return MAP of Error ID's and arguments
     */
    /****************************************************************************/
    virtual ErrorMap_t& GetErrors() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Gets the whether verifier is a local or not
     *
     *  \return Successful (true) or not (false)
     */
    /****************************************************************************/
    virtual bool IsLocalVerifier() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Virtual destructor
     */
    /****************************************************************************/
    virtual ~IVerifierInterface() {}

protected:

};
} // namespace DataManager

#endif // DATAMANAGER_VERIFIERINTERFACE_H
