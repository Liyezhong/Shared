/****************************************************************************/
/*! \file halExtIntr.h
 *
 *  \brief  Extern interrupt input handling
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and control external
 *      interrupt inputs. Also the alternate function pin initialization
 *      is done in this module, based on the settings in the global HAL
 *      configuration module.
 *
 *      Each port pin of the microcontroller can be used as an external
 *      interrupt input, but there are only 16 external interrupt lines.
 *      Bit of each port can be connected to interrupt line. Each
 *      interrupt line can handle only one interrupt input. This rules
 *      lead to restrictions in using port bits as interrupt inputs.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef HAL_EXTINT_H
#define HAL_EXTINT_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Control IDs for controlling external interrupts
typedef enum {
    EXT_CTRL_ENABLE,        //!< Enable external interrupt
    EXT_CTRL_DISABLE,       //!< Disable external interrupt
    EXT_CTRL_TRIGGER,       //!< Trigger interrupt by software
    EXT_CTRL_CLEAR,         //!< Clear interrupt pending bits
} ExtCtrlID_t;

//! Status IDs for requesting the state of an external interrupt
typedef enum {
    EXT_STAT_PENDING,       //!< Request interrupt pending status
} ExtStatID_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halExtIntOpen    (Device_t Device, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halExtIntControl (Handle_t Handle, ExtCtrlID_t ControlID);
Error_t halExtIntStatus  (Handle_t Handle, ExtStatID_t StatusID);
Error_t halExtIntClose   (Handle_t Handle);
Error_t halExtIntInit    (void);

//****************************************************************************/

#endif /*HAL_EXTINT_H*/
