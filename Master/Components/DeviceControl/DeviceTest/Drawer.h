#ifndef DEVICECONTROL_DRAWER_H
#define DEVICECONTROL_DRAWER_H

#include "Device.h"

namespace DeviceControl
{

class CDrawer : public CDevice
{
    Q_OBJECT

public:
    CDrawer(CMockFm *fm);

signals:
    // command request interface to DCP
    void ReadRackRfid(LoaderRFIDChannel_t Channel);
    void Block();
    void Unblock();


    // command response interface to DCP
    void ReportReadRackRfid(ReturnCode_t ReturnCode, LoaderRFIDChannel_t Channel, quint32 Uid, quint32 Data);
    void ReportBlock(ReturnCode_t ReturnCode);
    void ReportUnblock(ReturnCode_t ReturnCode);


protected:
    bool DefaultNackReadRackRfid(QEvent *p_Event);
    bool DefaultNackBlock(QEvent *p_Event);
    bool DefaultNackUnblock(QEvent *p_Event);
    
};

} //namespace

#endif /* DEVICECONTROL_DRAWER_H */

// vi: set ts=4 sw=4 et:

