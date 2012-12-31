/****************************************************************************/
/*! \file ExportConfigurationVerifier.h
 *
 *  \brief Definition file for class CExportConfigurationVerifier.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-06
 *  $Author:    $ Aparna AL
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

#ifndef DATAMANAGER_EXPORTCONFIGURATIONVERIFIER_H
#define DATAMANAGER_EXPORTCONFIGURATIONVERIFIER_H

#include <QString>
#include <QXmlStreamReader>

#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfiguration.h"

//lint -e1540
namespace DataManager {



/****************************************************************************/
/*!
 *  \brief CExportConfigurationVerifier class Inherited from IVerifierInterface
 */
/****************************************************************************/
class CExportConfigurationVerifier : public IVerifierInterface
{
public:
    CExportConfigurationVerifier();

    bool VerifyData(CDataContainerBase* p_Configuration);  // use concrete class for concrete verifier

    ErrorHash_t &GetErrors();

    void ResetLastErrors();

    bool IsLocalVerifier();

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CExportConfigurationVerifier() {}
private:
    CExportConfiguration* mp_ECD;    //!< ExportConfiguration Data Container
    ErrorHash_t m_ErrorsHash;          //!< To store Error ID and any arguments associated

    bool CheckFileNames(CConfigurationList Configuration);

};

} // namespace DataManager

#endif // DATAMANAGER_EXPORTCONFIGURATIONVERIFIER_H
