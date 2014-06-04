/****************************************************************************/
/*! \file FileSelection.h
 *
 *  \brief Include file for class CFileSelection.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-06-17
 *   $Author:  $ Raju
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

#ifndef MAINMENU_FILESELECTION_H
#define MAINMENU_FILESELECTION_H

#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/FileSelectionModel.h"
#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CFileSelection;
}

/****************************************************************************/
/**
 * \brief Dialog for program prioritization
 */
/****************************************************************************/
class CFileSelection : public CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CFileSelection(QWidget *p_Parent = 0);
    virtual ~CFileSelection();
    void SetFileSelectionPanelSize(const qint32 &Width, const qint32 &Height);
    void SetRowFocus(qint32 CurrentRow);
    void SetFileList(QStringList FileList);
    void SetData(qint32 MaxItemsSelectable, QString Sepeartor, qint32 PartOfFileName, QStringList SelectableFileList
                 , QStringList OptionalSelList);
private:
    Ui::CFileSelection *mp_Ui;                               //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;                         //!< Program table
    MainMenu::CFileSelectionModel m_Model;             //!< Model for the table view    
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CFileSelection)

protected:
    void changeEvent(QEvent *p_Event);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ItemActivated(QModelIndex Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OKButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void CancelButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void RetranslateUI();

signals:
    void SelectedFileList(QStringList);

};

} // end namespace MainMenu

#endif // PROGRAMS_PRIORITIZEPROGRAMS_H
