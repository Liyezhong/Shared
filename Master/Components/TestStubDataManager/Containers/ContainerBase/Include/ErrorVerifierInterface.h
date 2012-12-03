/****************************************************************************/
/*! \file ErrorVerifierInterface.h
 *
 *  \brief Definition file for interface IErrorVerifierInterface.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-05-03
 *  $Author:    $ Raju
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
#ifndef DATAMANAGER_ERRORVERIFIERINTERFACE_H
#define DATAMANAGER_ERRORVERIFIERINTERFACE_H

#include <QString>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Used for getting error description from the verifiers
 */
/****************************************************************************/
class IErrorVerifierInterface
{

public:
    /****************************************************************************/
    /*!
     *  \brief  Resets the last error which is done by verifier
     */
    /****************************************************************************/
    virtual void ResetLastError() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Gets the last error which is done by verifier
     *
     *  \iparam ErrorDescription = Description of the error
     *
     *  \iparam ErrorSource = Gives the source file name of the error
     *
     *  \return Successful (true) or not (false)
     */
    /****************************************************************************/
    virtual bool GetLastError(QString& ErrorDescription, QString& ErrorSource) = 0;

};
} // namespace DataManager

#endif // ERRORVERIFIERINTERFACE_H
