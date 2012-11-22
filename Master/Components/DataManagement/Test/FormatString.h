/****************************************************************************/
/** @file FormatString.h
 *
 *  @brief define class for translation Global::Exception to strings
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 25.11.2010
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Jan 12 2011, 14:00:07
 *
 */
/****************************************************************************/

#ifndef DATAMANAGEMENT_FORMATSTRING_H
#define DATAMANAGEMENT_FORMATSTRING_H

#include "DataManagement/Include/ParseXML.h"
#include "Global/Include/Exception.h"
#include "Global/Include/TranslatableString.h"

namespace DataManagement {

class FormatString
{
    public:
        FormatString();
        void formatString(const Global::Exception exc, QString& res);

    private:
        QHash<int, QString> m_testErrStrings;

};

} // end namespace DataManagement

#endif // DATAMANAGEMENT_FORMATSTRING_H
