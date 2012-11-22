#ifndef DEVICECONTROL_MOVEENDTOENDSTATE_H
#define DEVICECONTROL_MOVEENDTOENDSTATE_H

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CMoveEndToEnd;
class CStepperMotor;

/****************************************************************************/
class CMoveEndToEndState
{
public:
    virtual ~CMoveEndToEndState() {}
    virtual void DoReferenceRun(CMoveEndToEnd *, CStepperMotor *) {}
    virtual void MoveToPosition1(CMoveEndToEnd *, CStepperMotor *) {}
    virtual void MoveToPosition2(CMoveEndToEnd *, CStepperMotor *) {}
    virtual void OnReportReferenceMovementAckn(CMoveEndToEnd *, ReturnCode_t) {}
    virtual void OnReportMovementAckn(CMoveEndToEnd *, ReturnCode_t) {}

protected:
    void ChangeState(CMoveEndToEnd *p_Context, CMoveEndToEndState *p_State);
};

/****************************************************************************/
class CUninitializedState : public CMoveEndToEndState
{
public:
    void DoReferenceRun(CMoveEndToEnd *p_Context, CStepperMotor *p_StepperMotor);
    static CMoveEndToEndState *Instance() { return &m_Instance; }

private:
    static CUninitializedState m_Instance;
};

/****************************************************************************/
class CInitializingState : public CMoveEndToEndState
{
public:
    void OnReportReferenceMovementAckn(CMoveEndToEnd *p_Context, ReturnCode_t ReturnCode);
    static CMoveEndToEndState *Instance() { return &m_Instance; }

private:
    static CInitializingState m_Instance;
};

/****************************************************************************/
class CPosition1State : public CMoveEndToEndState
{
public:
    void MoveToPosition2(CMoveEndToEnd *p_Context, CStepperMotor *p_StepperMotor);
    static CMoveEndToEndState *Instance() { return &m_Instance; }

private:
    static CPosition1State m_Instance;
};

/****************************************************************************/
class CMoveToPosition2State : public CMoveEndToEndState
{
public:
    void OnReportMovementAckn(CMoveEndToEnd *p_Context, ReturnCode_t ReturnCode);
    static CMoveEndToEndState *Instance() { return &m_Instance; }

private:
    static CMoveToPosition2State m_Instance;
};

/****************************************************************************/
class CPosition2State : public CMoveEndToEndState
{
public:
    void MoveToPosition1(CMoveEndToEnd *p_Context, CStepperMotor *p_StepperMotor);
    static CMoveEndToEndState *Instance() { return &m_Instance; }

private:
    static CPosition2State m_Instance;
};

/****************************************************************************/
class CMoveToPosition1State : public CMoveEndToEndState
{
public:
    void OnReportMovementAckn(CMoveEndToEnd *p_Context, ReturnCode_t ReturnCode);
    static CMoveEndToEndState *Instance() { return &m_Instance; }

private:
    static CMoveToPosition1State m_Instance;
};

} //namespace

#endif /* DEVICECONTROL_MOVEENDTOENDSTATE_H */
