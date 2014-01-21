/****************************************************************************/
/*! \file FileView.h
 *
 *  \brief Header file for class CFileView.
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

//lint -e429

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
    friend class  CTestMainMenu;

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
    MainMenu::CTextDialog *mp_Dialog;       //!< Text view in the dialog
    QStandardItemModel m_Model;             //!< Model for the table view

    void AddEmptyRows();
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CFileView)

protected:
    void changeEvent(QEvent *p_Event);

private slots:
    void OpenButtonClicked();
    void ItemActivated(const QModelIndex& Index);
    void RetranslateUI();

signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when Open button is pressed in File View widget.
     *
     *  \iparam FileName = Name of the file.
     */
    /****************************************************************************/
    void GetFileContent(const QString &FileName);

    /****************************************************************************/
    /**
     * \brief Hides the wait dialog
     *
     */
    /****************************************************************************/
    void HideDialog();

    /****************************************************************************/
    /**
     * \brief Shows the wait dialog
     *
     */
    /****************************************************************************/
    void ShowDialog();

public slots:
    void DayRunLogFileContent(const QDataStream &DataStream);
    void DayRunLogFileNames(const QStringList &FileNames);

};

} // end namespace MainMenu

#endif // MAINMENU_FILEVIEW_H
