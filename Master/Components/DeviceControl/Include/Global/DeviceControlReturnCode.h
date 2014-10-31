#ifndef DEVICE_CONTROL_RETURNCODE_H
#define DEVICE_CONTROL_RETURNCODE_H

namespace DeviceControl
{
typedef enum {

	DCL_ERR_FCT_CALL_SUCCESS						= 0x0000, //!< function was executed successfully
	DCL_ERR_FCT_CALL_FAILED							= 0x0001,  //!< function was failed, no further information available
	DCL_ERR_UNDEFINED                               = 0x0002,  //!< The return code was NOT set
	DCL_ERR_FCT_CALL_NOT_FOUND                      = 0x0003,  //!< inside the function, a parameter concerning action was not done because anything was not found
	DCL_ERR_FCT_NOT_IMPLEMENTED                     = 0x0004,  //!< Temporary return value for bare functions, should be removed at the end
	DCL_ERR_INVALID_STATE                           = 0x0005,  //!< a method was called while the internal state machine was not able to process the functionality
	DCL_ERR_NOT_INITIALIZED                         = 0x0006,  //!< A method was called before complete initialisation was done
	DCL_ERR_TIMEOUT                                 = 0x0007,  //!< A timeout appeared (typically a CAN-Bus timeout, while waiting for an answer)
	DCL_ERR_INVALID_PARAM                           = 0x0008,  //!< One ore more invalid parameters were passed to the method
	DCL_ERR_INTERNAL_ERR                            = 0x0009,  //!< The error reason is internal, further information is available at ...
	DCL_ERR_EXTERNAL_ERROR                          = 0x0010,  //!< The error reason is external, it was reported via CAN-bus
	DCL_ERR_EXTERNAL_WARNING                        = 0x001A,  //!< The warning reason is external, it was reported via CAN-bus
	DCL_ERR_EXTERNAL_INFO                           = 0x001B,  //!< The information reason is external, it was reported via CAN-bus
	DCL_ERR_NULL_PTR_ACCESS                         = 0x001C,  //!< A null pointer would be accessed, but there was an instruction to avoid its direct access
	DCL_ERR_CANMSG_INVALID                          = 0x001D,  //!< An invalid CAN-message was received (e.g. dlc not correct)
	DCL_ERR_CANBUS_NOT_READY                        = 0x001E,  //!< A CAN-message was tried to be send while the CAN bus is in error state
	DCL_ERR_CANBUS_ERROR                            = 0x001F,  //!< A CAN-message was tried to be send while an 'unknown' CAN bus error is active
	DCL_ERR_UNEXPECTED_BREAK                        = 0x0020,  //!< Current program was breaked by other program
	DCL_ERR_SNYC_CALL_BUSY                          = 0x0021,  //!< Current program was breaked by other program
	DCL_ERR_TIMER_TIMEOUT                           = 0x0022,  //!< Timer Timout
	DCL_ERR_CANCOMMUTOR_START               		= 0x0023,  //!< CAN Communicator start
	DCL_ERR_CANCOMMUTOR_INTERFACE_OPEN      		= 0x0024,  //!< CAN Communicator Interface Open
	DCL_ERR_CANCOMMUTOR_TCPCLIENT_INIT      		= 0x0025,  //!< CAN Communicator TCP Client Initialized
	DCL_ERR_CANCOMMUTOR_COMM_FAILED         		= 0x0026,  //!< CAN Communicator communication failed
	DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_0    		= 0x0027,  //!< Get level sensor state 0
	DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1    		= 0x0028,  //!< Get level sensor state 1
	DCL_ERR_DEV_TEMP_CTRL_STATE_ERR                 = 0x0029,  //!< Temperatuer control state error
	DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON                = 0x002A,  //!< Temperatuer control is already on
	DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR              = 0x002B,  //!< Temperatuer control set temperature error
	DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR             = 0x002C,  //!< Temperatuer control set state error
	DCL_ERR_DEV_RETORT_TSENSOR1_TEMPERATURE_OVERRANGE          	= 500010201,
	DCL_ERR_DEV_RETORT_TSENSOR2_TEMPERATURE_OVERRANGE          	= 500010221,
	DCL_ERR_DEV_RETORT_TSENSOR3_TEMPERATURE_OVERRANGE          	= 500010241,
	DCL_ERR_DEV_RETORT_TSENSOR1_TEMPERATURE_NOSIGNAL           	= 500010211,
	DCL_ERR_DEV_RETORT_TSENSOR2_TEMPERATURE_NOSIGNAL           	= 500010231,
	DCL_ERR_DEV_RETORT_TSENSOR3_TEMPERATURE_NOSIGNAL           	= 500010251,
	DCL_ERR_DEV_RETORT_TSENSOR1_TO_2_SELFCALIBRATION_FAILED    	= 500010261,
	DCL_ERR_DEV_RETORT_LEVELSENSOR_HEATING_OVERTIME            	= 500010301,
	DCL_ERR_DEV_LEVELSENSOR_TEMPERATURE_OVERRANGE              	= 500010321,
	DCL_ERR_DEV_LIDLOCK_CLOSE_STATUS_ERROR                     	= 500010461,
	DCL_ERR_DEV_RETORT_HEATING_OVERTIME                        	= 500010501,
	DCL_ERR_DEV_WAXBATH_TSENSORUP_OUTOFRANGE                   	= 500020021,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN1_OUTOFRANGE                	= 500020031,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN2_OUTOFRANGE                	= 500020041,
	DCL_ERR_DEV_WAXBATH_SENSORUP_HEATING_OUTOFTARGETRANGE      	= 500020081,
	DCL_ERR_DEV_WAXBATH_SENSORDOWN1_HEATING_OUTOFTARGETRANGE   	= 500020082,
	DCL_ERR_DEV_WAXBATH_SENSORDOWN2_HEATING_OUTOFTARGETRANGE   	= 500020083,
	DCL_ERR_DEV_WAXBATH_SENSORUP_HEATING_ABNORMAL              	= 500020051,
	DCL_ERR_DEV_WAXBATH_SENSORDOWN1_HEATING_ABNORMAL           	= 500020061,
	DCL_ERR_DEV_WAXBATH_SENSORDOWN2_HEATING_ABNORMAL           	= 500020071,
	DCL_ERR_DEV_WAXBATH_OVENCOVER_STATUS_OPEN                  	= 500020090,
	DCL_ERR_DEV_RV_MOTOR_CANNOTGET_ORIGINALPOSITION            	= 500030001,
	DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_RETRY                   	= 500030011,
	DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_EXCEEDUPPERLIMIT        	= 500030021,
	DCL_ERR_DEV_RV_HEATING_TEMPSENSOR1_OUTOFRANGE              	= 500030041,
	DCL_ERR_DEV_RV_HEATING_TEMPSENSOR2_OUTOFRANGE              	= 500030051,
	DCL_ERR_DEV_RV_HEATING_CURRENT_OUTOFRANGE                  	= 500030061,
	DCL_ERR_DEV_RV_HEATING_TEMPSENSOR2_NOTREACHTARGET          	= 500030071,
	DCL_ERR_DEV_RV_HEATING_TSENSOR2_LESSTHAN_30DEGREEC_OVERTIME	= 500030081,
	DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION                   	= 500030091,
	DCL_ERR_DEV_LA_PRESSURESENSOR_PRECHECK_FAILED              	= 500040001,
	DCL_ERR_DEV_LA_PRESSURESENSOR_OUTOFRANGE                   	= 500040005,
	DCL_ERR_DEV_LA_BOTTLECHECK_PRESSUREBUILD_FAILED            	= 500040011,
	DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_EMPTY                    	= 500040012,
	DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_LEAKAGE                  	= 500040013,
	DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_BLOCKAGE                 	= 500040014,
	DCL_ERR_DEV_LA_SEALING_FAILED_PRESSURE                     	= 500040021,
	DCL_ERR_DEV_LA_SEALING_FAILED_VACUUM                       	= 500040051,
	DCL_ERR_DEV_LA_FILLING_TIMEOUT_2MIN                        	= 500040101,
	DCL_ERR_DEV_LA_FILLING_TIMEOUT_4MIN                        	= 500040121,
	DCL_ERR_DEV_LA_FILLING_OVERFLOW                            	= 500040141,
	DCL_ERR_DEV_LA_FILLING_INSUFFICIENT                        	= 500040161,
	DCL_ERR_DEV_LA_DRAINING_TIMEOUT_BULIDPRESSURE              	= 500040201,
	DCL_ERR_DEV_LA_DRAINING_TIMEOUT_EMPTY_2MIN                 	= 500040221,
	DCL_ERR_DEV_LA_DRAINING_TIMEOUT_EMPTY_4MIN                 	= 500040241,
	DCL_ERR_DEV_LA_RELEASING_TIMEOUT                           	= 500040301,
	DCL_ERR_DEV_LA_STATUS_EXHAUSTFAN                           	= 500040401,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE1_ABNORMAL                  	= 500040501,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE2_ABNORMAL                  	= 500040511,
	DCL_ERR_DEV_LA_TUBEHEATING_TSENSOR1_OUTOFRANGE             	= 500040521,
	DCL_ERR_DEV_LA_TUBEHEATING_TSENSOR2_OUTOFRANGE             	= 500040531,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE1_NOTREACHTARGETTEMP        	= 500040541,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE2_NOTREACHTARGETTEMP        	= 500040551,
	DCL_ERR_DEV_MC_DC_5V_ASB3_OUTOFRANGE                       	= 500050001,
	DCL_ERR_DEV_MC_DC_5V_ASB5_OUTOFRANGE                       	= 500050011,
	DCL_ERR_DEV_MC_VOLTAGE_24V_ASB3_OUTOFRANGE                 	= 500050301,
	DCL_ERR_DEV_MC_VOLTAGE_24V_ASB5_OUTOFRANGE                 	= 500050311,
	DCL_ERR_DEV_MC_VOLTAGE_24V_ASB15_OUTOFRANGE                	= 500050321,
	DCL_ERR_DEV_MC_REMOTEALARM_UNCONNECTED                     	= 500050401,
	DCL_ERR_DEV_MC_LOCALALARM_UNCONNECTED                      	= 500050411,
	DCL_ERR_DEV_ASB5_AC_CURRENT_OUTOFRANGE                     	= 500050501,
	DCL_ERR_DEV_POWERFAILURE                                   	= 500060100,
    DCL_ERR_DEV_POWERFAILURE_AFTER_ERRHANDLING_FAILED           = 500060200,
	DCL_ERR_DEV_TISSUE_PROTECT_REPORT                          	= 500070001
} ReturnCode_t;
}

Q_DECLARE_METATYPE(DeviceControl::ReturnCode_t)
#endif
