/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/CommandInterface/Include/CommandInterfaceBase.h
 *
 *  \brief Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath, Ramya GJ
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
#include <DataManager/Include/DataManagerBase.h>
#include <Threads/Include/MasterThreadController.h>
#include "Threads/Include/CommandChannel.h"
#include "Global/Include/UITranslator.h"

namespace DataManager {
/****************************************************************************/
/**
 * \brief Base class for all DataManager Command Interfaces
 */
/****************************************************************************/
class CCommandInterfaceBase : public QObject
{
    Q_OBJECT
    friend class TestCmdInterface;
public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *  \iparam p_DataManager = Datamanager object
     *  \iparam p_HimalayaMasterThreadController = Himalaya Master thread Cntrl
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
    virtual ~CCommandInterfaceBase()
    {
        mp_DataManager = NULL;
        mp_MasterThreadController = NULL;
        mp_DataContainer = NULL;
    }

protected:
    //Pointer section
    CDataManagerBase                             *mp_DataManager;                     //!< Self Explaining
    Threads::MasterThreadController              *mp_MasterThreadController;          //!< Will be used to call BroadCastCommand
    CDataContainerCollectionBase                 *mp_DataContainer;                   //!< Provides access to various containers

    /****************************************************************************/
    /*!
     *  \brief Provides functionality to SendNack to GUI
     *  \iparam Ref = Command reference
     *  \iparam AckCommandChannel = Command Channel to send Ack
     *  \iparam ErrorList = List of errors occured
     *  \iparam EventCode = EventCode to send for Default message
     */
    /****************************************************************************/
    void SendNackToGUI(Global::tRefType Ref, Threads::CommandChannel &AckCommandChannel, ListOfErrors_t &ErrorList, quint32 EventCode)
    {
        QString ErrorString;
        //Error List was not generated, we inform user that update was not possible
        if (ErrorList.isEmpty()) {
            ErrorString = Global::UITranslator::TranslatorInstance().
                    Translate(Global::TranslatableString(EventCode,
                                                         Global::tTranslatableStringList()));
            Global::EventObject::Instance().RaiseEvent(EventCode);
        }
        else {
            DataManager::Helper::ErrorIDToString(ErrorList, ErrorString);
            if (ErrorString.isEmpty()) {
                ErrorString = Global::UITranslator::TranslatorInstance().
                        Translate(Global::TranslatableString(EventCode,
                                                             Global::tTranslatableStringList()));
            }
        }
        if (mp_MasterThreadController) {
            mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Provides functionality to SendNack to GUI
     *  \iparam Ref = Command reference
     *  \iparam AckCommandChannel = Command Channel to send Ack
     *  \iparam EventCode = EventCode to send for Default message
     */
    /****************************************************************************/
    void SendNackToGUIWithDefaultMsg(Global::tRefType Ref, Threads::CommandChannel &AckCommandChannel, quint32 EventCode)
    {
        //Error List was not generated, we inform user that update was not possible
        QString ErrorString(Global::UITranslator::TranslatorInstance().
                            Translate(Global::TranslatableString(EventCode, Global::tTranslatableStringList())));
        //Logs the event
        Global::EventObject::Instance().RaiseEvent(EventCode);
        if (mp_MasterThreadController) {
            mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Provides functionality to SendNack to GUI
     *  \iparam Ref = Command reference
     *  \iparam AckCommandChannel = Command Channel to send Ack
     *  \iparam Command = Command required to update GUI
     */
    /****************************************************************************/
    void SendAckAndUpdateGUI(Global::tRefType Ref, Threads::CommandChannel &AckCommandChannel, const Global::CommandShPtr_t &Command)
    {
        if (mp_MasterThreadController) {
            mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
            (void)mp_MasterThreadController->SendCommand(Command, AckCommandChannel); //avoid lint 534
        }
    }
};

}// end of namespace DataManager

#endif // COMMANDINTERFACEBASE_H
