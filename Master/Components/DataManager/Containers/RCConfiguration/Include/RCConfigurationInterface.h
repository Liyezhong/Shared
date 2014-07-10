/****************************************************************************/
/*! \file Components/DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h
 *
 *  \brief RCConfigurationInterface definition.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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
#ifndef DATAMANAGER_RCCONFIGURATIONINTERFACE_H
#define DATAMANAGER_RCCONFIGURATIONINTERFACE_H

#include <QString>
#include <QReadWriteLock>
#include <QLocale>
#include <QXmlStreamReader>

#include "DataManager/Containers/RCConfiguration/Include/RCConfiguration.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"

//lint -sem(DataManager::CRCConfigurationInterface::SetDefaultAttributes,initializer)
//lint -sem(DataManager::CRCConfigurationInterface::UpdateRCConfiguration, custodial(1))

namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for remote care.
 *
 * <b>This class is not thread safe.</b>
 */
/****************************************************************************/
class CRCConfigurationInterface : public CDataContainerBase
{
private:
    QReadWriteLock*             mp_ReadWriteLock;       //!< Synchronisation object.
    bool                        m_DataVerificationMode; //!< Store the Date verification mode flag
    CRCConfiguration*           mp_RCConfiguration;        //!< Store the user settings
    QString                     m_FileName;             //!< Store the file name
    /****************************************************************************/


    bool SerializeContent(QIODevice& IODevice, bool CompleteData);
    bool DeserializeContent(QIODevice& IODevice, bool CompleteData);
    ErrorMap_t m_ErrorMap;  //!< Event List for GUI. This member is not copied when using copy constructor/Assignment operator

protected:
public:

    CRCConfigurationInterface();
    CRCConfigurationInterface(const CRCConfigurationInterface &);
    void CopyFromOther(const CRCConfigurationInterface &Other);

    ~CRCConfigurationInterface();

    friend QDataStream& operator <<(const QDataStream& OutDataStream, const CRCConfigurationInterface& RCInterface);
    friend QDataStream& operator >>(const QDataStream& InDataStream, const CRCConfigurationInterface& RCInterface);

    CRCConfigurationInterface & operator = (const CRCConfigurationInterface &);

    /****************************************************************************/
    /*!
     *  \brief Get the container type
     *
     *  \return True or False
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() { return RCCONFIGURATION; }

    /****************************************************************************/
    /*!
     *  \brief Get the Data verification mode flag
     *
     *  \return True or False
     */
    /****************************************************************************/
    bool GetDataVerificationMode() { return m_DataVerificationMode; }

    /****************************************************************************/
    /*!
     *  \brief Set the Data verification mode flag value
     *
     *  \iparam Value = flag value
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value) { m_DataVerificationMode = Value; }

    bool Read(const QString& FileName);

    bool UpdateRCConfiguration(const CRCConfiguration* p_RCConfiguration);

    CRCConfiguration* GetRCConfiguration(const bool& CopySettings);

    /****************************************************************************/
    /*!
     *  \brief Get the RC config (shallow copy)
     *
     *  \return RC config class
     */
    /****************************************************************************/
    CRCConfiguration* GetRCConfiguration() const
    {
        if (mp_RCConfiguration) {
            return mp_RCConfiguration;
        }
        else {
            return NULL;
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the version of The RC config File
     *
     *  \return Version Number
     */
    /****************************************************************************/
    int GetVersion()
    {
        // container we don't store the version instead we write everything in CRCConfiguration class
        // itself. Because RC config is not a container.
        // check whether RC config exists or not
        if (mp_RCConfiguration != NULL) {
            return mp_RCConfiguration->GetVersion();
        }
        return 0;
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the fileName of the RC config
     *
     *  \return File name String
     */
    /****************************************************************************/
    QString GetFilename() {return m_FileName;}


}; // end class CRCConfigurationInterface

} // end namespace DataManager


#endif // DATAMANAGER_RCCONFIGURATIONINTERFACE_H
