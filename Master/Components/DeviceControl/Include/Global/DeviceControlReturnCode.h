#ifndef DEVICE_CONTROL_RETURNCODE_H
#define DEVICE_CONTROL_RETURNCODE_H

namespace DeviceControl
{
typedef enum {

	DCL_ERR_FCT_CALL_SUCCESS      = 0x0000,  //!< function was executed successfully
	DCL_ERR_FCT_CALL_FAILED       = 0x0001,  //!< function was failed, no further information available
	DCL_ERR_FCT_CALL_NOT_FOUND    = 0x0002,  //!< inside the function, a parameter concerning action was not done because anything was not found
	DCL_ERR_FCT_NOT_IMPLEMENTED   = 0x0003,  //!< Temporary return value for bare functions, should be removed at the end
	DCL_ERR_INVALID_STATE         = 0x0010,  //!< a method was called while the internal state machine was not able to process the functionality
	DCL_ERR_NOT_INITIALIZED       = 0x0011,  //!< A method was called before complete initialisation was done
	DCL_ERR_TIMEOUT               = 0x0012,  //!< A timeout appeared (typically a CAN-Bus timeout, while waiting for an answer)
	DCL_ERR_INVALID_PARAM         = 0x0013,  //!< One ore more invalid parameters were passed to the method
	DCL_ERR_INTERNAL_ERR          = 0x0014,  //!< The error reason is internal, further information is available at ...
	DCL_ERR_EXTERNAL_ERROR        = 0x0015,  //!< The error reason is external, it was reported via CAN-bus
	DCL_ERR_EXTERNAL_WARNING      = 0x0016,  //!< The warning reason is external, it was reported via CAN-bus
	DCL_ERR_EXTERNAL_INFO         = 0x0017,  //!< The information reason is external, it was reported via CAN-bus
	DCL_ERR_NULL_PTR_ACCESS       = 0x0018,  //!< A null pointer would be accessed, but there was an instruction to avoid its direct access
	DCL_ERR_CANMSG_INVALID        = 0x0020,  //!< An invalid CAN-message was received (e.g. dlc not correct)
	DCL_ERR_CANBUS_NOT_READY      = 0x0021,  //!< A CAN-message was tried to be send while the CAN bus is in error state
	DCL_ERR_CANBUS_ERROR          = 0x0022,  //!< A CAN-message was tried to be send while an 'unknown' CAN bus error is active
	DCL_ERR_UNEXPECTED_BREAK      = 0x0023,   //!< Current program was breaked by other program
	DCL_ERR_SNYC_CALL_BUSY        = 0x0024,   //!< Current program was breaked by other program
	DCL_ERR_TIMER_TIMEOUT         = 0x0025,
	DCL_ERR_RETORT_BOTTOM_SIDELOW_HEATING_ELEMENT_FAILED   	= 0x0030,
	DCL_ERR_RETORT_SIDTOP_SIDEMID_HEATING_ELEMENT_FAILED   	= 0x0031,
	DCL_ERR_RETORT_TSENSOR1_TEMPERATURE_OVERANGE           	= 0x0032,
	DCL_ERR_RETORT_TSENSOR1_TEMPERATURE_NOSIGNAL           	= 0x0033,
	DCL_ERR_RETORT_TSENSOR2_TEMPERATURE_OVERRANGE          	= 0x0034,
	DCL_ERR_RETORT_TSENSOR2_TEMPERATURE_NOSIGNAL           	= 0x0035,
	DCL_ERR_RETORT_TSENSOR3_TEMPERATURE_OVERRANGE          	= 0x0036,
	DCL_ERR_RETORT_TSENSOR3_TEMPERATURE_NOSIGNAL           	= 0x0037,
	DCL_ERR_RETORT_TSENSOR1_TO_2_SELFCALIBRATION_FAILED    	= 0x0038,
	DCL_ERR_RETORT_LEVELSENSOR_HEATING_OVERTIME            	= 0x0039,
	DCL_ERR_RETORT_LEVELSENSOR_TEMPERATURE_OVERRANGE       	= 0x003A,
	DCL_ERR_RETORT_LIDLOCK_CLOSE_STATUS_ERROR              	= 0x003B,
	DCL_ERR_WAXBATH_TOP_HEATINGPAD_CURRENT_OUTOFRANGE      	= 0x003C,
	DCL_ERR_WAXBATH_BOTTOM_HEATINGPAD_CURRENT_OUTOFRANGE   	= 0x003D,
	DCL_ERR_WAXBATH_TSENSORUP_OUTOFRANGE                   	= 0x003E,
	DCL_ERR_WAXBATH_TSENSORDOWN1_OUTOFRANGE                	= 0x003F,
	DCL_ERR_WAXBATH_TSENSORDOWN2_OUTOFRANGE                	= 0x0040,
	DCL_ERR_WAXBATH_TSENSORUP_HEATING_ABNORMAL             	= 0x0041,
	DCL_ERR_WAXBATH_TSENSORDOWN1_HEATING_ABNORMAL          	= 0x0042,
	DCL_ERR_WAXBATH_TSENSORDOWN2_HEATING_ABNORMAL          	= 0x0043,
	DCL_ERR_WAXBATH_TSENSORUP_HEATING_OUTOFTARGETRANGE     	= 0x0044,
	DCL_ERR_WAXBATH_TSENSORDOWN1_HEATING_OUTOFTARGETRANGE  	= 0x0045,
	DCL_ERR_WAXBATH_TSENSORDOWN2_HEATING_OUTOFTARGETRANGE  	= 0x0046,
	DCL_ERR_WAXBATH_OVENCOVER_STATUWS_OPEN                 	= 0x0047,
	DCL_ERR_RV_MOTOR_CANNOTGET_ORIGINALPOSITION            	= 0x0048,
	DCL_ERR_RV_MOTOR_INTERNALSTEPS_RETRY                   	= 0x0049,
	DCL_ERR_RV_MOTOR_INTERNALSTEPS_EXCEEDUPPERLIMIT        	= 0x004A,
	DCL_ERR_RV_HEATING_TEMPSENSOR1_OUTOFRANGE              	= 0x004B,
	DCL_ERR_RV_HEATING_TEMPSENSOR2_OUTOFRANGE              	= 0x004C,
	DCL_ERR_RV_HEATING_CURRENT_OUTOFRANGE                  	= 0x004D,
	DCL_ERR_RV_HEATING_TEMPSENSOR2_NOTREACHTARGET          	= 0x004E,
	DCL_ERR_RV_HEATING_TSENSOR2_LESSTHAN_30DEGREEC_OVERTIME	= 0x004F,
	DCL_ERR_RV_MOTOR_LOSTCURRENTPOSITION                   	= 0x0050,
	DCL_ERR_LA_PRESSURESENSOR_PRECHECK_FAILED              	= 0x0051,
	DCL_ERR_LA_PRESSURESENSOR_OUTOFRANGE                   	= 0x0052,
	DCL_ERR_LA_BOTTLECHECK_PRESSUREBUILD_FAILED            	= 0x0053,
	DCL_ERR_LA_BOTTLECHECK_FAILED_EMPTY                    	= 0x0054,
	DCL_ERR_LA_BOTTLECHECK_FAILED_INSUFFICIENT             	= 0x0055,
	DCL_ERR_LA_BOTTLECHECK_FAILED_BLOCKAGE                 	= 0x0056,
	DCL_ERR_LA_SEALING_FAILED_PRESSURE                     	= 0x0057,
	DCL_ERR_LA_SEALING_FAILED_VACCUUM                      	= 0x0058,
	DCL_ERR_LA_FILLING_TIMEOUT_2MIN                        	= 0x0059,
	DCL_ERR_LA_FILLING_TIMEOUT_4MIN                        	= 0x005A,
	DCL_ERR_LA_FILLING_OVERFLOW                            	= 0x005B,
	DCL_ERR_LA_FILLING_INSUFFICIENT                        	= 0x005C,
	DCL_ERR_LA_DRAINING_TIMEOUT_BUILDPRESSURE              	= 0x005D,
	DCL_ERR_LA_DRAINING_TIMEOUT_EMPTY_2MIN                 	= 0x005E,
	DCL_ERR_LA_DRAINING_TIMEOUT_EMPTY_4MIN                 	= 0x005F,
	DCL_ERR_LA_RELEASING_TIMEOUT                           	= 0x0060,
	DCL_ERR_LA_STATUS_EXHAUSTFAN                           	= 0x0061,
	DCL_ERR_LA_TUBEHEATING_TUBE1_ABNORMAL                  	= 0x0062,
	DCL_ERR_LA_TUBEHEATING_TUBE2_ABNORMAL                  	= 0x0063,
	DCL_ERR_LA_TUBEHEATING_TSENSOR1_OUTOFRANGE             	= 0x0064,
	DCL_ERR_LA_TUBEHEATING_TSENSOR2_OUTOFRANGE             	= 0x0065,
	DCL_ERR_LA_TUBEHEATING_TUBE1_NOTREACHTARGETTEMP        	= 0x0066,
	DCL_ERR_LA_TUBEHEATING_TUBE2_NOTREACHTARGETTEMP        	= 0x0067,
	DCL_ERR_MC_DC_5V_ASB3_OUTOFRANGE                       	= 0x0068,
	DCL_ERR_MC_DC_5V_ASB5_OUTOFRANGE                       	= 0x0069,
	DCL_ERR_MC_DC_5V_ASB15_OUTOFRANGE                      	= 0x006A,
	DCL_ERR_MC_VOLTAGE_24V_ASB3_OUTOFRANGE                 	= 0x006B,
	DCL_ERR_MC_VOLTAGE_24V_ASB5_OUTOFRANGE                 	= 0x006C,
	DCL_ERR_MC_VOLTAGE_24V_ASB15_OUTOFRANGE                	= 0x006D,
	DCL_ERR_MC_REMOTEALARM_UNCONNECTED                     	= 0x006E,
	DCL_ERR_MC_LOCALALARM_UNCONNECTED                      	= 0x006F
} RetunCode_t;
}

Q_DECLARE_METATYPE(DeviceControl::ReturnCode_t)
#endif
