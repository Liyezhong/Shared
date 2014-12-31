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
#include <QTimer>

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

    explicit CMessageDlg(QWidget *p_Parent = NULL, quint64 ID = 0);
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
    void StartTimer(int msec);

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
        emit ButtonLeftClicked(m_ID);
	 reject();
    }

    /****************************************************************************/
    /**
     * \brief Slot called when message box timeout
     */
    /****************************************************************************/
    void OnTimeout()
    {
        emit MsgBoxTimeout(m_ID);
     reject();
    }

    /****************************************************************************/
    /**
     * \brief Slot called when center button is clicked
     */
    /****************************************************************************/
    void OnButtonCenterClicked()
    {
    emit ButtonCenterClicked(m_ID);
	accept();
    }

    /****************************************************************************/
    /**
     * \brief Slot called when right corner button is clicked
     */
    /****************************************************************************/
    void OnButtonRightClicked()
    {
    emit ButtonRightClicked(m_ID);
        accept();
    }

signals:

    /****************************************************************************/
    /**
     * \brief This signal is emitted when right corner button is clicked
     * \param quint64 message id
     */
    /****************************************************************************/
    void MsgBoxTimeout(quint64);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when right corner button is clicked
     * \param quint64 message id
     */
    /****************************************************************************/
    void ButtonRightClicked(quint64);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when left corner button is clicked
     * \param quint64 message id
     */
    /****************************************************************************/
    void ButtonLeftClicked(quint64);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when centre button is clicked
     * \param quint64 message id
     */
    /****************************************************************************/
    void ButtonCenterClicked(quint64);
private:
    quint64 m_ID; ///< mesage box id (event id)
    QTimer m_Timer; ///< timer for message box timeout

};

} // end namespace MainMenu

#endif // MAINMENU_MESSAGEDLG_H
