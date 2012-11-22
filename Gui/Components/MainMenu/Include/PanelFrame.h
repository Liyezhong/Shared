/****************************************************************************/
/*! \file PanelFrame.h
 *
 *  \brief PanelFrame definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
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

#ifndef MAINMENU_PANELFRAME_H
#define MAINMENU_PANELFRAME_H

#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CPanelFrame;
}

/****************************************************************************/
/**
 * \brief This is the basic background widget for all GUI panels.
 */
/****************************************************************************/
class CPanelFrame : public QWidget
{
    Q_OBJECT

public:
    explicit CPanelFrame(QWidget *p_Parent = 0);
    virtual ~CPanelFrame();

    void SetPanelTitle(QString Title);
    void SetTitleCenter();
    void SetContent(QLayout *p_Content);
    QWidget *GetContentFrame();

private:
    void paintEvent(QPaintEvent *);
    Ui::CPanelFrame *mp_FrameUi;    //!< User interface
    bool m_IsDialog;    //!< Indicates if the frame belongs to a dialog

protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_PANELFRAME_H
