/****************************************************************************/
/*! \file ServiceConfiguration.cpp
 *
 *  \brief Definition file for class CServiceConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-25
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
#ifndef SERVICECONFIGURATION_H
#define SERVICECONFIGURATION_H


#include "QXmlStreamWriter"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ExportConfiguration/Include/ConfigurationList.h"


namespace DataManager {
/******************************************************************************/
/*!
 *  \brief  This class implements service configuration for the Export component
 */
/******************************************************************************/
class CServiceConfiguration
{
    friend class CExportConfiguration;
private:
    CConfigurationList m_ServiceConfigList; ///< to store the service configuration list

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

public:
    CServiceConfiguration();

    CServiceConfiguration(const CServiceConfiguration&);

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CServiceConfiguration& ExportConfiguration);
    friend QDataStream& operator >>(QDataStream& InDataStream, CServiceConfiguration& ExportConfiguration);
    CServiceConfiguration& operator=(const CServiceConfiguration&);

    /****************************************************************************/
    /*!
     *  \brief Retrieve the service configuration
     *  \return configuration class
     */
    /****************************************************************************/
    CConfigurationList GetServiceConfigurationList() { return m_ServiceConfigList;}

    /****************************************************************************/
    /*!
     *  \brief Sets the service configuration
     *
     *  \iparam ServiceList = Configuration of the service
     */
    /****************************************************************************/
    void SetServiceConfigurationList(CConfigurationList ServiceList) { m_ServiceConfigList = ServiceList;}

};
}

#endif // SERVICECONFIGURATION_H
