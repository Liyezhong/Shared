/****************************************************************************/
/*! \file PanelFrame.h
 *
 *  \brief Header file for class CPanelFrame.
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
    friend class  CTestMainMenu;

public:
    explicit CPanelFrame(QWidget *p_Parent = 0);
    virtual ~CPanelFrame();

    void SetPanelTitle(QString Title);
    void SetPanelTitle(QString TitleLeftCorner, QString TitleRigtCorner);
    void SetTitleCenter();
    void SetContent(QLayout *p_Content);
    QWidget *GetContentFrame();
    void SetContentsMargins(int Left, int Top, int Right, int Bottom);
    void SetPanelTitleMinimumHeight(int MinimumHeight);
    void SetTitleVerticalCenter();
    void SetMaxStringPanelTitle(qint32 StringLength, QString TitleText);
    void SetPanelTitlePixmap(QPixmap TitlePixmap);

private:
    void paintEvent(QPaintEvent *);
    Ui::CPanelFrame *mp_FrameUi;    //!< User interface
    bool m_IsDialog;    //!< Indicates if the frame belongs to a dialog
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CPanelFrame)

protected:
    void changeEvent(QEvent *p_Event);
signals:
    void LanguageChanged();
};

} // end namespace MainMenu

#endif // MAINMENU_PANELFRAME_H
