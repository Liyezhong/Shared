/****************************************************************************/
/*! \file DataContainerCollectionBase.h
 *
 *  \brief Definition file for class CDataContainerCollectionBase.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-19
 *  $Author:    $ Michael Thiel
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

#ifndef DATAMANAGER_DATACONTAINERCOLLECTIONBASE_H
#define DATAMANAGER_DATACONTAINERCOLLECTIONBASE_H

#include <QString>
#include <QIODevice>

#include "TestStubDataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"

namespace Threads {
    class MasterThreadController;
}
namespace DataManager {

class CDataContainerCollectionBase: public QObject
{
    Q_OBJECT
private:
    bool ResetDCUserSettings();
    bool ResetDCDeviceConfiguration();

protected:
    bool m_IsInitialized;
    virtual bool InitContainers();
    virtual bool DeinitContainers();

public:
    CDataContainerCollectionBase(Threads::MasterThreadController *p_MasterThreadController);
    virtual ~CDataContainerCollectionBase();

    Threads::MasterThreadController *mp_MasterThreardController; //!< We use it for  broadcasting command

    // prefix mp_ left because members are intentially accessible from outside (public)
    CUserSettingsInterface* SettingsInterface; //!< Provides interface to read the Settings info from xml
    CDeviceConfigurationInterface* DeviceConfigurationInterface; //!< Container for Device configuration
};

} // namespace DataManager

#endif // DATAMANAGER_DATACONTAINERCOLLECTIONBASE_H

