/****************************************************************************/
/*! \file SemiTransparentOverlay.cpp
 *
 *  \brief Implementation of file for class CSemiTransparentOverlay.
 *
 *  \b Description:
 *          This class implements a base widget, which will give disable look and feel
 *          for the scroll wheel widgets.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-01-17
 *   $Author:  $ N.Kamath
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
#include "MainMenu/Include/SemiTransparentOverlay.h"
#include "MainMenu/Include/WheelPanel.h"
#include "MainMenu/Include/SliderControl.h"
#include "Application/Include/LeicaStyle.h"
#include <QPainter>
#include <QDebug>

namespace MainMenu {

const int  SIZE_OFFSET = 10;                //!< Offset added to the width and height


/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CSemiTransparentOverlay::CSemiTransparentOverlay(CWheelPanel *p_Parent): QWidget(p_Parent),
    mp_Parent(p_Parent),
    mp_SliderParent(NULL),
    m_Enabled(false),
    m_SliderControl(false)
{
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_SliderParent = Parent widget
 */
/****************************************************************************/
CSemiTransparentOverlay::CSemiTransparentOverlay(CSliderControl *p_SliderParent): QWidget(p_SliderParent),
                                mp_Parent(NULL), mp_SliderParent(p_SliderParent), m_Enabled(false), m_SliderControl(true)
{
}

/****************************************************************************/
/*!
 *  \brief Paints the semitransparent overlay to give disabled look.
 */
/****************************************************************************/
void CSemiTransparentOverlay::paintEvent(QPaintEvent *)
{
    if (m_Enabled) {
        QPainter Painter(this);
        QPixmap Source(QString(":/%1/Digits/Digit_Overlay_disabled.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
        if (m_SliderControl) {
            if (mp_SliderParent) {
                QPixmap Target(mp_SliderParent->size());
                Source = Source.scaled(mp_SliderParent->size(),Qt::IgnoreAspectRatio,Qt::FastTransformation);
                Source = Source.scaledToWidth(Source.width() + SIZE_OFFSET);
                Source = Source.scaledToHeight(Source.height() + SIZE_OFFSET);
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, &Source, -4, 0, 0, 0);
                Painter.setOpacity(0.5);
                Painter.drawPixmap(0, 0, Target);
            }
        }
        else {
            if (mp_Parent) {
                QPixmap Target(mp_Parent->size());
                Target.fill(Qt::transparent);
                Application::CLeicaStyle::BorderPixmap(&Target, &Source, 17, 0, 18, 0);
                Painter.setOpacity(0.5);
                Painter.drawPixmap(0, 0, Target);
            }
        }
    }
}

} // end namespace MainMenu
