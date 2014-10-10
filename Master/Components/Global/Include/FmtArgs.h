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
#include <QStringList>

namespace Global {

/**
 * @brief comfortably allow to append integer values to QStringList
 */

class FmtArgs: public Global::tTranslatableStringList
{
    public:
        /**
          * \brief operator <<
          * \param val val
          * \return FmtArgs
        */
        FmtArgs& operator<<(const int val)
        {
            this->append(QString::number(val));
            return *this;
        }

        /**
          * \brief operator <<
          * \param strg string
          * \return FmtArgs
        */
        FmtArgs& operator<<(const QString& strg)
        {
            this->append(strg);
            return *this;
        }

        /**
          * \brief operator <<
          * \param strgList string list
          * \return FmtArgs
        */
        FmtArgs& operator<<(const QStringList& strgList)
        {
            foreach (QString strg, strgList) {
                this->append(strg);
            }
            return *this;
        }

};

} // end namespace GLOBAL

#endif //GLOBAL_FMTARGS_H
