/****************************************************************************/
/*! \file SliderControl.h
 *
 *  \brief Header file for class CSliderControl.
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

//lint -e578

#ifndef MAINMENU_SLIDERCONTROL_H
#define MAINMENU_SLIDERCONTROL_H

#include <QMouseEvent>
#include <QSlider>

namespace MainMenu {
     class CSemiTransparentOverlay;

/****************************************************************************/
/**
 * \brief This is a touchscreen optimized slider.
 */
/****************************************************************************/
class CSliderControl : public QSlider
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    //! Left or right position of the slider
    typedef enum {
        PosLeft,
        PosRight
    } Position_t;

    explicit CSliderControl(QWidget *p_Parent = 0);
    virtual ~ CSliderControl();
    void SetPosition(Position_t Position);
    Position_t GetPosition();
    void SetDisabled(bool Disabled);

Q_SIGNALS:
    void positionChanged(MainMenu::CSliderControl::Position_t Position);

private:
    Position_t m_Position;  //!< Position of the slider
    CSemiTransparentOverlay *mp_SemiTransparentOverlay; //!< Semitransparent widget to give disabled look.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CSliderControl)

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnValueChanged(int Value);
};

} // end namespace MainMenu

#endif // MAINMENU_SLIDERCONTROL_H
