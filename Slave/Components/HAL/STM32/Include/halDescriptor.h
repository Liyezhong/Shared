/***********************************************************************************/
/*! \file halDescriptor.h
 *
 *  \brief  HAL configuration descriptors
 *
 *  $Version: $ 0.1
 *  $Date:    $ 26.07.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      The HAL is configured to hardware by descriptor tables for the various 
 *      classes of hardware devices. The descriptor tables are defined in the 
 *      HAL configuration file linked in when the firmware for a certain hard-
 *      ware board gets build.
 *
 *      This module contain weak declarations of those tables to be used, if
 *      one or multiple descriptor tables are not defined in the configuration
 *      file, due to the fact, that a hardware class is not present on a
 *      certain hardware board.
 *
 *      The descriptors declared here are all empty, indicating the HAL that
 *      a ressource is not available.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/***********************************************************************************/

#ifndef HAL_DESCRIPTOR_H
#define HAL_DESCRIPTOR_H

//****************************************************************************/
// External Descriptors (residing in halConfigure.c)
//****************************************************************************/

extern const halPinDesciptor_t halPinDescriptors[];
extern const UInt32 halPinDescriptorCount;

extern const halPortDescriptor_t halPortDescriptors[];
extern const UInt32 halPortDescriptorCount;

extern const halAnalogDescriptor_t halAnalogDescriptors[];
extern const UInt32 halAnalogDescriptorCount;

extern const halExtIntrDescriptor_t halExtIntrDescriptors[];
extern const UInt32 halExtIntrDescriptorCount;
                    
extern const halTimerDescriptor_t halTimerDescriptors[];
extern const UInt32 halTimerDescriptorCount;

extern const halStepperDescriptor_t halStepperDescriptors[];
extern const UInt32 halStepperDescriptorCount;

extern const halStorageDescriptor_t halStorageDescriptors[];
extern const UInt32 halStorageDescriptorCount;

extern const halSerialDescriptor_t halSerialDescriptors[];
extern const UInt32 halSerialDescriptorCount;

extern const halAd779xDescriptor_t halAd779xDescriptors[];
extern const UInt32 halAd779xDescriptorCount;

extern const halProcessorInfo_t halProcessorInfo;
extern const halPinMapping_t halPinMappings;

extern const UInt32 halSystemFrequency;
extern const UInt32 halCrystalFrequency;
extern const UInt32 halRtcClockFrequency;
extern const UInt32 halProcessorVoltage;
extern const UInt32 halWatchdogInterval;
extern const UInt32 halNonVolatileSize;
extern const UInt32 halNonVolatileCache;

//****************************************************************************/

#endif /*HAL_DESCRIPTOR_H*/
