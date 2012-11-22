/****************************************************************************/
/*! \file DialogFrame.h
 *
 *  \brief DialogFrame definition.
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

public:
    explicit CDialogFrame(QWidget *p_Parent = 0);
    virtual ~CDialogFrame();

    void SetDialogTitle(QString Title);
    void SetContent(QLayout *p_Content);

protected:
    QWidget *GetContentFrame();
    void changeEvent(QEvent *p_Event);

private:
    Ui::CDialogFrame *mp_DialogUi;  //!< User interface
};

} // end namespace MainMenu

#endif // MAINMENU_DIALOGFRAME_H
