/****************************************************************************/
/*! \file LogViewer.h
 *
 *  \brief Definition file for class CSystemLogViewer.
 *  This class is implemented to read Colorado Log files.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-16
 *   $Author:  $ Soumya. D
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

#ifndef LOGVIEWER_LOGVIEWER_H
#define LOGVIEWER_LOGVIEWER_H

#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/MessageDlg.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QDir>
#include <QFileInfoList>
#include<QEvent>

namespace LogViewer {

namespace Ui {
    class CLogViewer;
}

/****************************************************************************/
/**
 * \brief This is implements SystemLogViewer
 */
/****************************************************************************/
class CLogViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CLogViewer(QString LogFileName = NULL, QString LogFilePath = NULL, QWidget *p_Parent = 0);
    ~CLogViewer();

    /****************************************************************************/
    /*!
     *  \brief  To add data item to the table
     *  \iparam LogFileName = System log file name
     */
    /****************************************************************************/
    void AddItem(QString LogFileName);


protected:
    void changeEvent(QEvent *p_Event);

private:    
    Ui::CLogViewer *mp_Ui;                                      //!< User Interface
    QString m_LogFileType;                                      //!< Log file type
    QString m_LogFilePath;                                      //!< Log file path
    MainMenu::CBaseTable *mp_TableWidget;                       //!< Table for the system log files   
    QStandardItemModel m_Model;                                 //!< Model for the System log table
    int m_CurrentIndex;                                         //!< Current Selected Index
    QVariant m_LogFileName;                                     //!< Stores system log file name
    MainMenu::CMessageDlg *mp_MessageDlg;                       //!< Information Message dialog    

signals:
    /****************************************************************************/
    /**
     * \brief Signal emitted for displaying log file contents
     * \iparam FileName = Log file name
     * \iparam FilePath = Log file path
     */
    /****************************************************************************/
    void DisplayLogFileContents(QString FileName, QString FilePath);

private slots:
    void RetranslateUI();

public slots:
    void SelectionChanged(QModelIndex Index);
    void ExecDialog();
    void UpdateLogFileTableEntries();
    void ResetLogFilePath();

};

} // end namespace LogViewer

#endif // LOGVIEWER_LOGVIEWER_H
