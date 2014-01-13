/****************************************************************************/
/*! \file DataContainer.h
 *
 *  \brief Definition file for class CDataContainer.
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

#ifndef DATAMANAGER_DATACONTAINER_H
#define DATAMANAGER_DATACONTAINER_H

#include <QString>
#include <QIODevice>

#include "HimalayaDataManager/Containers/Reagents/Include/DataReagentList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "TestStubDataManager/Containers/Racks/Include/RackList.h"

namespace DataManager {
//!<  List containing macro steps and normal program steps
typedef QList<CProgramStep*> ListOfExpandedSteps_t;
//!< Self explaining
//static const QString SLIDE_ID_SPECIAL_REAGENT = "S2";

//!< Self explaining
//static const QString MAX_DURATION_PERCENTAGE_DRY_STATION_STEP = "11900%";
//!< Self explaining
//static const QString MAX_DURATION_PERCENTAGE_SID_STATION_STEP = "5900%";

class CDataContainer: public QObject
{
    Q_OBJECT
private:
    ListOfExpandedSteps_t m_ExpandedStepList;   //!< Normal steps + Macro steps
    bool m_IsInitialized;

    bool InitContainers();
    bool DeinitContainers();

    bool ResetDCProgramList();
    bool ResetDCReagentList();
    bool ResetDCStationList();
    bool ResetDCUserSettings();
    bool ResetDCProgramSequenceList();
    bool ResetDCRackList();
    bool AddStepsToExpandedStepList(CProgram &Program);
    bool CompareSteps(CProgramStep& CurrentProgramStep, CProgramStep& NextProgramStep, bool& OK);
public:
    CDataContainer();
    ~CDataContainer();

    bool RefreshProgramStepStationlist();

    /* Methods to handle expanded step list */
    bool RefreshExpandedStepList(QString ProgramID);

    /****************************************************************************/
    /*!
     *  \brief Retrieves the expanded steplist
     *
     *  \return Expanded steplist
     *  \todo Currently returning step list count. change to expanded list count
     *        when macro's are added.
     */
    /****************************************************************************/
    int GetNumberOfStepsInExpandedList() {return m_ExpandedStepList.count();}

    bool GetProgramStepExpanded(const unsigned int Index, const CProgram* p_Program, CProgramStep* p_ProgStep);

    // praefix mp_ left because members are intentially accessible from outside (public)
    CDataProgramList* ProgramList;  //!< Container for programs
    CDataReagentList* ReagentList;  //!< Container for reagents
    CDataStationList* StationList;  //!< Container for stations
    CRackList* RackList;        //!< Container for racks
    CProgramSequenceList* ProgramSequenceList;  //!< Container for additional program information
    CUserSettings* Settings;         //!< Container for user settings
    CDataStationMatrix* StationMatrix;
};

} // namespace DataManager

#endif // DATAMANAGER_DATACONTAINER_H

