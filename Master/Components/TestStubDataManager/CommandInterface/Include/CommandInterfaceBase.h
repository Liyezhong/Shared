/****************************************************************************/
/*! \file CommandInterfaceBase.h
 *
 *  \brief Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath
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
#ifndef COMMANDINTERFACEBASE_H
#define COMMANDINTERFACEBASE_H

#include <QObject>
#include <TestStubDataManager/Include/DataManagerBase.h>
#include "../../../../../../Colorado/ColoradoMain/Gui/GuiStub/Components/Threads/Include/MasterThreadController.h"
#include "../../../../../../Colorado/ColoradoMain/Gui/GuiStub/Components/Threads/Include/CommandChannel.h"
#include "TestStubDataManager/Include/DataManagerBase.h"

namespace DataManager {
/****************************************************************************/
/**
 * \brief Base class for all DataManager Command Interfaces
 */
/****************************************************************************/
class CCommandInterfaceBase : public QObject
{
    Q_OBJECT
public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *  \iparam p_DataManager = Datamanager object
     *  \iparam p_ColoradoMasterThreadController = Colorado Master thread Cntrl
     *  \iparam p_DataContainer = DataContainer pointer
     */
    /****************************************************************************/
    explicit CCommandInterfaceBase(CDataManagerBase *p_DataManager,
                                   Threads::MasterThreadController *p_MasterThreadController,
                                   CDataContainerCollectionBase *p_DataContainer):
        QObject(0),
        mp_DataManager(p_DataManager),
        mp_MasterThreadController(p_MasterThreadController),
        mp_DataContainer(p_DataContainer)
    {
    }
    /****************************************************************************/
    /*!
     *  \brief Destructor
     */
    /****************************************************************************/
    virtual ~CCommandInterfaceBase(){}

protected:
    //Pointer section
    CDataManagerBase                             *mp_DataManager;                     //!< Self Explaining
    Threads::MasterThreadController       *mp_MasterThreadController; //!< Will be used to call BroadCastCommand
    CDataContainerCollectionBase                           *mp_DataContainer;                  //!< Provides access to various containers
    virtual void RegisterCommands() = 0;
    virtual void UnRegisterCommands(){}
};

}// end of namespace DataManager

#endif // COMMANDINTERFACEBASE_H
