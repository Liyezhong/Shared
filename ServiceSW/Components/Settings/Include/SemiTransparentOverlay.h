/****************************************************************************/
/*! \file SemiTransparentOverlay.h
 *
 *  \brief Header file for class CSemiTransparentOverlay.
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
#ifndef SETTINGS_SEMITRANSPARENTOVERLAY_H
#define SETTINGS_SEMITRANSPARENTOVERLAY_H

#include "MainMenu/Include/SliderControl.h"
#include <QWidget>

namespace Settings {

class CWheelPanelWidget;
class CSliderControl;

/****************************************************************************/
/**
 * \brief This class implements the semi transparent overlay to give the
 *        disabled look and feel of the scroll widget.
 */
/****************************************************************************/
class CSemiTransparentOverlay:public QWidget{
    Q_OBJECT

public:
    CSemiTransparentOverlay(Settings::CWheelPanelWidget *p_Parent = NULL);
    CSemiTransparentOverlay(MainMenu::CSliderControl *p_SliderParent = NULL);
    ~CSemiTransparentOverlay(){mp_Parent = 0;}

    /****************************************************************************/
    /**
     * \brief This methods sets the visibility of SemiTransparentOverlay.
     *
     *  \iparam Enabled = True if enable else False.
     */
    /****************************************************************************/
    void SetEnabled(bool Enabled){ m_Enabled = Enabled; }

private:
    void paintEvent(QPaintEvent *);
    CWheelPanelWidget *mp_Parent; //!< Parent Wheel panel widget
    MainMenu::CSliderControl *mp_SliderParent;  //!< Slider widget
    bool m_Enabled;         //!< True- widget is enabled
    bool m_SliderControl;    //!< True - Slider widget is enabled
};

}//End of namespace Settings

#endif // SETTINGS_SEMITRANSPARENTOVERLAY_H
