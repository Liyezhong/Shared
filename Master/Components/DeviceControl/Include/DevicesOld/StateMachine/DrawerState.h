#ifndef DEVICECONTROL_DRAWERSTATE_H
#define DEVICECONTROL_DRAWERSTATE_H

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CDigitalOutput;
class CDrawer;

/****************************************************************************/
class CDrawerState
{
public:
    virtual ~CDrawerState() {}
    virtual void Initialize(CDrawer *) {}
    virtual void OnReportInitialize(CDrawer *) {}
    virtual void SetBlockState (CDrawer *, CDigitalOutput *, CDigitalOutput *, BlockState_t) {}
    virtual void OnReportSetBlockState (CDrawer *, ReturnCode_t) {}
    virtual void Stop(CDrawer *) {}
    virtual void OnReportStop(CDrawer *) {}
    virtual void ReadRfid(CDrawer *) {}
    virtual void OnReadRfid(CDrawer *) {}
protected:
    void ChangeState(CDrawer *p_Context, CDrawerState *p_State);
};

/****************************************************************************/
class CBlockedState : public CDrawerState
{
public:
    void SetBlockState (CDrawer *p_Context, CDigitalOutput *p_LedBlocked,
                        CDigitalOutput *p_LedFree, BlockState_t BlockState);
    static CDrawerState *Instance() { return &m_Instance; }

private:
    static CBlockedState m_Instance;
};

/****************************************************************************/
class CUnblockingState : public CDrawerState
{
public:
    void OnReportSetBlockState (CDrawer *p_Context, ReturnCode_t ReturnCode);
    static CDrawerState *Instance() { return &m_Instance; }

private:
    static CUnblockingState m_Instance;
};

/****************************************************************************/
class CUnblockedState : public CDrawerState
{
public:
    void SetBlockState (CDrawer *p_Context, CDigitalOutput *p_LedBlocked,
                        CDigitalOutput *p_LedFree, BlockState_t BlockState);
    static CDrawerState *Instance() { return &m_Instance; }

private:
    static CUnblockedState m_Instance;
};

/****************************************************************************/
class CBlockingState : public CDrawerState
{
public:
    void OnReportSetBlockState (CDrawer *p_Context, ReturnCode_t ReturnCode);
    static CDrawerState *Instance() { return &m_Instance; }

private:
    static CBlockingState m_Instance;
};

} //namespace

#endif /* DEVICECONTROL_DRAWERSTATE_H */
