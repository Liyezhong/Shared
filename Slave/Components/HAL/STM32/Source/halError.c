/****************************************************************************/
/*! \file halError.c
 *
 *  \brief  HAL error functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 24.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to convert error codes to error
 *      strings for displaying them on the console during debugging.
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

#include"Global.h"
#include"halError.h"

#ifdef DEBUG              // Omit conversion table in non-debug versions

//****************************************************************************/
//! Event-ID to Text String Conversion Table (Errors, Warnings, Infos)
//****************************************************************************/

const ErrorString_t halErrorStrings[] = {

    // General HAL errors
    { E_DEVICE_UNCONFIGURED,       "E_DEVICE_UNCONFIGURED"      },
    { E_DEVICE_HANDLE_INVALID,     "E_DEVICE_HANDLE_INVALID"    },
    { E_DEVICE_NOT_EXISTS,         "E_DEVICE_NOT_EXISTS"        },
    { E_DEVICE_NOT_OPEN,           "E_DEVICE_NOT_OPEN"          },
    { E_DEVICE_ALREADY_OPEN,       "E_DEVICE_ALREADY_OPEN"      },
    { E_DEVICE_ACCESS_MODE,        "E_DEVICE_ACCESS_MODE"       },
    { E_DEVICE_WRITE_ONLY,         "E_DEVICE_WRITE_ONLY"        },
    { E_DEVICE_READ_ONLY,          "E_DEVICE_READ_ONLY"         },
    { E_DEVICE_BUSY,               "E_DEVICE_BUSY"              },
    { E_DEVICE_DISABLED,           "E_DEVICE_DISABLED"          },

    { E_INVALID_REGISTER_ID,       "E_INVALID_REGISTER_ID"      },
    { E_INVALID_PERIPHERAL_ID,     "E_INVALID_PERIPHERAL_ID"    },
    { E_INVALID_INTERRUPT_ID,      "E_INVALID_INTERRUPT_ID"     },
    { E_INVALID_PRIORITY,          "E_INVALID_PRIORITY"         },
    { E_INVALID_PARAMETER,         "E_INVALID_PARAMETER"        },    
    { E_UNKNOWN_DEVICE_ID,         "E_UNKNOWN_DEVICE_ID"        },
    { E_UNKNOWN_STATUS_ID,         "E_UNKNOWN_STATUS_ID"        },
    { E_UNKNOWN_CONTROL_ID,        "E_UNKNOWN_CONTROL_ID"       },
    { E_UNSUPPORTED_FUNCTION,      "E_UNSUPPORTED_FUNCTION"     },
    { E_HEAP_MEMORY_FULL,          "E_HEAP_MEMORY_FULL"         },

    // GPIO port related error codes
    { E_PORT_DESCRIPTOR,           "E_PORT_DESCRIPTOR"          },
    { E_PORT_NOT_EXISTS,           "E_PORT_NOT_EXISTS"          },
    { E_PORT_DEFAULT_VALUE,        "E_PORT_DEFAULT_VALUE"       },
    { E_PORT_POLARITY_MASK,        "E_PORT_POLARITY_MASK"       },
    { E_PORT_DIRECTION,            "E_PORT_DIRECTION"           },
    { E_PORT_INTERFACE_TYPE,       "E_PORT_INTERFACE_TYPE"      },

    // GPIO pin related error codes
    { E_PIN_DESCRIPTOR,            "E_PIN_DESCRIPTOR"           },
    { E_PIN_NUMBER_INVALID,        "E_PIN_NUMBER_INVALID"       },
    { E_PIN_TYPE_MISMATCH,         "E_PIN_TYPE_MISMATCH"        },
    { E_PIN_ALLOCATED_TWICE,       "E_PIN_ALLOCATED_TWICE"      },
    { E_PIN_NOT_INTERRUPTABLE,     "E_PIN_NOT_INTERRUPTABLE"    },

    // Analog i/o related error codes
    { E_ADC_INPUT_NOT_EXISTS,      "E_ADC_INPUT_NOT_EXISTS"     },
    { E_ADC_INPUT_USED_TWICE,      "E_ADC_INPUT_USED_TWICE"     },
    { E_DAC_OUTPUT_NOT_EXISTS,     "E_DAC_OUTPUT_NOT_EXISTS"    },
    { E_DAC_OUTPUT_USED_TWICE,     "E_DAC_OUTPUT_USED_TWICE"    },

    // UART specific error codes
    { E_SCI_ILLEGAL_FORMAT,        "E_SCI_ILLEGAL_FORMAT"       },
    { E_SCI_ILLEGAL_BAUDRATE,      "E_SCI_ILLEGAL_BAUDRATE"     },
    { E_SCI_FORMAT_MISSING,        "E_SCI_FORMAT_MISSING"       },
    { E_SCI_FRAMING_ERROR,         "E_SCI_FRAMING_ERROR"        },
    { E_SCI_PARITY_ERROR,          "E_SCI_PARITY_ERROR"         },
    { E_SCI_OVERRUN_ERROR,         "E_SCI_OVERRUN_ERROR"        },
    { E_SCI_BREAK_DETECED,         "E_SCI_BREAK_DETECED"        },
    { E_SCI_RECEIVE_ERROR,         "E_SCI_RECEIVE_ERROR"        },
    { E_SCI_TRANSMIT_ERROR,        "E_SCI_TRANSMIT_ERROR"       },
    { E_SCI_NOISE_ERROR,           "E_SCI_NOISE_ERROR"          },

    // CAN controller related error codes
    { E_CAN_INVALID_BITRATE,       "E_CAN_INVALID_BITRATE"      },
    { E_CAN_INVALID_SEGTIME,       "E_CAN_INVALID_SEGTIME"      },
    { E_CAN_RECEIVER_OVERRUN,      "E_CAN_RECEIVER_OVERRUN"     },
    { E_CAN_INVALID_MESSAGE,       "E_CAN_INVALID_MESSAGE"      },
    { E_CAN_INVALID_PARAMETER,     "E_CAN_INVALID_PARAMETER"    },
    { E_CAN_FILTER_COUNT,          "E_CAN_FILTER_COUNT"         },

    // Timer controller related error codes
    { E_TIMER_DESCRIPTOR,          "E_TIMER_DESCRIPTOR"         },
    { E_TIMER_CAPCOM_CHANNEL,      "E_TIMER_CAPCOM_CHANNEL"     },
    { E_TIMER_CAPCOM_OVERFLOW,     "E_TIMER_CAPCOM_OVERFLOW"    },
    { E_TIMER_COUNT_DIRECTION,     "E_TIMER_COUNT_DIRECTION"    },
    { E_TIMER_INVALID_PRESCALE,    "E_TIMER_INVALID_PRESCALE"   },
    { E_TIMER_DELAY_LIMIT,         "E_TIMER_DELAY_LIMIT"        },
    { E_VALUE_OUT_OF_RANGE,        "E_VALUE_OUT_OF_RANGE"       },
    
    // Stepper related error codes
    { E_STEPPER_DESCRIPTOR,        "E_STEPPER_DESCRIPTOR"       },
    { E_STEPPER_ILLEGAL_MODE,      "E_STEPPER_ILLEGAL_MODE"     },
    { E_STEPPER_MOTOR_STALLED,     "E_STEPPER_MOTOR_STALLED"    },
    { E_STEPPER_SHORT_CIRCUIT,     "E_STEPPER_SHORT_CIRCUIT"    },
    { E_STEPPER_TEMPERATURE,       "E_STEPPER_TEMPERATURE"      },
    { E_STEPPER_OPEN_LOAD,         "E_STEPPER_OPEN_LOAD"        },
    { E_STEPPER_BUSY,              "E_STEPPER_BUSY"             },
    { E_STEPPER_NO_BUS,            "E_STEPPER_NO_BUS"           },

    // Storage (FRAM) specific error codes
    { E_STORAGE_READ_ERROR,        "E_STORAGE_READ_ERROR"       },
    { E_STORAGE_WRITE_ERROR,       "E_STORAGE_WRITE_ERROR"      },
    { E_STORAGE_OPEN_ERROR,        "E_STORAGE_OPEN_ERROR"       },
    { E_STORAGE_NOT_EXISTS,        "E_STORAGE_NOT_EXISTS"       },
    { E_STORAGE_ADDRESS,           "E_STORAGE_ADDRESS"          },
    { E_STORAGE_SIZE,              "E_STORAGE_SIZE"             },
    { E_STORAGE_CLASS,             "E_STORAGE_CLASS"            },
    { E_STORAGE_PROTECTED,         "E_STORAGE_PROTECTED"        },

    // Flash memory related error codes
    { E_FLASH_ALIGNMENT,           "E_FLASH_ALIGNMENT"          },
    { E_FLASH_BLOCKSIZE,           "E_FLASH_BLOCKSIZE"          },
    { E_FLASH_WRITE,               "E_FLASH_WRITE"              },
    { E_FLASH_ERASE,               "E_FLASH_ERASE"              },
    { E_FLASH_VERIFY,              "E_FLASH_VERIFY"             },
    { E_FLASH_WRITE_PROTECTED,     "E_FLASH_WRITE_PROTECTED"    },
    { E_FLASH_BUSY,                "E_FLASH_BUSY"               },
    { E_FLASH_TIMEOUT,             "E_FLASH_TIMEOUT"            },

    // I2C interface related errors
    { E_BUS_INVALID_ADDRESS,       "E_BUS_INVALID_ADDRESS"      },
    { E_BUS_ILLEGAL_BAUDRATE,      "E_BUS_ILLEGAL_BAUDRATE"     },
    { E_BUS_ILLEGAL_CLOCKRATE,     "E_BUS_ILLEGAL_CLOCKRATE"    },
    { E_BUS_INEXACT_BAUDRATE,      "E_BUS_INEXACT_BAUDRATE"     },
    { E_BUS_ALLOCATED_TWICE,       "E_BUS_ALLOCATED_TWICE"      },
    { E_BUS_MEMBER_COUNT,          "E_BUS_MEMBER_COUNT"         },
    { E_BUS_MEMBER_INVALID,        "E_BUS_MEMBER_INVALID"       },
    { E_BUS_JOB_ABORTED,           "E_BUS_JOB_ABORTED"          },
    { E_BUS_ERROR,                 "E_BUS_ERROR"                },
    { E_BUS_ACKNOWLEDGE,           "E_BUS_ACKNOWLEDGE"          },
    { E_BUS_ARBITRATION,           "E_BUS_ARBITRATION"          },
    { E_BUS_UNSPECIFIED,           "E_BUS_UNSPECIFIED"          },

    // DMA controller related error codes
    { E_DMA_INVALID_MODE,          "E_DMA_INVALID_MODE"         },
    { E_DMA_INVALID_PRIORITY,      "E_DMA_INVALID_PRIORITY"     },

    // External interrupt related error codes
    { E_EXTINT_NOT_EXISTS,         "E_EXTINT_NOT_EXISTS"        },
    { E_EXTINT_USED_TWICE,         "E_EXTINT_USED_TWICE"        },

    // Realtime clock related error codes
    { E_RTC_NOT_RUNNING,           "E_RTC_NOT_RUNNING"          },
    { E_RTC_YEAR_OUT_OF_RANGE,     "E_RTC_YEAR_OUT_OF_RANGE"    },

    // System clock related error codes
    { E_SYSCLOCK_MULTIPLIER,       "E_SYSCLOCK_MULTIPLIER"      },
    { E_SYSCLOCK_DIVIDER,          "E_SYSCLOCK_DIVIDER"         },
    { E_SYSCLOCK_RATE,             "E_SYSCLOCK_RATE"            },

    // Watchdog related error codes
    { E_WATCHDOG_INTERVAL,         "E_WATCHDOG_INTERVAL"        },
};

//! Number of elements in the error string table
const UInt32 halErrorStringCount = ELEMENTS(halErrorStrings);


/*****************************************************************************/
/*!
 *  \brief   Get error description table
 *
 *      Returns the error string table pointer in ErrorStrings. The size
 *      of the table is returned as return parameter.
 *
 *      This function is available only in debug mode
 *
 *  \oparam  ErrorStrings = Error string table pointer
 *
 *  \return  Size of error string table
 *
 ****************************************************************************/

UInt32 halGetErrorStrings (const ErrorString_t *ErrorStrings[]) {

    if (ErrorStrings != NULL) {
        *ErrorStrings = halErrorStrings;
    }
    return (halErrorStringCount);
}

//****************************************************************************/

#endif
