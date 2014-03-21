/****************************************************************************/
/*! \file fmStepperMotorConfig.h
 * 
 *  $Rev:    $ 0.1
 *  $Date:   $ 12.03.2012
 *  $Author: $ Rainer Boehles
 *
 *  \brief Publics for configuration functions of the function module 'stepper motor'
 *
 *         
 * <dl compact><dt>Company:</dt><dd> Leica Biosystems Nussloch GmbH </dd></dl>
 *
 * (c) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 * This is unpublished proprietary source code of Leica.
 * The copyright notice does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMSTEPPERMOTORCONFIG_H_
#define FMSTEPPERMOTORCONFIG_H_


//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/


//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/

//! evaluate and store the received parameter data block
Error_t smConfigureParam(smData_t *Data, ConfigData_Param_t *Param, UInt8 Dlc);

//! evaluate and store the received profile data block
Error_t smConfigureProfile(smProfiles_t *Profiles, UInt8 Index, ConfigData_Param_t *Param, UInt8 Dlc);

//! check if stepper configuration data is complete and valid
Error_t smConfigIsValid(smData_t* Data);



#endif /*FMSTEPPERMOTORCONFIG_H_*/
