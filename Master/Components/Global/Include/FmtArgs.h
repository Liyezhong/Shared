/****************************************************************************/
/** @file FmtArgs.h
 *
 *  @brief simple helper class for event throwing
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 09.10.2012
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
 */
/****************************************************************************/

#ifndef GLOBAL_FMTARGS_H
#define GLOBAL_FMTARGS_H

#include "Global/Include/TranslatableString.h"

namespace Global {

/**
 * @brief comfortably allow to append integer values to QStringList
 */

class FmtArgs: public Global::tTranslatableStringList
{
    public:
        /// overloaded << operator for appending integer
        FmtArgs& operator<<(const int val)
        {
            this->append(QString::number(val));
            return *this;
        }

        /// << operator for strings must be redefined for child class!
        FmtArgs& operator<<(const QString& strg)
        {
            this->append(strg);
            return *this;
        }
};

} // end namespace GLOBAL

#endif //GLOBAL_FMTARGS_H
