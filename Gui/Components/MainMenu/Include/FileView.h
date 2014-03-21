/****************************************************************************/
/*! \file FileView.h
 *
 *  \brief FileView definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-29, 2012-11-07
 *   $Author:  $ M.Scherer, Raju
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

#ifndef MAINMENU_FILEVIEW_H
#define MAINMENU_FILEVIEW_H

#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/TextDialog.h"
#include "MainMenu/Include/PanelFrame.h"
#include <QStandardItemModel>
#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CFileView;
}

/****************************************************************************/
/**
 * \brief This widget allows the user to read log files.
 */
/****************************************************************************/
class CFileView :public QWidget
{
    Q_OBJECT

public:
    explicit CFileView(QWidget *p_Parent = 0);
    void SetEventViewerPanelSize(const qint32 &Width, const qint32 &Height);
    void SetCaption(QString Caption);
    void DisableButton();
    virtual ~CFileView();    

private:
    Ui::CFileView *mp_Ui;                   //!< User interface
    qint32 m_SelectableRows;                //!< Selectable rows
    qint32 m_PreviousIndex;                //!< Previous index
    MainMenu::CBaseTable *mp_TableWidget;   //!< Table displaying logfiles    
    MainMenu::CTextDialog *mp_Dialog;   //!< Text view in the dialog
    QStandardItemModel m_Model;             //!< Model for the table view


    void AddEmptyRows();

protected:
    void changeEvent(QEvent *p_Event);
private slots:
    void OpenButtonClicked();
    void ItemActivated(const QModelIndex& Index);
signals:
    void GetFileContent(const QString &FileName);

public slots:
    void DayRunLogFileContent(const QDataStream &DataStream);
    void DayRunLogFileNames(const QStringList &FileNames);    

};

} // end namespace MainMenu

#endif // MAINMENU_FILEVIEW_H
