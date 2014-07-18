/****************************************************************************/
/*! \file   ArchiveServiceInforState.h
 *
 *  \brief  Definition file for class CArchiveServiceInforState
 *
 *  \version  0.1
 *  \date     2014-07-012
 *  \author   A.Yang
 *
 *  \b Description:
 *
 *   N/A
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2014 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DEVICECONTROL_ARCHIVESERVICEINFORSTATE_H
#define DEVICECONTROL_ARCHIVESERVICEINFORSTATE_H

#include "DeviceState.h"




namespace DeviceControl
{

class CBaseDevice;

/****************************************************************************/
/*!
 *  \brief  Archive the service information for every device
 */
/****************************************************************************/
class CArchiveServiceInforState : public CState
{
    Q_OBJECT

public:
    explicit CArchiveServiceInforState(CBaseDevice *pBaseDevice, QState *p_Parent = 0);

signals:
private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CArchiveServiceInforState)

    bool EmitGetServiceInfor(QEvent *p_Event);
    bool Finished(QEvent *p_Event);

    CBaseDevice *m_pBaseDevice;       //!< 

};

} //namespace

#endif // DEVICECONTROL_ARCHIVESERVICEINFORSTATE_H
