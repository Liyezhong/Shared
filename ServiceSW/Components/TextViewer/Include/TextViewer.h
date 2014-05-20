/****************************************************************************/
/*! \file TextViewer.h
 *
 *  \brief Definition file for class CTextViewer.
 *  This class is implemented to read Colorado Log and service help text files.
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

#ifndef TEXTVIEWER_LOGVIEWER_H
#define TEXTVIEWER_LOGVIEWER_H

#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/MessageDlg.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QDir>
#include <QFileInfoList>
#include<QEvent>

namespace TextViewer {

namespace Ui {
    class CTextViewer;
}

/****************************************************************************/
/**
 * \brief This is implements SystemLogViewer
 */
/****************************************************************************/
class CTextViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CTextViewer(QString FileType = NULL, QString FilePath = NULL, QWidget *p_Parent = 0);
    ~CTextViewer();

    /****************************************************************************/
    /*!
     *  \brief  To add data item to the table
     *  \iparam FileName = File name
     */
    /****************************************************************************/
    void AddItem(QString FileName);


protected:
    void changeEvent(QEvent *p_Event);

private:    
    Ui::CTextViewer *mp_Ui;                                      //!< User Interface
    QString m_FileType;                                         //!< File type
    QString m_FilePath;                                         //!< File path
    MainMenu::CBaseTable *mp_TableWidget;                       //!< Table for the system log files   
    QStandardItemModel m_Model;                                 //!< Model for the System log table
    int m_CurrentIndex;                                         //!< Current Selected Index
    QVariant m_FileName;                                        //!< Stores system log file name
    MainMenu::CMessageDlg *mp_MessageDlg;                       //!< Information Message dialog    

signals:
    /****************************************************************************/
    /**
     * \brief Signal emitted for displaying log file contents
     * \iparam FileName = Log file name
     * \iparam FilePath = Log file path
     */
    /****************************************************************************/
    void DisplayFileContents(QString FileName, QString FilePath);

private slots:
    void RetranslateUI();

public slots:
    void SelectionChanged(QModelIndex Index);
    void ExecDialog();
    void UpdateFileTableEntries();
    void ResetLogFilePath();

};

} // end namespace TextViewer

#endif // TEXTVIEWER_LOGVIEWER_H
