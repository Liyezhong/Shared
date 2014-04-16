/****************************************************************************/
/*! \file WheelPanelWidget.h
 *
 *  \brief Header file for class CWheelPanel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-07-08
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

#ifndef SETTINGS_WHEELPANELWIDGET_H
#define SETTINGS_WHEELPANELWIDGET_H

#include "Settings/Include/ScrollWheelWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

namespace Settings {

class CSemiTransparentOverlay;
/****************************************************************************/
/**
 * \brief This class is the frame for one or multiple scroll wheels.
 */
/****************************************************************************/
class CWheelPanelWidget : public QWidget
{
    Q_OBJECT

public:
    //!< Separator between two scoll wheels
    typedef enum {
        NONE,
        COLON,
        FULLSTOP
    } SeparatorType_t;

    explicit CWheelPanelWidget(QWidget *p_Parent = 0);
    virtual ~CWheelPanelWidget();
    void Init(qint32 Count);
    void SetTitle(QString Title);
    void SetSubtitle(QString Subtitle, qint32 Position);
    void AddScrollWheel(Settings::CScrollWheelWidget *p_Wheel, qint32 Position);
    void AddSeparator(SeparatorType_t Type, qint32 Position);
    void SetDisabled(bool Disabled);
    void SetThreeDigitMode(bool Mode);
    void SetLayoutContentsMargin(int LeftMargin, int TopMargin, int RightMargin, int BottonMargin);
    void SetVerticalSpacing(int VerticalSpacing);

    /****************************************************************************/
    /**
     * \brief Gets the wheel panel size.
     *
     *  \return Size.
     */
    /****************************************************************************/
    QSize GetWheelPanelSize() { return size();}

private:
    void paintEvent(QPaintEvent *);
    QGridLayout *mp_Layout;                 //!< Main layout
    QLabel *mp_Title;                       //!< Title above the scroll wheels
    QList<QLabel *> m_SubtitleList;         //!< Titles below a single scroll
    QList<QLabel *> m_SeparatorList;        //!< Separators between wheel
    QPixmap  m_PanelPixmap;                 //!< Digit  panel Pixmap
    Settings::CSemiTransparentOverlay *mp_SemiTransparentOverlay; //!< Semitransparent widget to give disabled look.
};

} // end namespace Settings


#endif // SETTINGS_WHEELPANELWIDGET_H
