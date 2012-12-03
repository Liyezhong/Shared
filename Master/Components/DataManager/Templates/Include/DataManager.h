/****************************************************************************/
/*! \file DataManager.h
 *
 *  \brief Definition file for class CDataManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth
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

#ifndef DATAMANAGER_DATAMANAGER_H
#define DATAMANAGER_DATAMANAGER_H

#include <QString>
#include <QIODevice>

#include "../Include/DataContainer.h"

namespace DataManager {

class CDataManager
{
private:
    bool m_IsInitialized;
    CDataContainer* mp_DataContainer;

    bool InitDataContainer();
    bool DeinitDataContainer();

public:
    CDataManager();
    ~CDataManager();


    //********************************************************************
    //some function to access the program list
    //********************************************************************

    //get copy of complete program list
    bool GetProgramList(CDataProgramList* ProgramListCopy);

    //delete program with given ID
    bool DeleteProgram(QString ID);

    //update program
    bool UpdateProgram(CProgram* p_Program);

    // write program list to file
    bool WriteProgramList(QString Filename);

    // de-/serialize complete program list
    bool SerializeProgramList(QByteArray* p_ByteArray);
    bool DeserializeProgramList(QByteArray* p_ByteArray);

    //********************************************************************
    //some function to access the reagent list
    //********************************************************************

    // de-/serialize complete reagent list
    bool SerializeReagentList(QByteArray* p_ByteArray);
    bool DeserializeReagentList(QByteArray* p_ByteArray);

    //get copy of complete reagent list
    bool GetReagentList(CDataReagentList* ReagentListCopy);

    //delete reagent with given ID
    bool DeleteReagent(QString ID);

    //update reagent
    bool UpdateReagent(CReagent* p_Reagent);
    bool UpdateReagent(QByteArray* p_ByteArray);

    // write reagent list to file
    bool WriteReagentList(QString Filename);

    //********************************************************************
    //some function to access the station list
    //********************************************************************
    bool GetStationList(CDataStationList* StationListCopy);
    bool DeleteStation(QString ID);
    bool UpdateStation(CStation* p_Station);
    bool UpdateStation(QByteArray* p_ByteArray);
    bool SerializeStationList(QByteArray* p_ByteArray);
    bool DeserializeStationList(QByteArray* p_ByteArray);
    bool WriteStationList(QString Filename);
};

} // namespace DataManager

#endif // DATAMANAGER_DATAMANAGER_H


