/****************************************************************************/
/*! \file TestBaseLogger.h
 *
 *  \brief DDefinition file for class TestBaseLogger.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
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

#ifndef DATALOGGING_TESTBASELOGGER_H
#define DATALOGGING_TESTBASELOGGER_H

#include <DirectoryHelper.h>

#include <QObject>

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Test class for BaseLogger class.
 */
/****************************************************************************/
class TestBaseLogger : public QObject {
    Q_OBJECT
private:
    DirectoryHelper     m_DirHelper;        ///< Helper for cleaning up a directory.
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     *
     * We cleanup the data directory we use.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     *
     * We cleanup the data directory we use.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
    /****************************************************************************/
    /**
     * \brief Test constructor
     */
    /****************************************************************************/
    void utConstructor();
    /****************************************************************************/
    /**
     * \brief Test creation of new file. Open succeeds.
     */
    /****************************************************************************/
    void utCreateNewFileOK();
    /****************************************************************************/
    /**
     * \brief Test creation of new file. Open fails.
     */
    /****************************************************************************/
    void utCreateNewFileNOK();
    /****************************************************************************/
    /**
     * \brief Test remove of file. Remove succeeds.
     */
    /****************************************************************************/
    void utRemoveFileOK();
    /****************************************************************************/
    /**
     * \brief Test remove of file. Remove fails.
     */
    /****************************************************************************/
    void utRemoveFileNOK();
    /****************************************************************************/
    /**
     * \brief Test open for append of new file. Open succeeds.
     */
    /****************************************************************************/
    void utOpenAppendOK();
    /****************************************************************************/
    /**
     * \brief Test open for append of new file. Open fails.
     */
    /****************************************************************************/
    void utOpenAppendNOK();
    /****************************************************************************/
    /**
     * \brief Test append line.
     */
    /****************************************************************************/
    void utAppend();
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    TestBaseLogger() :
        m_DirHelper("Data/Logger")
    {
    }
}; // end class TestBaseLogger

} // end namespace DataLogging

#endif // DATALOGGING_TESTBASELOGGER_H
