/****************************************************************************/
/*! \file SWVersionListVerifier.h
 *
 *  \brief Definition file for class CExportConfigurationVerifier.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
 *  $Author:    $ Raju
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

#ifndef DATAMANAGER_SWVERSIONLISTVERIFIER_H
#define DATAMANAGER_SWVERSIONLISTVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/SWVersions/Include/SWVersionList.h"

//lint -e1540

namespace DataManager {



/****************************************************************************/
/*!
 *  \brief CSWVersionListVerifier class Inherited from IVerifierInterface
 */
/****************************************************************************/
class CSWVersionListVerifier : public IVerifierInterface
{
public:
    CSWVersionListVerifier();

    bool VerifyData(CDataContainerBase* p_Configuration);  // use concrete class for concrete verifier

    ErrorHash_t &GetErrors();

    void ResetLastErrors();

    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CSWVersionListVerifier() {}
private:
    CSWVersionList* mp_SWVL;              //!< SWVersionList Data Container
    ErrorHash_t m_ErrorsHash;            //!< To store Error ID and any arguments associated
};

} // namespace ColoradoDataContainer

#endif // DATAMANAGER_SWVERSIONLISTVERIFIER_H
