/****************************************************************************/
/*! \file InstrumentHistoryVerifier.cpp
 *
 *  \brief InstrumentHistoryVerifier class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-08
 *   $Author:  $ Soumya D, Shuvasmita
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

#include "DataManager/Containers/InstrumentHistory/Include/InstrumentHistoryVerifier.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/GlobalEventCodes.h"
#include <Global/Include/EventObject.h>
//lint -e613

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CInstrumentHistoryVerifier::CInstrumentHistoryVerifier(): mp_MDL(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief Verifies the data present in the module list against constraints.
 *  \iparam p_InstrumentHistory
 *  \return true - verify success , false - verify failure
 */
/****************************************************************************/
bool CInstrumentHistoryVerifier::VerifyData(const CDataContainerBase *p_InstrumentHistory)
{
    bool VerifiedData = true;

    mp_MDL = static_cast<CInstrumentHistory*>(const_cast<CDataContainerBase*>(p_InstrumentHistory));

    if(mp_MDL == NULL) {
        qDebug() << "Module List Data container is empty" << endl;
        VerifiedData = false;
        return VerifiedData;
    }

    QString InstrumentName = mp_MDL->GetInstrumentName();
    if(!InstrumentName.startsWith("SPECTRA")) {
        qDebug() << "Instrument Does not Exists" << endl;
        VerifiedData = false;
    }

    //!< Check the content of each Module
    CModule *p_Module = new CModule();
    QStringList ModuleList;
    ModuleList << "Drawer Left" << "Drawer Right" << "Oven" << "YZ_Arm_Left" << "YZ_Arm_Right"
               << "X-Arm" << "Agitator" << "Transfer" << "E Box" << "SlideID" << "HeatedCuvetts"
               << "Water" << "Exhaust" << "Display" << "Cover" << "JoyStick" << "RFID Consumables";


    for ( qint32 Count=0; Count<mp_MDL->GetNumberOfModuleLists(); Count++) {

        CModuleDataList *p_ModuleDataList = new CModuleDataList();
        *p_ModuleDataList = *mp_MDL->GetModuleList(Count);
        if (p_ModuleDataList)
        {

            for(qint32 i=0; i<p_ModuleDataList->GetNumberofModules(); i++)
            {
                *p_Module = *p_ModuleDataList->GetModule(i);

                if (p_Module) {

                    QString ModuleName = p_Module->GetModuleName();

                    if(!ModuleList.contains(ModuleName, Qt::CaseSensitive)) {
                        qDebug() << "Unkown Module Name" << endl;
                        /*Global::EventObject::Instance().RaiseEvent(EVENT_DM_INSTR_UNKNOWN_MODULE_NAME,
                                                                   Global::tTranslatableStringList()
                                                                   << p_Module->GetModuleName(),
                                                                   true, Global::GUI_MSG_BOX);
                                                                   */
                        VerifiedData = false;
                    }
                }
            }

            CSubModule *p_SubModule = new CSubModule();
            for(qint32 j=0; j<p_Module->GetNumberofSubModules(); j++)
            {
                *p_SubModule = *p_Module->GetSubModuleInfo(j);

                if(!p_SubModule) {
                    qDebug() << "Sub Module Data does not Exist" << endl;
                    /*Global::EventObject::Instance().RaiseEvent(EVENT_DM_INSTR_UNKNOWN_SUB_MODULE_DATA,
                                                               Global::tTranslatableStringList()
                                                               << "", true, Global::GUI_MSG_BOX);*/
                    VerifiedData = false;
                }
            }

            delete p_SubModule;
            delete p_Module;
        }

        delete p_ModuleDataList;
    }

    return VerifiedData;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CInstrumentHistoryVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CInstrumentHistoryVerifier::ResetErrors()
{
    m_ErrorMap.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 *  \return true
 */
/****************************************************************************/
bool CInstrumentHistoryVerifier::IsLocalVerifier() const
{
    return true;
}

}
