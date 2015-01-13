/****************************************************************************/
/*! \file MessageDlg.h
 *
 *  \brief Header file for class CMessageDlg.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
 *   $Author:  $ M.Scherer
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

#ifndef MAINMENU_MESSAGEDLG_H
#define MAINMENU_MESSAGEDLG_H

#include "MainMenu/Include/DialogFrame.h"
#include <QMessageBox>

namespace MainMenu {

namespace Ui {
    class CMessageDlg;
}

/****************************************************************************/
/**
 * \brief This class displays a information, warning or error message box.
 */
/****************************************************************************/
class CMessageDlg : public CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

private:
    Ui::CMessageDlg *mp_Ui; //!< User interface
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CMessageDlg)

public:
    //! Enum describes the message severity
    typedef enum {
        Information,
        Warning,
        Critical
    } Severity_t;

    explicit CMessageDlg(QWidget *p_Parent = NULL);
    virtual ~CMessageDlg();
    void SetTitle(const QString &TitleLeft);
    void SetTitle(const QString &TitleLeft, const QString &TitleRight);
    void SetText(const QString &Text);
    void SetIcon(QMessageBox::Icon);
    void SetButtonText(qint32 ButtonNumber, QString ButtonText);
    void EnableButton(qint32 ButtonNumber, bool IsEnable);
    void HideCenterButton();
    void HideButtons();
    void HideButtonsOneAndTwo();
    void HideAllButtons();

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void Show();

private slots:
    /****************************************************************************/
    /**
     * \brief Slot called when Left corner button is clicked
     */
    /****************************************************************************/
    void OnButtonLeftClicked()
    {
        emit ButtonLeftClicked();
     reject();
    }

    /****************************************************************************/
    /**
     * \brief Slot called when center button is clicked
     */
    /****************************************************************************/
    void OnButtonCenterClicked()
    {
    emit ButtonCenterClicked();
    accept();
    }

    /****************************************************************************/
    /**
     * \brief Slot called when right corner button is clicked
     */
    /****************************************************************************/
    void OnButtonRightClicked()
    {
    emit ButtonRightClicked();
        accept();
    }

signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted when right corner button is clicked
     */
    /****************************************************************************/
    void ButtonRightClicked();

    /****************************************************************************/
    /**
     * \brief This signal is emitted when left corner button is clicked
     */
    /****************************************************************************/
    void ButtonLeftClicked();

    /****************************************************************************/
    /**
     * \brief This signal is emitted when centre button is clicked
     */
    /****************************************************************************/
    void ButtonCenterClicked();

};

} // end namespace MainMenu

#endif // MAINMENU_MESSAGEDLG_H
