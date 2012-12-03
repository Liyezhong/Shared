// ================================================================================================
/*! @file Platform/Master/Components/ExternalUtilities/Test/TestExternalUtilities.cpp
 *
 *  @brief
 *  Pseudo Unit-Test that should always succeed.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2011-01-31
 *  $Author:    $ Martin Weitzel
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 * ================================================================================================
*/
#include <QTest>

#include "ExternalUtilities/Include/_ExternalUtilities.h"

namespace ExternalUtilities {
namespace UnitTest {

    /*!
     * Unit Test for Classes in the ExternalUtilities Namespace.
     *
     * (For further information see source code.)
    */
    class TestExternalUtilities: public QObject {

        Q_OBJECT

    private slots:

        /**
         * Empty test (just assert files are compile-clean).
        */
        void CompileClean() {
        }
    };

}} // EONS ::ExternalUtilities::UnitTest

QTEST_MAIN(ExternalUtilities::UnitTest::TestExternalUtilities)
#include "TestExternalUtilities.moc"
