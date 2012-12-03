/****************************************************************************/
/*! \file halCommon.h
 * 
 *  \brief  Common HAL declarations
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains definitions that are used by all HAL modules
 *       and for the HAL configuartion as well.
 * 
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice 
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef HAL_COMMON_H
#define HAL_COMMON_H

//****************************************************************************/
// Constants and Macros
//****************************************************************************/

#define CHANNEL_CLASS_MASK      0xFF00   //!< channel mask to isolate class
#define CHANNEL_INDEX_MASK      0x00FF   //!< channel mask to isolate index

#define CHANNEL_CLASS_DIGIO     0x3200   //!< Channel class digital i/o
#define CHANNEL_CLASS_ANALOG    0x7300   //!< Channel class analog i/o
#define CHANNEL_CLASS_TIMER     0x4400   //!< Channel class timers
#define CHANNEL_CLASS_CAPTURE   0x2600   //!< Channel class capture units
#define CHANNEL_CLASS_ENCODE    0x1900   //!< Channel class encoders
#define CHANNEL_CLASS_SERIAL    0x8900   //!< Channel class serial i/o
#define CHANNEL_CLASS_SPI       0x6200   //!< Channel class SPI
#define CHANNEL_CLASS_I2C       0x0900   //!< Channel class I2C
#define CHANNEL_CLASS_STORAGE   0x2900   //!< Channel class storage

#define CHANNELID(Class,Index)  ((Class) | (Index))

//! Enumeration to be used by HAL open functions
typedef enum {
    HAL_OPEN_READ  = BIT(0),    //!< Open for reading
    HAL_OPEN_WRITE = BIT(1),    //!< Open for writing
    HAL_OPEN_ERASE = BIT(2),    //!< Open for erasing    
    HAL_OPEN_RW    = HAL_OPEN_READ+HAL_OPEN_WRITE
} HAL_OPEN_MODE_t;

//****************************************************************************/
// Constants to be used for configuration (don't change) 
//****************************************************************************/

// Digital I/O Port Directions (Input/Output)
#define PORT_INPUT          0   //!< Define port as input
#define PORT_OUTPUT         1   //!< Define port as push/pull output

// Digital I/O Port Types (currently only INTERN supported)
#define PORT_TYPE_INTERN    0   //!< Processor intern GPIO ports
#define PORT_TYPE_SERIAL    1   //!< serial port expander (shift registers)
#define PORT_TYPE_I2C       2   //!< serial I2C port expander
#define PORT_TYPE_SPI       3   //!< serial SPI port expander 
#define PORT_TYPE_MAXVAL    3   //!< max. value for port type

// GPIO Port Qualification Mode (individual for each pin)
#define INPUT_QUAL_SYSCLK   0   //!< Synchronize to sysclock
#define INPUT_QUAL_3SAMPLES 1   //!< Qualification using 3 samples
#define INPUT_QUAL_6SAMPLES 2   //!< Qualification using 6 samples
#define INPUT_QUAL_NONE     3   //!< No synchronization/qualification
#define INPUT_QUAL_MAXVAL   3   //!< max. value for qualification

// GPIO Port Pullup Enable/Disable (individual for each pin)
#define PULLUP_ENABLED      1   //!< enable pullup on pin
#define PULLUP_DISABLED     0   //!< disable pullup on pin

// GPIO External Interrupt Edge Sensitivity
#define XINT_EDGE_RISING    1   //!< interrupt on rising edge
#define XINT_EDGE_FALLING   2   //!< interrupt on falling edge
#define XINT_EDGE_BOTH      3   //!< interrupt on rising and falling edge
#define NO_INTERRUPT        0   //!< no interrupt 

// Macros to define characteristics for each external interrupt
#define XINT1(edge)         (((edge) << 4) | 0)
#define XINT2(edge)         (((edge) << 4) | 1)
#define XINT3(edge)         (((edge) << 4) | 3)
#define XINT4(edge)         (((edge) << 4) | 4)
#define XINT5(edge)         (((edge) << 4) | 5)
#define XINT6(edge)         (((edge) << 4) | 6)
#define XINT7(edge)         (((edge) << 4) | 7)
#define XNMI(edge)          (((edge) << 4) | 2)


//****************************************************************************/
// Type Declarations
//****************************************************************************/

//! Structure to configure logical digital I/O ports
typedef struct {
    UInt16 ChannelID;       //!< Logical port ID
    UInt16 Direction   :1;  //!< data direction (input, output)
    UInt16 Interface   :2;  //!< interface (intern, serial, I2C, SPI)
    UInt16 BitCount    :5;  //!< number of bits
    UInt16 PortNr      :3;  //!< hardware port number
    UInt16 Shifts      :5;  //!< number of bits to shift 
    UInt32 Polarity;        //!< polarity bitmask (inverted, non-inverted)
    UInt32 Initial;         //!< initial output value
} PORT_DESCRIPTOR_t;

//! Structure to configure logical analog I/O channels
typedef struct {
    UInt16 ChannelID;       //!< Logical channel ID
    UInt16 Direction   :1;  //!< data direction (input, output)
    UInt16 Interface   :2;  //!< interface (intern, serial, I2C, SPI)
    UInt16 BitCount    :5;  //!< number of bits
    UInt16 ChannelNr   :5;  //!< hardware channel number    
    UInt16 MaxValue;        //!< scaling: full scale value
    UInt16 Offset;          //!< offset to add/subtract to/from value
    UInt16 Initial;         //!< initial output value
} CHANNEL_DESCRIPTOR_t;

//! Structure to configure physical processor pins
typedef struct {
    UInt16 PortNum     :3;  //!< GPIO port number (0...3)
    UInt16 PortBit     :5;  //!< GPIO port bit number (0...31)
    UInt16 Multiplexer :2;  //!< GPIO pin multiplexer
    UInt16 Qualifier   :2;  //!< GPIO input qualifier type
    UInt16 Pullup      :1;  //!< GPIO pin pullup enable/disable
    UInt16 Interrupt   :8;  //!< GPIO external interrupt mode
} PIN_DEFINITION_t;

//! Structure to configure input qualification sample rates
typedef struct {
    UInt16 PortNum     :2;  //!< GPIO port number (0...3)
    UInt16 ByteNum     :2;  //!< GPIO port byte number (0...3)
    UInt16 SampleRate  :8;  //!< Sample rate (0...255)
} PIN_QUALIFICATION_t;

typedef UInt16 CHANNEL_t;   //!< Logical channel identifier

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

inline void halUnprotectRegisters(void) { asm(" EALLOW"); }
inline void halProtectRegisters(void)   { asm(" EDIS");   }

//****************************************************************************/

#endif
