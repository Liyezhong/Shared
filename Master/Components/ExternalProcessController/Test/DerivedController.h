/****************************************************************************/
/*! \file DerivedController.h
 *
 *  \brief Definition file for class DerivedController.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.02.2011
 *   $Author:  $ Y.Novak
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

#ifndef EXTERNALPROCESSCONTROL_DERIVEDCONTROLLER_H
#define EXTERNALPROCESSCONTROL_DERIVEDCONTROLLER_H

#include <ExternalProcessController/Include/ExternalProcessController.h>

namespace ExternalProcessControl {

const QString DC_TEST_CMD = "nop";            ///< erroneous process name
const QString DC_TEST_NAME = "Himalaya GUI";  ///< real process name

/****************************************************************************/
/**
 * \brief This is a Derived Test Controller.
 *
 */
/****************************************************************************/
class DerivedController : public ExternalProcessControl::ExternalProcessController
{
    Q_OBJECT

private:

    /// test classes are all friends
    friend class TestExternalProcessCtlr;

protected:
    virtual void OnPowerFail();

public:

    virtual void OnReadyToWork();

public:

    DerivedController(Global::gSourceType TheLoggingSource);
    virtual ~DerivedController();

public:

    bool m_ReadyToWorkFlag;  ///< shows if process is ready foir normal operation
};

} // end namespace

#endif // EXTERNALPROCESSCONTROL_DERIVEDCONTROLLER_H
