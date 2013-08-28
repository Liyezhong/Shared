/****************************************************************************/
/*! \file WheelPanel.cpp
 *
 *  \brief WheelPanel implementation.
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
    m_PanelPixmap = QPixmap(QString(":/%1/Digits/Digit_Panel.png").arg(Application::CLeicaStyle::GetProjectNameString()));

    mp_Layout = new QGridLayout(this);
    mp_Layout->setContentsMargins(9, 3, 10, 8);
    mp_Layout->setSpacing(0);

    mp_Title = new QLabel();
    mp_Title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mp_Title->setMinimumSize(0, 40);
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CWheelPanel::~CWheelPanel()
{
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

/****************************************************************************/
/*!
 *  \brief Initializes the wheel panel
 *
 *  \iparam Count = Number of wheels in the panel
 */
/****************************************************************************/
void CWheelPanel::Init(qint32 Count)
{
    setMinimumSize(19 + 86 * Count + 4 * (Count - 1), 277);
    mp_Layout->addWidget(mp_Title, 0, 0, 1, 2 * Count - 1);

    for (qint32 i = 0; i < Count; i++) {
        QLabel *p_Label = new QLabel();
        p_Label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        p_Label->setMinimumSize(86, 38);
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
    if (Disabled){
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
	setMinimumSize(137,277);
        m_PanelPixmap = QPixmap(QString(":/%1/Digits/Digit_Panel_3_digits.png").arg(Application::CLeicaStyle::GetProjectNameString()));
        mp_Layout->setContentsMargins(5, 3, 5, 8);
    }
    else {
	setMinimumSize(105,277);
        m_PanelPixmap = QPixmap(QString(":/%1/Digits/Digit_Panel.png").arg(Application::CLeicaStyle::GetProjectNameString()));
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
    mp_Title->setText(tr("%1").arg(Title));
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
    m_SubtitleList[Position]->setText(tr("%1").arg(Subtitle));
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
            m_SeparatorList[Position]->setPixmap(QPixmap(QString(":/%1/Digits/Digit_Colon.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(7, 43, 4, 188));
            break;
        case FULLSTOP:
            m_SeparatorList[Position]->setPixmap(QPixmap(QString(":/%1/Digits/Digit_FullStopp.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(7, 43, 4, 188));
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
    QPixmap Source(QString(":/%1/Digits/Digit_Panel.png").arg(Application::CLeicaStyle::GetProjectNameString()));
    QPixmap Target(size());
    Target.fill(Qt::transparent);
    Application::CLeicaStyle::BorderPixmap(&Target, &Source, 17, 0, 18, 0);
    Painter.drawPixmap(0, 0, Target);
}

} // end namespace MainMenu
