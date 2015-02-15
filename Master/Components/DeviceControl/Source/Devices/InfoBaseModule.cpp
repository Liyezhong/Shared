/****************************************************************************/
/*! \file   InfoBaseModule.cpp
 *
 *  \brief  Implementaion file for class CInfoBaseModule
 *
 *  \version  0.1
 *  \date     2012-01-07
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DeviceControl/Include/Devices/InfoBaseModule.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoBaseModule
typedef CSignalTransition<CInfoBaseModule> CInfoBaseModuleTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoBaseModule
 *
 *  \iparam p_BaseModule = Base module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoBaseModule::CInfoBaseModule(CBaseModule *p_BaseModule, DataManager::CSubModule *p_SubModule, QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_BaseModule(p_BaseModule), mp_SubModule(p_SubModule)
{
    //lint -esym(1524, CInfoBaseModule)
    CState *p_Init = new CState("Init", this);
   // CState *p_ReqSerialNumber = new CState("ReqSerialNumber", this);
    CState *p_ReqEndTestResult = new CState("ReqEndTestResult", this);
    CState *p_ReqHWInfo = new CState("ReqHWInfo", this);
    CState *p_ReqSWInfo = new CState("ReqSWInfo", this);
    CState *p_ReqLoaderInfo = new CState("ReqLoaderInfo", this);
    CState *p_ReqLifeCycleData = new CState("ReqLifeCycleData", this);
    CState *p_ReqLaunchDate = new CState("ReqLaunchDate", this);
    CState *p_ReqBoardName = new CState("ReqBoardName", this);
    CState *p_ReqUniqueNumber = new CState("ReqUniqueNumber", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(this, SIGNAL(Skip()), p_Final);

    /*p_Init->addTransition(new CInfoBaseModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoBaseModule::ReqSerialNumber,
        p_ReqSerialNumber));

    p_ReqSerialNumber->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportSerialNumber(quint32, ReturnCode_t, QString)),
        *this, &CInfoBaseModule::ReqEndTestResult,
        p_ReqEndTestResult));*/

    p_Init->addTransition(new CInfoBaseModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoBaseModule::ReqEndTestResult,
        p_ReqEndTestResult));

    p_ReqEndTestResult->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportEndTestResult(quint32, ReturnCode_t, TestResult_t, QDate)),
        *this, &CInfoBaseModule::ReqHWInfo,
        p_ReqHWInfo));

    p_ReqHWInfo->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportHWInfo(quint32, ReturnCode_t, quint8, quint8 ,QDate)),
        *this, &CInfoBaseModule::ReqSWInfo,
        p_ReqSWInfo));
    p_ReqSWInfo->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportSWInfo(quint32, ReturnCode_t, quint16, QDate)),
        *this, &CInfoBaseModule::ReqLoaderInfo,
        p_ReqLoaderInfo));
    p_ReqLoaderInfo->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportLoaderInfo(quint32, ReturnCode_t, quint8, quint8, QDate)),
        *this, &CInfoBaseModule::ReqLifeCycleData,
        p_ReqLifeCycleData));
    p_ReqLifeCycleData->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportLifeCycleData(quint32, ReturnCode_t, quint32, quint16)),
        *this, &CInfoBaseModule::ReqLaunchDate,
        p_ReqLaunchDate));
    p_ReqLaunchDate->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportLaunchDate(quint32, ReturnCode_t, bool, QDate)),
        *this, &CInfoBaseModule::ReqBoardName,
        p_ReqBoardName));
    p_ReqBoardName->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportBoardName(quint32, ReturnCode_t, QString)),
        *this, &CInfoBaseModule::ReqUniqueNumber,
        p_ReqUniqueNumber));
    p_ReqUniqueNumber->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportUniqueNumber(quint32, ReturnCode_t, QByteArray)),
        *this, &CInfoBaseModule::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("SerialNumber", QString());
    mp_SubModule->AddParameterInfo("EndTestResult", QString());
    mp_SubModule->AddParameterInfo("EndTestDate", QString());
    mp_SubModule->AddParameterInfo("HardwareMajorVersion", QString());
    mp_SubModule->AddParameterInfo("HardwareMinorVersion", QString());
    mp_SubModule->AddParameterInfo("HardwareProductionDate", QString());
    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("SoftwareReleaseDate", QString());
    mp_SubModule->AddParameterInfo("BootLoaderMajorVersion", QString());
    mp_SubModule->AddParameterInfo("BootLoaderMinorVersion", QString());
    mp_SubModule->AddParameterInfo("BootLoaderReleaseDate", QString());
    mp_SubModule->AddParameterInfo("OperationTime", "minutes", QString());
    mp_SubModule->AddParameterInfo("StartUpCycles", QString());
    mp_SubModule->AddParameterInfo("LaunchedFlag", QString());
    mp_SubModule->AddParameterInfo("FirstLaunchDate", QString());
    mp_SubModule->AddParameterInfo("BoardName", QString());
    mp_SubModule->AddParameterInfo("UniqueNumber", QString());
}

/****************************************************************************/
/*!
 *  \brief  Requests the serial number from the base module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqSerialNumber(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode;

    if(mp_BaseModule->GetMainState() == CBaseModule::CN_MAIN_STATE_INACTIVE) {
        emit Skip();
        return false;
    }
    ReturnCode = mp_BaseModule->ReqSerialNumber();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the end test result from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportSerialNumber
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqEndTestResult(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    /*QString SerialNumber;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, SerialNumber)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SerialNumber", SerialNumber)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }*/

    Q_UNUSED(p_Event);

    ReturnCode = mp_BaseModule->ReqEndTestResult();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the hardware information from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportEndTestResult
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqHWInfo(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    TestResult_t TestResult;
    QDate Date;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, TestResult)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 3, Date)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("EndTestResult", QString().setNum(TestResult))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("EndTestDate", Date.toString())) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqHWInfo();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the software information from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportHWInfo
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqSWInfo(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint8 VersionMajor;
    quint8 VersionMinor;
    QDate Date;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, VersionMajor)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 3, VersionMinor)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 4, Date)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("HardwareMajorVersion", QString().setNum(VersionMajor))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("HardwareMinorVersion", QString().setNum(VersionMajor))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("HardwareProductionDate", Date.toString())) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqSWInfo();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the loader information from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportSWInfo
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqLoaderInfo(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint16 Version;
    QDate Date;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 3, Date)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", QString().setNum(Version))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("SoftwareReleaseDate", Date.toString())) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqLoaderInfo();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the life cycle data from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportLoaderInfo
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqLifeCycleData(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint8 VersionMajor;
    quint8 VersionMinor;
    QDate Date;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, VersionMajor)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 3, VersionMinor)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 4, Date)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("BootLoaderMajorVersion", QString().setNum(VersionMajor))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("BootLoaderMinorVersion", QString().setNum(VersionMajor))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("BootLoaderReleaseDate", Date.toString())) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqLifeCycleData();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the launch date from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportLifeCycleData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqLaunchDate(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint32 OperationTime;
    quint16 StartupCycles;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, OperationTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 3, StartupCycles)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("OperationTime", QString().setNum(OperationTime))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("StartUpCycles", QString().setNum(StartupCycles))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqLaunchDate();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the board name from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportLaunchDate
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqBoardName(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    bool Launched;
    QDate Date;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, Launched)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 3, Date)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("LaunchedFlag", QString().setNum(Launched))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("FirstLaunchDate", Date.toString())) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqBoardName();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the unique number from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportBoardName
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqUniqueNumber(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    QString BoardName;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, BoardName)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("BoardName", BoardName)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode = mp_BaseModule->ReqUniqueNumber();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Transition to the final state
 *
 *  \iparam p_Event = Parameters of the signal ReportUniqueNumber
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::Finished(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    QByteArray UniqueNumber;

    ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoBaseModuleTransition::GetEventValue(p_Event, 2, UniqueNumber)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    QString UniqueNumberStr;
    for (quint8 i = 0; i < UniqueNumber.size(); i++) {
        QString Digit = QString("%1").arg((qint32)UniqueNumber.at(i), 2, 16, QChar('0'));
        UniqueNumberStr.append(Digit.right(2));
    }

    if (!mp_SubModule->UpdateParameterInfo("UniqueNumber", UniqueNumberStr)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
