/****************************************************************************/
/*! \file SemiTransparentOverlay.cpp
 *
 *  \brief SemiTransparent Overlay implementation.
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

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CSemiTransparentOverlay::CSemiTransparentOverlay(CWheelPanel *p_Parent):QWidget(p_Parent) {
    mp_Parent = p_Parent;
    m_Enabled = false;
    m_SliderControl = false;
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_SliderParent = Parent widget
 */
/****************************************************************************/
CSemiTransparentOverlay::CSemiTransparentOverlay(CSliderControl *p_SliderParent):QWidget(p_SliderParent) {
    mp_SliderParent = p_SliderParent;
    m_Enabled = false;
    m_SliderControl = true ;
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
        QPixmap Source1(QSize(149,37));
        QPixmap Source(QString(":/%1/Digits/Digit_Overlay_disabled.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
        if(m_SliderControl){
            QPixmap Target(mp_SliderParent->size());
          //  Source(QSize(149,37)); //mp_SliderParent->size();
           // Source.width(149);
            Source.scaled(QSize(149,37),Qt::IgnoreAspectRatio,Qt::FastTransformation);
            Target.fill(Qt::transparent);
         //   Application::CLeicaStyle::BorderPixmap(&Target, &Source, 17, 0, 18, 0);
            Application::CLeicaStyle::BorderPixmap(&Target, &Source, 0, 0, 0, 0);
            Painter.setOpacity(0.5);
            Painter.drawPixmap(0, 0, Target);
            qDebug()<<"Slider Size"<< mp_SliderParent->size();
            qDebug()<<"Overlay Image Size"<< Source.size();

        }
        else{
            QPixmap Target(mp_Parent->size());
            Target.fill(Qt::transparent);
            Application::CLeicaStyle::BorderPixmap(&Target, &Source, 17, 0, 18, 0);
            Painter.setOpacity(0.5);
            Painter.drawPixmap(0, 0, Target);
        }
    }
}

} // end namespace MainMenu
