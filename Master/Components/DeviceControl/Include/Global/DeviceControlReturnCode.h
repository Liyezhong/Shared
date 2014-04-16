#ifndef DEVICE_CONTROL_RETURNCODE_H
#define DEVICE_CONTROL_RETURNCODE_H

namespace DeviceControl
{
typedef enum {

	DCL_ERR_FCT_CALL_SUCCESS      			= 0x0000,  //!< function was executed successfully
	DCL_ERR_FCT_CALL_FAILED       			= 0x0001,  //!< function was failed, no further information available
	DCL_ERR_UNDEFINED              			= 0x0002,  //!< The return code was NOT set
	DCL_ERR_FCT_CALL_NOT_FOUND    			= 0x0003,  //!< inside the function, a parameter concerning action was not done because anything was not found
	DCL_ERR_FCT_NOT_IMPLEMENTED   			= 0x0004,  //!< Temporary return value for bare functions, should be removed at the end
	DCL_ERR_INVALID_STATE         			= 0x0005,  //!< a method was called while the internal state machine was not able to process the functionality
	DCL_ERR_NOT_INITIALIZED       			= 0x0006,  //!< A method was called before complete initialisation was done
	DCL_ERR_TIMEOUT               			= 0x0007,  //!< A timeout appeared (typically a CAN-Bus timeout, while waiting for an answer)
	DCL_ERR_INVALID_PARAM         			= 0x0008,  //!< One ore more invalid parameters were passed to the method
	DCL_ERR_INTERNAL_ERR          			= 0x0009,  //!< The error reason is internal, further information is available at ...
	DCL_ERR_EXTERNAL_ERROR        			= 0x0010,  //!< The error reason is external, it was reported via CAN-bus
	DCL_ERR_EXTERNAL_WARNING      			= 0x001A,  //!< The warning reason is external, it was reported via CAN-bus
	DCL_ERR_EXTERNAL_INFO         			= 0x001B,  //!< The information reason is external, it was reported via CAN-bus
	DCL_ERR_NULL_PTR_ACCESS       			= 0x001C,  //!< A null pointer would be accessed, but there was an instruction to avoid its direct access
	DCL_ERR_CANMSG_INVALID        			= 0x001D,  //!< An invalid CAN-message was received (e.g. dlc not correct)
	DCL_ERR_CANBUS_NOT_READY      			= 0x001E,  //!< A CAN-message was tried to be send while the CAN bus is in error state
	DCL_ERR_CANBUS_ERROR          			= 0x001F,  //!< A CAN-message was tried to be send while an 'unknown' CAN bus error is active
	DCL_ERR_UNEXPECTED_BREAK      			= 0x0020,  //!< Current program was breaked by other program
	DCL_ERR_SNYC_CALL_BUSY        			= 0x0021,  //!< Current program was breaked by other program
	DCL_ERR_TIMER_TIMEOUT         			= 0x0022,  //!< Timer Timout
	DCL_ERR_CANCOMMUTOR_START           	= 0x0023,  //!< CAN Communicator start
	DCL_ERR_CANCOMMUTOR_INTERFACE_OPEN  	= 0x0024,  //!< CAN Communicator Interface Open
	DCL_ERR_CANCOMMUTOR_TCPCLIENT_INIT  	= 0x0025,  //!< CAN Communicator TCP Client Initialized
	DCL_ERR_CANCOMMUTOR_COMM_FAILED     	= 0x0026,  //!< CAN Communicator communication failed
	DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_0	= 0x0027,  //!< Get level sensor state 0
	DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1	= 0x0028,  //!< Get level sensor state 1
	DCL_ERR_DEV_TEMP_CTRL_STATE_ERR			= 0x0029,  //!< Temperatuer control state error
	DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON		= 0x002A,  //!< Temperatuer control is already on
	DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR		= 0x002B,  //!< Temperatuer control set temperature error
	DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR		= 0x002C,  //!< Temperatuer control set state error
	DCL_ERR_DEV_RETORT_BOTTOM_SIDELOW_HEATING_ELEMENT_FAILED   	= 0x0060,
	DCL_ERR_DEV_RETORT_SIDTOP_SIDEMID_HEATING_ELEMENT_FAILED   	= 0x0061,
	DCL_ERR_DEV_RETORT_TSENSOR1_TEMPERATURE_OVERANGE           	= 0x0062,
	DCL_ERR_DEV_RETORT_TSENSOR1_TEMPERATURE_NOSIGNAL           	= 0x0063,
	DCL_ERR_DEV_RETORT_TSENSOR2_TEMPERATURE_OVERRANGE          	= 0x0064,
	DCL_ERR_DEV_RETORT_TSENSOR2_TEMPERATURE_NOSIGNAL           	= 0x0065,
	DCL_ERR_DEV_RETORT_TSENSOR3_TEMPERATURE_OVERRANGE          	= 0x0066,
	DCL_ERR_DEV_RETORT_TSENSOR3_TEMPERATURE_NOSIGNAL           	= 0x0067,
	DCL_ERR_DEV_RETORT_TSENSOR1_TO_2_SELFCALIBRATION_FAILED    	= 0x0068,
	DCL_ERR_DEV_RETORT_LEVELSENSOR_HEATING_OVERTIME            	= 0x0069,
	DCL_ERR_DEV_RETORT_LEVELSENSOR_TEMPERATURE_OVERRANGE       	= 0x006A,
	DCL_ERR_DEV_RETORT_LIDLOCK_CLOSE_STATUS_ERROR              	= 0x006B,
	DCL_ERR_DEV_WAXBATH_TOP_HEATINGPAD_CURRENT_OUTOFRANGE      	= 0x006C,
	DCL_ERR_DEV_WAXBATH_BOTTOM_HEATINGPAD_CURRENT_OUTOFRANGE   	= 0x006D,
	DCL_ERR_DEV_WAXBATH_TSENSORUP_OUTOFRANGE                   	= 0x006E,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN1_OUTOFRANGE                	= 0x006F,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN2_OUTOFRANGE                	= 0x0070,
	DCL_ERR_DEV_WAXBATH_TSENSORUP_HEATING_ABNORMAL             	= 0x0071,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN1_HEATING_ABNORMAL          	= 0x0072,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN2_HEATING_ABNORMAL          	= 0x0073,
	DCL_ERR_DEV_WAXBATH_TSENSORUP_HEATING_OUTOFTARGETRANGE     	= 0x0074,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN1_HEATING_OUTOFTARGETRANGE  	= 0x0075,
	DCL_ERR_DEV_WAXBATH_TSENSORDOWN2_HEATING_OUTOFTARGETRANGE  	= 0x0076,
	DCL_ERR_DEV_WAXBATH_OVENCOVER_STATUWS_OPEN                 	= 0x0077,
	DCL_ERR_DEV_RV_MOTOR_CANNOTGET_ORIGINALPOSITION            	= 0x0078,
	DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_RETRY                   	= 0x0079,
	DCL_ERR_DEV_RV_MOTOR_INTERNALSTEPS_EXCEEDUPPERLIMIT        	= 0x007A,
	DCL_ERR_DEV_RV_HEATING_TEMPSENSOR1_OUTOFRANGE              	= 0x007B,
	DCL_ERR_DEV_RV_HEATING_TEMPSENSOR2_OUTOFRANGE              	= 0x007C,
	DCL_ERR_DEV_RV_HEATING_CURRENT_OUTOFRANGE                  	= 0x007D,
	DCL_ERR_DEV_RV_HEATING_TEMPSENSOR2_NOTREACHTARGET          	= 0x007E,
	DCL_ERR_DEV_RV_HEATING_TSENSOR2_LESSTHAN_30DEGREEC_OVERTIME	= 0x007F,
	DCL_ERR_DEV_RV_MOTOR_LOSTCURRENTPOSITION                   	= 0x0080,
	DCL_ERR_DEV_LA_PRESSURESENSOR_PRECHECK_FAILED              	= 0x0081,
	DCL_ERR_DEV_LA_PRESSURESENSOR_OUTOFRANGE                   	= 0x0082,
	DCL_ERR_DEV_LA_BOTTLECHECK_PRESSUREBUILD_FAILED            	= 0x0083,
	DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_EMPTY                    	= 0x0084,
	DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_INSUFFICIENT             	= 0x0085,
	DCL_ERR_DEV_LA_BOTTLECHECK_FAILED_BLOCKAGE                 	= 0x0086,
	DCL_ERR_DEV_LA_SEALING_FAILED_PRESSURE                     	= 0x0087,
	DCL_ERR_DEV_LA_SEALING_FAILED_VACCUUM                      	= 0x0088,
	DCL_ERR_DEV_LA_FILLING_TIMEOUT_2MIN                        	= 0x0089,
	DCL_ERR_DEV_LA_FILLING_TIMEOUT_4MIN                        	= 0x008A,
	DCL_ERR_DEV_LA_FILLING_OVERFLOW                            	= 0x008B,
	DCL_ERR_DEV_LA_FILLING_INSUFFICIENT                        	= 0x008C,
	DCL_ERR_DEV_LA_DRAINING_TIMEOUT_BUILDPRESSURE              	= 0x008D,
	DCL_ERR_DEV_LA_DRAINING_TIMEOUT_EMPTY_2MIN                 	= 0x008E,
	DCL_ERR_DEV_LA_DRAINING_TIMEOUT_EMPTY_4MIN                 	= 0x008F,
	DCL_ERR_DEV_LA_RELEASING_TIMEOUT                           	= 0x0090,
	DCL_ERR_DEV_LA_STATUS_EXHAUSTFAN                           	= 0x0091,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE1_ABNORMAL                  	= 0x0092,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE2_ABNORMAL                  	= 0x0093,
	DCL_ERR_DEV_LA_TUBEHEATING_TSENSOR1_OUTOFRANGE             	= 0x0094,
	DCL_ERR_DEV_LA_TUBEHEATING_TSENSOR2_OUTOFRANGE             	= 0x0095,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE1_NOTREACHTARGETTEMP        	= 0x0096,
	DCL_ERR_DEV_LA_TUBEHEATING_TUBE2_NOTREACHTARGETTEMP        	= 0x0097,
	DCL_ERR_DEV_MC_DC_5V_ASB3_OUTOFRANGE                       	= 0x0098,
	DCL_ERR_DEV_MC_DC_5V_ASB5_OUTOFRANGE                       	= 0x0099,
	DCL_ERR_DEV_MC_DC_5V_ASB15_OUTOFRANGE                      	= 0x009A,
	DCL_ERR_DEV_MC_VOLTAGE_24V_ASB3_OUTOFRANGE                 	= 0x009B,
	DCL_ERR_DEV_MC_VOLTAGE_24V_ASB5_OUTOFRANGE                 	= 0x009C,
	DCL_ERR_DEV_MC_VOLTAGE_24V_ASB15_OUTOFRANGE                	= 0x009D,
	DCL_ERR_DEV_MC_REMOTEALARM_UNCONNECTED                     	= 0x009E,
	DCL_ERR_DEV_MC_LOCALALARM_UNCONNECTED                      	= 0x009F
} ReturnCode_t;
}

Q_DECLARE_METATYPE(DeviceControl::ReturnCode_t)
#endif
