/****************************************************************************/
/*! \file TextDialog.h
 *
 *  \brief TextDialog definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-30
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

#ifndef MAINMENU_TEXTDIALOG_H
#define MAINMENU_TEXTDIALOG_H

#include "MainMenu/Include/DialogFrame.h"
#include <QTextEdit>

namespace MainMenu {

namespace Ui {
    class CTextDialog;
}

/****************************************************************************/
/**
 * \brief Content panel for dialogs displaying text files.
 */
/****************************************************************************/
class CTextDialog : public MainMenu::CDialogFrame
{
    Q_OBJECT

public:
    explicit CTextDialog(QWidget *p_Parent = 0);
    virtual ~CTextDialog();
    void SetCaption(QString Caption);
    void SetText(QString Text);

private:
    Ui::CTextDialog *mp_Ui; //!< User interface
    QTextEdit *mp_TextEdit; //!< Widget displaying a text file


protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_TEXTDIALOG_H
