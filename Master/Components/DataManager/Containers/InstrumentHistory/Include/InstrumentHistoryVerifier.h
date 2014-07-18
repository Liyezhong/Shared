/****************************************************************************/
/*! \file InstrumentHistoryVerifier.h
 *
 *  \brief Definition for CInstrumentHistoryVerifier class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-08
 *   $Author:  $ Soumya D , Shuvasmita
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
#ifndef DATAMANAGER_INSTRUMENTHISTORYVERIFIER_H
#define DATAMANAGER_INSTRUMENTHISTORYVERIFIER_H

#include <QStringList>
#include <QDebug>

#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include "DataManager/Containers/InstrumentHistory/Include/InstrumentHistory.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief  This class implements Module List verifier
 */
/****************************************************************************/
class CInstrumentHistoryVerifier :public IVerifierInterface
{
public:
    CInstrumentHistoryVerifier();
    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CInstrumentHistoryVerifier() {}

    bool VerifyData(const CDataContainerBase* p_InstrumentHistory);
    ErrorMap_t &GetErrors();

    void ResetErrors();

    bool IsLocalVerifier() const;

private:
    CInstrumentHistory *mp_MDL;  //!< Module List Container
    ErrorMap_t m_ErrorMap; //!< To store Error ID and any arguments associated

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CInstrumentHistoryVerifier)

};

}
#endif // DATAMANAGER_INSTRUMENTHISTORYVERIFIER_H
