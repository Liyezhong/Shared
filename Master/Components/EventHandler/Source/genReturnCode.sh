#!/bin/sh

xmlFile=../../../../../Himalaya/HimalayaMain/Master/Components/Main/Build/Settings/EventScenarioErrorMap.xml
targetFile=../../DeviceControl/Include/Global/DeviceControlReturnCode.h

RetCodeGen=$(find $PWD -name ReturnCodeGenerator | xargs ls -rtl | tail -n 1 | awk '{print $9}')

rm -rf tmp
$RetCodeGen $xmlFile tmp

echo "#ifndef DEVICE_CONTROL_RETURNCODE_H" > $targetFile
echo "#define DEVICE_CONTROL_RETURNCODE_H" >> $targetFile
echo "\nnamespace DeviceControl\n{\ntypedef enum {\n" >> $targetFile
echo "\tDCL_ERR_FCT_CALL_SUCCESS      = 0x0000,  //!< function was executed successfully" >> $targetFile
echo "\tDCL_ERR_FCT_CALL_FAILED       = 0x0001,  //!< function was failed, no further information available" >> $targetFile
echo "\tDCL_ERR_FCT_CALL_NOT_FOUND    = 0x0002,  //!< inside the function, a parameter concerning action was not done because anything was not found" >> $targetFile
echo "\tDCL_ERR_FCT_NOT_IMPLEMENTED   = 0x0003,  //!< Temporary return value for bare functions, should be removed at the end" >> $targetFile
echo "\tDCL_ERR_INVALID_STATE         = 0x0010,  //!< a method was called while the internal state machine was not able to process the functionality" >> $targetFile
echo "\tDCL_ERR_NOT_INITIALIZED       = 0x0011,  //!< A method was called before complete initialisation was done" >> $targetFile
echo "\tDCL_ERR_TIMEOUT               = 0x0012,  //!< A timeout appeared (typically a CAN-Bus timeout, while waiting for an answer)" >> $targetFile
echo "\tDCL_ERR_INVALID_PARAM         = 0x0013,  //!< One ore more invalid parameters were passed to the method" >> $targetFile
echo "\tDCL_ERR_INTERNAL_ERR          = 0x0014,  //!< The error reason is internal, further information is available at ..." >> $targetFile
echo "\tDCL_ERR_EXTERNAL_ERROR        = 0x0015,  //!< The error reason is external, it was reported via CAN-bus" >> $targetFile
echo "\tDCL_ERR_EXTERNAL_WARNING      = 0x0016,  //!< The warning reason is external, it was reported via CAN-bus" >> $targetFile
echo "\tDCL_ERR_EXTERNAL_INFO         = 0x0017,  //!< The information reason is external, it was reported via CAN-bus" >> $targetFile
echo "\tDCL_ERR_NULL_PTR_ACCESS       = 0x0018,  //!< A null pointer would be accessed, but there was an instruction to avoid its direct access" >> $targetFile
echo "\tDCL_ERR_CANMSG_INVALID        = 0x0020,  //!< An invalid CAN-message was received (e.g. dlc not correct)" >> $targetFile
echo "\tDCL_ERR_CANBUS_NOT_READY      = 0x0021,  //!< A CAN-message was tried to be send while the CAN bus is in error state" >> $targetFile
echo "\tDCL_ERR_CANBUS_ERROR          = 0x0022,  //!< A CAN-message was tried to be send while an 'unknown' CAN bus error is active" >> $targetFile
echo "\tDCL_ERR_UNEXPECTED_BREAK      = 0x0023,   //!< Current program was breaked by other program" >> $targetFile
echo "\tDCL_ERR_SNYC_CALL_BUSY        = 0x0024,   //!< Current program was breaked by other program" >> $targetFile
echo "\tDCL_ERR_TIMER_TIMEOUT         = 0x0025," >> $targetFile

cat tmp | while read line
do
	echo "\t$line" >> $targetFile
done
rm -rf tmp

echo "} RetunCode_t;\n}" >> $targetFile
echo "\nQ_DECLARE_METATYPE(DeviceControl::ReturnCode_t)" >> $targetFile
echo "#endif" >> $targetFile
