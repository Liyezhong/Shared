/****************************************************************************/
/*! \file SliderControl.cpp
 *
 *  \brief Implementation of file for class CSliderControl.
 *
 *  \b Description:
 *          This class implements a base widget for displaying/Controlling the
 *          slider widget.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-02
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

#include "MainMenu/Include/SliderControl.h"
#include "MainMenu/Include/SemiTransparentOverlay.h"
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
CSliderControl::CSliderControl(QWidget *p_Parent) : QSlider(p_Parent)
{
    m_Position = PosLeft;
    setPageStep(maximum() - minimum());
    setMinimumWidth(149);
    setTracking(false);
    mp_SemiTransparentOverlay = new CSemiTransparentOverlay(this);
    mp_SemiTransparentOverlay->setFixedSize(width()+SIZE_OFFSET,height()+SIZE_OFFSET);
    (void) connect(this, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSliderControl::~CSliderControl()
{
    try {
        if (mp_SemiTransparentOverlay) {
            delete mp_SemiTransparentOverlay;
        }
    }
    catch (...) {
        // to please Lint.
    }
}
/****************************************************************************/
/*!
 *  \brief Sets the slider position when the slider value has changed
 *
 *  \iparam Value = Slider value
 */
/****************************************************************************/
void CSliderControl::OnValueChanged(int Value)
{
    if (Value > maximum() / 2) {
        setSliderPosition(maximum());
        m_Position = PosRight;
    }
    else {
        setSliderPosition(minimum());
        m_Position = PosLeft;
    }

    emit positionChanged(m_Position);
}

/****************************************************************************/
/*!
 *  \brief  Disable/enable the wheel panel
 *
 *  \iparam Disabled - true for disable, false for enable.
 */
/****************************************************************************/
void CSliderControl::SetDisabled(bool Disabled)
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
 *  \brief Sets the position of the slider
 *
 *  \iparam Position = Slider position
 */
/****************************************************************************/
void CSliderControl::SetPosition(Position_t Position)
{
    if(Position == PosLeft) {
        setValue(minimum());
    }
    else {
        setValue(maximum());
    }
}

/****************************************************************************/
/*!
 *  \brief Gets the position of the slider
 *
 *  \return Slider position
 */
/****************************************************************************/
CSliderControl::Position_t CSliderControl::GetPosition()
{
    return m_Position;
}

} // end namespace MainMenu
