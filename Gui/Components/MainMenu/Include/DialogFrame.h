/****************************************************************************/
/*! \file DialogFrame.h
 *
 *  \brief Header file for class CDialogFrame.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-08
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

#ifndef MAINMENU_DIALOGFRAME_H
#define MAINMENU_DIALOGFRAME_H

#include <QDialog>

class QMainWindow;

namespace MainMenu {

namespace Ui {
    class CDialogFrame;
}

/****************************************************************************/
/**
 * \brief This class provides a frame for all dialogs in the user interface.
 */
/****************************************************************************/
class CDialogFrame : public QDialog
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CDialogFrame(QWidget *p_Parent = 0, QWidget *pMainWindow = 0);
  //  explicit CDialogFrame(QWidget *p_Parent = 0, Qt::WindowFlags f = 0);
    virtual ~CDialogFrame();

    void SetDialogTitle(QString Title);
    void SetDialogTitle(QString TitleLeftCorner, QString TitleRightCorner);
    void SetContent(QLayout *p_Content);
    void SetMaxStringDialogTitle(qint32 StringLength, QString TitleText);
    void SetMainWindow(QWidget *mp_MainWindow);
    void CenterPosition();
    virtual void setVisible(bool visible);
protected:
    QWidget *GetContentFrame();
    void changeEvent(QEvent *p_Event);
    QWidget *mp_MainWindow;       //!< For position this dialog

private:
    Ui::CDialogFrame *mp_DialogUi;  //!< User interface
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CDialogFrame)
signals:
    void DialogLangaugeChanged();
};

} // end namespace MainMenu

#endif // MAINMENU_DIALOGFRAME_H
