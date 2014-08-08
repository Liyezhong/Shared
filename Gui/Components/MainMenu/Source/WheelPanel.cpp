/****************************************************************************/
/*! \file WheelPanel.cpp
 *
 *  \brief Implementation of file for class CWheelPanel.
 *
 *  \b Description:
 *          This class implements a base widget,which is used for
 *          displaying Panel frames for scroll wheel widgets.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-10
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

//lint -e1512

#include "Application/Include/LeicaStyle.h"
#include "MainMenu/Include/WheelPanel.h"
#include "MainMenu/Include/SemiTransparentOverlay.h"
#include <QPainter>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CWheelPanel::CWheelPanel(QWidget *p_Parent) : QWidget(p_Parent)
{
    QPalette Palette = palette();
    Palette.setColor(QPalette::WindowText, Qt::white);
    setPalette(Palette);

    mp_SemiTransparentOverlay = new CSemiTransparentOverlay(this);
    m_PanelPixmap = QPixmap(QString(":/%1/Digits/Digit_Panel.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));

    mp_Layout = new QGridLayout(this);
    mp_Title = new QLabel();
    mp_Title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // Check if device is Sepia or Colorado and set the layout margin accordingly
    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        // Set margin content for Sepia
        mp_Layout->setContentsMargins(8, 10, 6, 10);
        mp_Title->setMaximumSize(mp_Title->width(), 20);
    }
    else {
        // Set margin content for Colorado
        mp_Layout->setContentsMargins(9, 3, 10, 8);
        mp_Title->setMinimumSize(0, 40);
    }
    mp_Layout->setSpacing(0);
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CWheelPanel::~CWheelPanel()
{
    try {
        while (!m_SeparatorList.isEmpty()) {
            delete m_SeparatorList.takeFirst();
        }

        while (!m_SubtitleList.isEmpty()) {
            delete m_SubtitleList.takeFirst();
        }

        delete mp_Title;
        delete mp_Layout;
        if (mp_SemiTransparentOverlay) {
            delete mp_SemiTransparentOverlay;
        }
    }
    catch (...) {
        // to please lint
    }
}

/****************************************************************************/
/*!
 *  \brief Initializes the wheel panel
 *
 *  \iparam Count = Number of wheels in the panel
 */
/****************************************************************************/
void CWheelPanel::Init(qint32 Count)
{
    // Check for device type
    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        // Set minimum size of the widget for Sepia
       setMinimumSize(19 + 61 * Count + 4 * (Count - 1), 202);
    }
    else {
        // Set minimum size of the widget for Colorado
       setMinimumSize(19 + 86 * Count + 4 * (Count - 1), 277);
    }

    mp_Layout->addWidget(mp_Title, 0, 0, 1, 2 * Count - 1);

    for (qint32 i = 0; i < Count; i++) {
        QLabel *p_Label = new QLabel();
        p_Label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
            p_Label->setMinimumSize(56, 20);
        }
        else {
            p_Label->setMinimumSize(86, 38);
        }
        mp_Layout->addWidget(p_Label, 2, 2 * i);
        m_SubtitleList << p_Label;
    }

    for (qint32 i = 0; i < Count - 1; i++) {
        QLabel *p_Label = new QLabel();
        p_Label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        p_Label->setMinimumSize(4, 188);
        mp_Layout->addWidget(p_Label, 1, 2 * i + 1);
        m_SeparatorList << p_Label;
    }
    mp_SemiTransparentOverlay->setFixedSize(width(),height());
}

/****************************************************************************/
/*!
 *  \brief  Disable/enable the wheel panel
 *
 *  \iparam Disabled - true for disable, false for enable.
 */
/****************************************************************************/
void CWheelPanel::SetDisabled(bool Disabled)
{
    if (Disabled) {
        mp_SemiTransparentOverlay->raise();
        mp_SemiTransparentOverlay->SetEnabled(true);
    }
    else {
        mp_SemiTransparentOverlay->lower();
        mp_SemiTransparentOverlay->SetEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief  Set the pixmap for 3 digit/2 digit mode of scroll wheel
 *
 *  \iparam Mode = true -3 digit mode, false - 2 digit mode.
 */
/****************************************************************************/
void CWheelPanel::SetThreeDigitMode(bool Mode)
{
    if (Mode) {
        setMinimumSize(137, 277);
        m_PanelPixmap = QPixmap(QString(":/%1/Digits/Digit_Panel_3_digits.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
        mp_Layout->setContentsMargins(5, 3, 5, 8);
    }
    else {
        if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
             setMinimumSize(80, 202);
        }
        else {
             setMinimumSize(105, 277);
        }
        m_PanelPixmap = QPixmap(QString(":/%1/Digits/Digit_Panel.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
        mp_Layout->setContentsMargins(9, 3, 10, 8);
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the title of the panel
 *
 *  \iparam Title = Title string
 */
/****************************************************************************/
void CWheelPanel::SetTitle(QString Title)
{
    mp_Title->setText(Title);
}

/****************************************************************************/
/*!
 *  \brief Sets the caption below a scroll wheel
 *
 *  \iparam Subtitle = Title string
 *  \iparam Position = Position of the scroll wheel
 */
/****************************************************************************/
void CWheelPanel::SetSubtitle(QString Subtitle, qint32 Position)
{
    m_SubtitleList[Position]->setText(Subtitle);
}

/****************************************************************************/
/*!
 *  \brief Adds a scroll wheel to the panel
 *
 *  \iparam p_Wheel = Pointer to the scroll wheel
 *  \iparam Position = Position of the scroll wheel
 */
/****************************************************************************/
void CWheelPanel::AddScrollWheel(MainMenu::CScrollWheel *p_Wheel, qint32 Position)
{
    mp_Layout->addWidget(p_Wheel, 1, 2 * Position);
}

/****************************************************************************/
/*!
 *  \brief Adds a separator between two scoll wheels
 *
 *  \iparam Type = Colon or fullstop
 *  \iparam Position = Position of the scroll wheel
 */
/****************************************************************************/
void CWheelPanel::AddSeparator(SeparatorType_t Type, qint32 Position)
{
    switch (Type) {
    case COLON:
        if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
            m_SeparatorList[Position]->setPixmap(QPixmap(":/Small/Digits/Digit_Colon.png").copy(30, 0, 2, 30));
        }
        else {
            m_SeparatorList[Position]->setPixmap(QPixmap(":/Large/Digits/Digit_Colon.png").copy(7, 43, 4, 188));
        }
        break;
    case FULLSTOP:
        if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
            m_SeparatorList[Position]->setPixmap(QPixmap(":/Small/Digits/Digit_FullStopp.png").copy(30, 0, 2, 30));
        }
        else {
            m_SeparatorList[Position]->setPixmap(QPixmap(":/Large/Digits/Digit_FullStopp.png").copy(7, 43, 4, 188));
        }
        break;
    default:
        break;
    }
}

/****************************************************************************/
/*!
 *  \brief Paints the background image of the scroll wheels
 */
/****************************************************************************/
void CWheelPanel::paintEvent(QPaintEvent *)
{
    QPainter Painter(this);
    QPixmap Source(QString(":/%1/Digits/Digit_Panel.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
    QPixmap Target(size());
    Target.fill(Qt::transparent);

    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        // Set border pixmap size for Sepia scroll wheel
        Application::CLeicaStyle::BorderPixmap(&Target, &Source, 30, 40, 45, 40);
    }
    else {
        // Set border pixmap size for Colorado scroll wheel
        Application::CLeicaStyle::BorderPixmap(&Target, &Source, 17, 0, 18, 0);
    }
    Painter.drawPixmap(0, 0, Target);
}

/****************************************************************************/
/*!
 *  \brief Sets the wheel panel's content margins.
 *  \iparam LeftMargin = Left margin value
 *  \iparam TopMargin = Top margin value
 *  \iparam RightMargin = Right margin value
 *  \iparam BottomMargin = Bottom margin value
 */
/****************************************************************************/
void CWheelPanel::SetLayoutContentsMargin(int LeftMargin, int TopMargin, int RightMargin, int BottonMargin)
{
   mp_Layout->setContentsMargins(LeftMargin, TopMargin, RightMargin, BottonMargin);
}

/****************************************************************************/
/*!
 *  \brief Sets the vertical spacing for the wheel panel.
 *  \iparam VerticalSpacing = VerticalSpacing value
 */
/****************************************************************************/
void CWheelPanel::SetVerticalSpacing(int VerticalSpacing)
{
   mp_Layout->setVerticalSpacing(VerticalSpacing);
}


/****************************************************************************/
/*!
 *  \brief Sets title text as per the maximum title string length.
 *
 *  \iparam StringLength = Maximum string lenght of dialog title.
 *  \iparam TitleText = String to be set
 *
 */
/****************************************************************************/
void CWheelPanel::SetTitleWithMaxLength(qint32 StringLength, QString TitleText)
{
    if (TitleText.length() > StringLength) {
        (void) TitleText.remove(StringLength, TitleText.length() - StringLength);
    }
    mp_Title->setText(TitleText);
}

} // end namespace MainMenu
