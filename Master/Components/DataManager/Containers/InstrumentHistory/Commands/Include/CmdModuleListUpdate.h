/****************************************************************************/
/*! \file CmdModuleListUpdate.h
 *
 *  \brief CmdModuleListUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 14.01.2013
 *   $Author:  $ Soumya. D
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

#ifndef MSGCLASSES_CMDMODULELISTUPDATE_H
#define MSGCLASSES_CMDMODULELISTUPDATE_H

#include "Global/Include/Commands/Command.h"
#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include <QDataStream>


namespace MsgClasses
{

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdModuleListUpdate command.
 */
/****************************************************************************/
class CmdModuleListUpdate : public Global::Command {

public:
    static QString NAME;    //!< Command name.
    QString               m_DeviceType;      //!< Device type
    bool                  m_Error;           //!< true if module data is erroneous
    /**
      * \brief constructor
      * \param Timeout Timeout
      * \param ModuleData Module Data
      * \param DeviceType device type
      * \param Error error
    */
    CmdModuleListUpdate(int Timeout, const DataManager::CModule& ModuleData, const QString& DeviceType, const bool Error = false);

    /**
    * \brief constructor
    */
    CmdModuleListUpdate();
    /**
    * \brief destructor
    */
    ~CmdModuleListUpdate();

    /**
    * \brief function GetName
    * \return QString
    */
    virtual QString GetName() const;

    /****************************************************************************/
    /*!
     *  \brief Returns module data
     *  \return Module object
     */
    /****************************************************************************/
    DataManager::CModule const* GetModuleData() const { return mp_ModuleData; }

    /**
    * \brief get device type
    * \return device type
    */
    const QString& DeviceType() const { return m_DeviceType; }

private:
    DataManager::CModule *mp_ModuleData;     //!< Pointer to Module object

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CmdModuleListUpdate)

}; //!< end class CmdModuleListUpdate

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDMODULELISTUPDATE_H
