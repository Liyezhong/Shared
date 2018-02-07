/****************************************************************************/
/*! \file HimalayaSettingsVerifier.h
 *
 *  \brief definition file for class HimalayaSettingVerifier.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-25
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef HIMALAYA_SETTINGSVERIFIER_H
#define HIMALAYA_SETTINGSVERIFIER_H



#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"

//lint -e1540

namespace DataManager {

const int MIN_SOUND_LEVEL_ERROR = 2; ///< Minimum value for error sound level
//const int MIN_PERIODIC_TIME     = 1; ///< Minimum value for the periodic time
const int MIN_PERIODIC_TIME     = 0; ///< Minimum value for the periodic time
const int MAX_PERIODIC_TIME     = 600; ///< Maximum value for the periodic time
const int MIN_PARAFFIN_TEMP     = 50; ///< Minimum value for paraffin bath temp.
const int MAX_PARAFFIN_TEMP     = 70; ///< Maximum value for paraffin bath temp.
const int PARAFFIN_TEMP_STEP    = 5; ///< Step/interval for paraffin bath temp.

/****************************************************************************/
/*!
 *  \brief  This class implements a list of user settings verifiers.
 */
/****************************************************************************/
class CHimalayaSettingsVerifier : public CUserSettingsVerifier
{
public:
    CHimalayaSettingsVerifier();
    bool VerifyData(CDataContainerBase* p_UserSettingsInterface);  // use concrete class for concrete verifier

private:

};

}//End of namespace DataManager
#endif // HIMALAYA_SETTINGSVERIFIER_H
