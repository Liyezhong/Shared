/****************************************************************************/
/*! \file halCommon.h
 *
 *  \brief  Common HAL declarations
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains definitions that are used by all HAL modules
 *       and for the HAL configuration as well.
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

#define MAPPING_DEFAULT         0        //!< Use default pin mapping

// Supported Processor Classes
#define DEVICE_FAMILY_F100      0        //!< Value line family device
#define DEVICE_FAMILY_F101      1        //!< Access line family device
#define DEVICE_FAMILY_F102      2        //!< Access + USB line family device
#define DEVICE_FAMILY_F103      3        //!< Performance line family device
#define DEVICE_FAMILY_F105      4        //!< Connectivity line family

// Defines for the Processor Densities
#define DEVICE_DENSITY_LOW      0        //!< Low density device
#define DEVICE_DENSITY_MEDIUM   1        //!< Medium density device
#define DEVICE_DENSITY_HIGH     2        //!< High density device
#define DEVICE_DENSITY_XL       3        //!< XL density device

// Logical Device ID classes
#define DEVICE_CLASS_DIGIN      0x3100   //!< Device class digital in
#define DEVICE_CLASS_DIGOUT     0x3200   //!< Device class digital out
#define DEVICE_CLASS_ANALOG     0x7300   //!< Device class analog i/o
#define DEVICE_CLASS_TIMER      0x4400   //!< Device class timers
#define DEVICE_CLASS_CAPTURE    0x2600   //!< Device class capture units
#define DEVICE_CLASS_ENCODE     0x1900   //!< Device class encoders
#define DEVICE_CLASS_SERIAL     0x8900   //!< Device class serial i/o
#define DEVICE_CLASS_SPI        0x6200   //!< Device class SPI
#define DEVICE_CLASS_I2C        0x0900   //!< Device class I2C
#define DEVICE_CLASS_CAN        0x3300   //!< Device class CAN
#define DEVICE_CLASS_STORAGE    0x2900   //!< Device class storage
#define DEVICE_CLASS_STEPPER    0x7500   //!< Device class stepper motor
#define DEVICE_CLASS_DEBUG      0x9A00   //!< Device class debug i/o
#define DEVICE_CLASS_IRQ        0x1100   //!< Device class external interrupt
#define DEVICE_CLASS_PWM        0xCC00   //!< Device class PWM output

#define DEVICE_CLASS_MASK       0xFF00   //!< Device mask to isolate class
#define DEVICE_INDEX_MASK       0x00FF   //!< Device mask to isolate index

//! Macro to build a device ID out of the device class and number
#define DEVICEiD(Class,Index)   ((Class) | (Index))


//****************************************************************************/
// Constants to be used by HAL open functions
//****************************************************************************/

#define HAL_OPEN_MODE      0x0F  //!< Open mode bit mask
#define HAL_OPEN_READ      0x01  //!< Open for reading
#define HAL_OPEN_WRITE     0x02  //!< Open for writing
#define HAL_OPEN_RW        0x03  //!< Open for reading and writing
#define HAL_OPEN_ERASE     0x04  //!< Open for erasing
#define HAL_OPEN_RWE       0x07  //!< Open for reading, writing and erasing

#define HAL_FLAG_INITZED   0x80  //!< Device initialized
#define HAL_FLAG_LOCK      0x40  //!< Device locked


//****************************************************************************/
// Constants to be used for pin configuration (don't change)
//****************************************************************************/

// Physical port defines (symbolic names)
#define PORT_A             0     //!< Processor port A
#define PORT_B             1     //!< Processor port B
#define PORT_C             2     //!< Processor port C
#define PORT_D             3     //!< Processor port D
#define PORT_E             4     //!< Processor port E
#define PORT_F             5     //!< Processor port F
#define PORT_G             6     //!< Processor port G

// Phisical pin multiplexer settings
#define PIN_MUX_GPIO       0     //!< General purpose port i/o
#define PIN_MUX_SFIO       1     //!< Special function i/o

// Physical pin types
#define PIN_TYP_ANALOG     0     //!< Analog input/output
#define PIN_TYP_DINPUT     1     //!< Digital input
#define PIN_TYP_DOUT2      2     //!< Digital output, 2MHz
#define PIN_TYP_DOUT10     3     //!< Digital output, 10MHz
#define PIN_TYP_DOUT50     4     //!< Digital output, 50MHz

// Physical pin options
#define PIN_OPT_NONE       0     //!< No pin options
#define PIN_OPT_HIGHZ      0     //!< High impedance input (default)
#define PIN_OPT_PULLUP     1     //!< Enable internal pullup (input only)
#define PIN_OPT_PULLDN     2     //!< Enable internal pulldown (input only)
#define PIN_OPT_OPENDR     3     //!< Enable open drain output (output only)
#define PIN_OPT_DRIVER     4     //!< Enable analog driver (analog output)

// Data Direction Defines (Input/Output)
#define DIR_INPUT          0     //!< Define resource as input
#define DIR_OUTPUT         1     //!< Define resource as output


//****************************************************************************/
// Constants to be used for other configuration (don't change)
//****************************************************************************/

// Bus Type Defines
#define BUS_TYPE_INTERN    0     //!< Processor intern parallel bus
#define BUS_TYPE_AD779x    1     //!< AD779x analog frontend (uses SPI)
#define BUS_TYPE_PWM       2     //!< PWM output signal
#define BUS_TYPE_SPI       3     //!< Simple write only SPI
#define BUS_TYPE_MAXVAL    4     //!< Max. value for bus type

// Serial I/O Type Defines
#define SIO_TYPE_I2C       0     //!< Serial I2C bus
#define SIO_TYPE_SPI       1     //!< Serial SPI bus
#define SIO_TYPE_CAN       2     //!< Serial CAN bus
#define SIO_TYPE_UART      3     //!< Serial SPI bus
#define SIO_TYPE_MAXVAL    3     //!< Max. value serial i/o type

// Storage/Memory Type Defines
#define MEM_CLASS_RAM      0     //!< Memory class RAM
#define MEM_CLASS_FLASH    1     //!< Memory class FLASH
#define MEM_CLASS_EEPROM   2     //!< Memory class EEPROM
#define MEM_CLASS_FRAM     3     //!< Memory class FRAM

// External Interrupt Edge Sensitivity
#define IRQ_EDGE_NONE      0     //!< No interrupt
#define IRQ_EDGE_RISING    1     //!< Interrupt on rising edge
#define IRQ_EDGE_FALLING   2     //!< Interrupt on falling edge
#define IRQ_EDGE_BOTH      3     //!< Interrupt on rising and falling edge

// Interrupt Priority Defines
#define IRQ_PRIO_HIGHEST   0     //!< Highest interrupt priority
#define IRQ_PRIO_DEFAULT   5     //!< Standard interrupt priority
#define IRQ_PRIO_LOWEST    15    //!< Lowest interrupt priority

// Timer channel type defines
#define TIM_SEL_DISABLE    0     //!< Disable timer or timer channel
#define TIM_SEL_COUNTER    1     //!< Configure core timer counter
#define TIM_SEL_CAPTURE    2     //!< Configure channel as capture unit
#define TIM_SEL_COMPARE    3     //!< Configure channel as compare unit

#define BITS_PER_PORT      16    //!< Number of bits per port


//****************************************************************************/
// Public Type Declarations
//****************************************************************************/

//! Structure to configure physical processor pins
typedef struct {
    UInt16 PortNo    :4;    //!< GPIO port number (0...6)
    UInt16 PortBit   :4;    //!< GPIO port bit number (0...15)
    UInt16 Multiplex :1;    //!< GPIO pin multiplexer (SF/GPIO)
    UInt16 Type      :3;    //!< GPIO pin type (in/out/analog)
    UInt16 Option    :3;    //!< GPIO pin pullup enable/disable
} halPinDesciptor_t;

//! Structure to configure logical digital I/O ports
typedef struct {
    UInt16 DeviceID;        //!< Logical device ID
    UInt16 Direction :1;    //!< Data direction (input, output)
    UInt16 Interface :2;    //!< Interface (intern, ...)
    UInt16 PortNo    :3;    //!< Physical port number
    UInt16 Shifts    :5;    //!< Number of bits to shift
    UInt16 BitCount  :5;    //!< Number of bits
    UInt32 Polarity;        //!< Polarity bitmask (inverted, non-inverted)
    UInt32 Initial;         //!< Initial output value
} halPortDescriptor_t;

//! Structure to configure logical analog I/O channels
typedef struct {
    UInt16 DeviceID;        //!< Logical device ID
    UInt16 Direction :1;    //!< Data direction (input, output)
    UInt16 Interface :2;    //!< Interface (intern, AD779x, PWM, SPI)
    UInt16 BitCount  :5;    //!< Number of bits
    UInt16 PortNo    :5;    //!< Physical input/output number
    UInt16 ConvTime  :3;    //!< Data conversion time
    Int32  MaxValue;        //!< Scaling: full scale value
    Int32  Initial;         //!< Initial output value
    UInt16 PeripheralID;    //!< Device ID of the underlying I/O (only for PWM, SPI)
} halAnalogDescriptor_t;

//! Structure to configure physical timer inputs/outputs
typedef struct {
    UInt16 DeviceID;        //!< Logical device ID
    UInt16 TimerNo   :4;    //!< Physical timer number (0...15)
    UInt16 UnitNo    :2;    //!< Capture/Compare unit number
    UInt16 Priority  :4;    //!< Priority of interrupt
    UInt16 TypeID    :2;    //!< Timer, capture, or compare unit
    UInt16 Mode;            //!< Mode of timer, capture, compare unit
} halTimerDescriptor_t;

//! Structure to configure external interrupt inputs
typedef struct {
    UInt16 DeviceID;        //!< Logical device ID
    UInt16 PortNo    :4;    //!< Port the external interrupt is mapped to
    UInt16 PortBit   :4;    //!< EXTINT interrupt line number [0...15]
    UInt16 Priority  :4;    //!< Interrupt priority
    UInt16 Edge      :2;    //!< Active edge (rising, falling, both, none)
} halExtIntrDescriptor_t;

//! Structure to configure serial busses
typedef struct {
    UInt16 DeviceID;        //!< Device ID of SPI port
    UInt16 SelectID;        //!< Device ID of chip select (SPI only)
    UInt16 BusType   :3;    //!< Bus type (I2C, SPI, UART, CAN)
    UInt16 Members   :4;    //!< Number of bus members (I2C only)
    UInt16 UnitNo    :2;    //!< Physical unit number
    UInt32 Baudrate;        //!< Bus data rate
} halSerialDescriptor_t;

//! Structure to configure logical storages
typedef volatile struct {
    UInt16 DeviceID;        //!< Logical device ID
    UInt16 Class     :5;    //!< Memory class (FRAM, EEPROM, FLASH)
    UInt16 Mode      :3;    //!< Access mode (Read/Write/Erase)
    UInt32 Address;         //!< Base address
    UInt32 Size;            //!< Size of logical memory
} halStorageDescriptor_t;

//! Structure to configure stepper motors
typedef struct {
    UInt16 DeviceID;        //!< Logical device ID
    UInt16 SpiNo;           //!< SPI channel number
    UInt32 Mode[4];         //!< TMC controller settings
} halStepperDescriptor_t;

//! Structure to configure extern ADC779x analog input channels
typedef struct {
    UInt16 DeviceID;        //!< Slave device ID (SPI)
    UInt16 PortNo    :4;    //!< Input channel number (logical)
    UInt16 ScanRate  :4;    //!< Input scan relative frequency
    UInt16 IoReg     :8;    //!< AD779x i/o register setting
    UInt16 Mode;            //!< AD779x mode register setting
    UInt16 Conf;            //!< AD779x configuration register setting
} halAd779xDescriptor_t;

//! Structure to configure peripheral pin mappings
typedef struct {
    UInt32 SP1       :1;    //!< SPI1 mapping
    UInt32 I2C1      :1;    //!< I2C1 mapping
    UInt32 USART1    :1;    //!< USART1 mapping
    UInt32 USART2    :1;    //!< USART2 mapping
    UInt32 USART3    :2;    //!< USART3 mapping
    UInt32 TIMER1    :2;    //!< TIMER1 mapping
    UInt32 TIMER2    :2;    //!< TIMER2 mapping
    UInt32 TIMER3    :2;    //!< TIMER3 mapping
    UInt32 TIMER4    :1;    //!< TIMER4 mapping
    UInt32 CAN1      :2;    //!< CAN1 mapping
    UInt32 PD0PD1    :1;    //!< Port D0/D1 mapping on OSC_IN/OSC_OUT
    UInt32 TIMER5    :1;    //!< TIMER5 channel4 internal remap
    UInt32 ADC1ETRJ  :1;    //!< ADC1 ext. trigger injected mapping
    UInt32 ADC1ETRG  :1;    //!< ADC1 ext. trigger regular mapping
    UInt32 ADC2ETRJ  :1;    //!< ADC2 ext. trigger injected mapping
    UInt32 ADC2ETRG  :1;    //!< ADC2 ext. trigger regular mapping
    UInt32 ETHERNET  :1;    //!< Ethernet MAC I/O mapping
    UInt32 CAN2      :1;    //!< CAN2 I/O mapping
    UInt32 RMII      :1;    //!< Ethernet MII/RMII selection
    UInt32 SWJCFG    :3;    //!< Serial wire JTAG configuration
    UInt32 Reserve1  :1;    //!< Reserved
    UInt32 SPI3      :1;    //!< SPI3 mapping
    UInt32 TIM2ITR1  :1;    //!< TIMER2 internal trigger 1 mapping
    UInt32 PTP_PPS   :1;    //!< Ethernet PTP PPS mapping
    UInt32 Reserve2  :1;    //!< Reserved
    UInt32 TIMER15   :1;    //!< TIMER15 mapping
    UInt32 TIMER16   :1;    //!< TIMER16 mapping
    UInt32 TIMER17   :1;    //!< TIMER17 mapping
    UInt32 CEC       :1;    //!< CEC mapping
    UInt32 TIM1CH1   :1;    //!< TIMER1 CH1 mapping        
    UInt32 TIMER9    :1;    //!< TIMER9 mapping
    UInt32 TIMER10   :1;    //!< TIMER10 mapping
    UInt32 TIMER11   :1;    //!< TIMER11 mapping
    UInt32 TIMER13   :1;    //!< TIMER13 mapping
    UInt32 TIMER14   :1;    //!< TIMER14 mapping
    UInt32 FSMC      :1;    //!< FSMC NADV connect/disconnect
    UInt32 TIMER67   :1;    //!< TIMER6/7 mapping
    UInt32 TIMER12   :1;    //!< TIMER12 mapping
    UInt32 MISC      :1;    //!< Miscellaneous mapping
    UInt32 Reserve3  :18;   //!< Reserved
} halPinMapping_t;

//! Structure to configure processor information block
typedef struct {
    UInt32 Processor  :3;   //!< Processor family ID
    UInt32 DensityID  :3;   //!< Processor density ID
    UInt32 ClockRate  :7;   //!< Processor clock rate (MHz)
    UInt32 CountGPIO  :8;   //!< Number of general purpose I/O's
    UInt32 CountPort  :4;   //!< Number of ports
    UInt32 CountADC   :6;   //!< Number of analog inputs
    UInt32 CountDAC   :2;   //!< Number of analog outputs
    UInt32 CountCAN   :2;   //!< Number of CAN controllers
    UInt32 CountDMA   :5;   //!< Number of DMA channels
    UInt32 CountTimer :5;   //!< Number of timers
    UInt32 CountUART  :4;   //!< Number of UARTs
    UInt32 CountSPI   :4;   //!< Number of SPI interfaces
    UInt32 CountI2C   :4;   //!< Number of I2C interfaces
    UInt32 TimerMask  :16;  //!< Timer selection bitmap
    UInt32 SizeFLASH  :12;  //!< Size of FLASH memory (kBytes)
    UInt32 SizeRAM    :10;  //!< Size of RAM (KBytes)
} halProcessorInfo_t;

typedef UInt16 Device_t;    //!< Logical channel identifier

//****************************************************************************/

#include "halDescriptor.h"  // Publish descriptor tables

//****************************************************************************/

#endif
