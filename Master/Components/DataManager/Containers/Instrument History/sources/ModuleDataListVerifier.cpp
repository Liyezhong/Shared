/****************************************************************************/
/*! \file ModuleDataListVerifier.cpp
 *
 *  \brief ModuleDataListVerifier class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-08
 *   $Author:  $ Soumya D
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

#include "ModuleDataListVerifier.h"

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
ModuleDataListVerifier::ModuleDataListVerifier(): mp_MDL(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief Verifies the data present in the module list against constraints.
 */
/****************************************************************************/
bool ModuleDataListVerifier::VerifyData(CModuleDataList *p_ModuleDataList)
{
    bool VerifiedData = true;

    mp_MDL = static_cast<CModuleDataList*>(p_ModuleDataList);

    if(!mp_MDL) {
        qDebug() << "Module List Data container is empty" << endl;
        VerifiedData = false;
    }

    QString InstrumentName = mp_MDL->GetInstrumentName();
    if(InstrumentName == "ST8200") {
        qDebug() << "Instrument Does not Exists" << endl;
        VerifiedData = false;
    }

    //!< Check the content of each Module
    CModule *p_Module;
    QStringList ModuleList;
    ModuleList << "Drawer left" << "Drawer Right" << "Oven" << "YZ_Arm_Left" << "YZ_Arm_Right"
               << "X-Arm" << "Agitator" << "Transfer" << "E Box" << "SlideID" << "HeatedCuvetts"
               << "Water" << "Exhaust" << "Display" << "Cover" << "JoyStic" << "RFID Consumables";


    for(qint32 i=0; i<mp_MDL->GetNumberofModules(); i++)
    {
        p_Module = mp_MDL->GetModule(i);

        QString ModuleName = p_Module->GetModuleName();

        if(!ModuleList.contains(ModuleName, Qt::CaseSensitive)) {
            qDebug() << "Unkown Module Name" << endl;
            VerifiedData = false;
        }
    }

    CSubModule *p_SubModule;
    for(qint32 j=0; j<p_Module->GetNumberofSubModules(); j++)
    {
        p_SubModule = p_Module->GetSubModuleInfo(j);
        if(!p_SubModule) {
            qDebug() << "Sub Module Data does not Exist" << endl;
            VerifiedData = false;
        }
    }

    return VerifiedData;
}

}
