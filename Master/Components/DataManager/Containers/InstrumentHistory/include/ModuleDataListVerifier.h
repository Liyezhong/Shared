/****************************************************************************/
/*! \file ModuleDataListVerifier.h
 *
 *  \brief Definition for ModuleDataListVerifier class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-08
 *   $Author:  $ Soumya D
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef MODULEDATALISTVERIFIER_H
#define MODULEDATALISTVERIFIER_H

#include "ModuleDataList.h"
#include <QStringList>
#include <QDebug>

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief  This class implements Module List verifier
 */
/****************************************************************************/
class ModuleDataListVerifier
{
public:
    ModuleDataListVerifier();

    bool VerifyData(CModuleDataList* p_ModuleDataList);

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~ModuleDataListVerifier() {}

private:
    CModuleDataList *mp_MDL; //!< Module List Container
};

}
#endif // MODULEDATALISTVERIFIER_H
