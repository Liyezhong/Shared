/****************************************************************************/
/*! \file halEncoder.h
 * 
 *  \brief  Quadrature Encoder Functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 22.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage quadrature 
 *      encoders to sense the position of a motor. Since the STM32 has
 *      no dedicated quadrature encoder hardware, the timers are used
 *      for this function. To be able to port the software to other
 *      hardware plattforms (which might have dedicated decoders), a
 *      dedicated software interface is provided, that hides the fact
 *      that standard timers are used on STM32 hardware.
 *
 *      The encoder supports up to two reference positions: when the
 *      actual position hits a reference position, an interrupt can
 *      be generated. An index input from an encoder is supported,
 *      which triggers sampling the actual position in an index
 *      register. It can also be configured to generate an interrupt.
 *      The index input and the 2nd reference position are available
 *      alternative only. Which option to use is determined by the
 *      HAL configuration file.
 * 
 *      Direction change interrupts and phase error interrupts are not
 *      implemented, because there is no hardware support for these
 *      features. Nevertheless, the actual data direction can be
 *      requested using the status function.
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

#ifndef HAL_ENCODER_H
#define HAL_ENCODER_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Encoder interrupt flag bit masks
#define ENC_IFLAG_OVERFLOW    0x01    //!< Encoder over-/underflow interrupt
#define ENC_IFLAG_LIMIT1      0x10    //!< Limit 1 position interrupt                                    
#define ENC_IFLAG_LIMIT2      0x08    //!< Limit 2 position interrupt 
#define ENC_IFLAG_INDEX       0x08    //!< Index impuls interrupt

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Register select IDs used by halEncodeRead/Write
typedef enum {
    ENC_REG_POSITION,      //!< Select actual position register
    ENC_REG_LIMIT1,        //!< Select limit 1 register
    ENC_REG_LIMIT2,        //!< Select limit 2 register 
    ENC_REG_ENDPOSITION    //!< Select end-position register
} EncRegsID_t;

//! Index uses same unit as limit 2 monitor (alternative)
#define ENC_REG_INDEX      ENC_REG_LIMIT2


//! Control IDs for controlling encoder (used by halEncoderControl)
typedef enum {
    ENC_INTR_OVERFLOW,     //!< Encoder over-/underflow interrupt control
    ENC_INTR_LIMIT1,       //!< Limit 1 interrupt control
    ENC_INTR_LIMIT2,       //!< Limit 2 interrupt control
} EncCtrlID_t;

//! Index uses same unit as limit 2 monitor (alternative)
#define ENC_INTR_INDEX     ENC_INTR_LIMIT2


//! Status IDs for requesting the state of an encoder 
typedef enum {
    ENC_STAT_DIRECTION,    //!< Request rotation direction
    ENC_STAT_PENDING,      //!< Request pending interrupts
    ENC_STAT_LIMITS,       //!< Request number of supported limits
    ENC_STAT_INDEX         //!< Request if index option available
} EncStatID_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halEncoderOpen    (Device_t Device, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halEncoderClose   (Handle_t Handle);
Error_t halEncoderInit    (void);

Error_t halEncoderRead    (Handle_t Handle, EncRegsID_t RegID, UInt32 *Value);
Error_t halEncoderWrite   (Handle_t Handle, EncRegsID_t RegID, UInt32 Value);
Error_t halEncoderControl (Handle_t Handle, EncCtrlID_t ControlID, Bool State);
Error_t halEncoderStatus  (Handle_t Handle, EncStatID_t StatusID);

//****************************************************************************/

#endif /*HAL_ENCODER_H*/
