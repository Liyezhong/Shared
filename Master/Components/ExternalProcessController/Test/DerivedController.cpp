/****************************************************************************/
/*! \file DerivedController.cpp
 *
 *  \brief Implementation file for class DerivedController.
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

#include <ExternalProcessController/Test/DerivedController.h>
#include <Global/Include/GlobalDefines.h>

namespace ExternalProcessControl {

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \param[in]   TheLoggingSource    Logging source to be used.
 */
/****************************************************************************/
DerivedController::DerivedController(Global::gSourceType TheLoggingSource) :
    ExternalProcessController::ExternalProcessController(DC_TEST_NAME, TheLoggingSource),
    m_ReadyToWorkFlag(false)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
DerivedController::~DerivedController()
{
}

/****************************************************************************/
/**
 * \brief This method is called when controller is connected and ready to work.
 *
 *    This means we are in the "Working" state.
 *
 * \return
 */
/****************************************************************************/
void DerivedController::OnReadyToWork()
{
    m_ReadyToWorkFlag = true;
}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 */
/****************************************************************************/
void DerivedController::OnPowerFail()
{
}

} // end namespace HimalayaGui

