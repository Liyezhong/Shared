/****************************************************************************/
/*! \file LeicaStyle.cpp
 *
 *  \brief LeicaStyle implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-07-20
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
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

namespace Application {

bool CLeicaStyle::m_StyleSize = false;

/****************************************************************************/
/*!
 *  \brief Reimplemented function setting the color palette
 *
 *  \iparam Palette = Palette to be modified
 */
/****************************************************************************/
void CLeicaStyle::polish(QPalette &Palette)
{
    Palette.setColor(QPalette::Window, QColor(233, 234, 235));
    Palette.setColor(QPalette::ButtonText, Qt::white);
    Palette.setColor(QPalette::Button, QColor(226, 227, 228));
    Palette.setColor(QPalette::Highlight, QColor(218, 32, 29));
}

/****************************************************************************/
/*!
 *  \brief Reimplemented function returning the pixel metrics
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam p_Widget = Requesting widget
 *
 *  \return Metric in pixels
 */
/****************************************************************************/
int CLeicaStyle::pixelMetric(PixelMetric Which, const QStyleOption *p_Option, const QWidget *p_Widget) const
{
    switch (Which) {
        case PM_TabBarTabHSpace:
            return 10;
        case PM_TabBarBaseOverlap:
            return 0;
        case PM_IndicatorHeight:
        case PM_SliderLength:
            return 34;
        case PM_ExclusiveIndicatorWidth:
        case PM_ExclusiveIndicatorHeight:
        case PM_IndicatorWidth:
            return 37;
        default:
            return QWindowsStyle::pixelMetric(Which, p_Option, p_Widget);
    }
}

/****************************************************************************/
/*!
 *  \brief Reimplemented function drawing simple subcomponents
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam p_Painter = Painter of the widget
 *  \iparam p_Widget = Requesting widget
 */
/****************************************************************************/
void CLeicaStyle::drawPrimitive(PrimitiveElement Which, const QStyleOption *p_Option, QPainter *p_Painter, const QWidget *p_Widget) const
{
    switch (Which) {
        case PE_FrameTabWidget:
        {
            const QStyleOptionTabWidgetFrame *p_TabWidgetFrame = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(p_Option);
            QRect Cornered(p_Option->rect);
            QRect Rounded(p_Option->rect);
            if (p_TabWidgetFrame->shape == QTabBar::RoundedNorth) {
                Cornered.setHeight(p_Option->rect.height() - 20);
                Rounded.setTop(p_Option->rect.bottom() - 40);
            }
            else {
                Cornered.setWidth(p_Option->rect.width() - 20);
                Rounded.setLeft(p_Option->rect.right() - 40);
            }
            p_Painter->setPen(QColor(183, 185, 188));
            p_Painter->setBrush(QColor(183, 185, 188));
            p_Painter->drawRect(Cornered);
            p_Painter->setRenderHint(QPainter::Antialiasing);
            p_Painter->drawRoundedRect(Rounded, 16.0, 16.0);
            break;
        }
        case PE_FrameGroupBox:
        {
            QRect Rect = p_Option->rect;
            QPen Pen = p_Painter->pen();
            p_Painter->setRenderHint(QPainter::Antialiasing);
            Rect.setLeft(Rect.left() + 1);
            Rect.setWidth(Rect.width() - 1);
            Rect.setHeight(Rect.height() - 1);
            Pen.setWidth(2);
            p_Painter->setPen(Pen);
            p_Painter->drawRoundedRect(Rect, 1, 1);
            break;
        }
        case PE_IndicatorRadioButton:
        {
            if (p_Option->state & QStyle::State_Enabled) {
                if (p_Option->state & QStyle::State_On) {
                    if (p_Option->state & QStyle::State_Sunken) {
                        p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/RadioButton/RadioButton-CheckedPressed.png").arg(GetStyleSizeString())));
                    }
                    else {
                        p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/RadioButton/RadioButton-Checked.png").arg(GetStyleSizeString())));
                    }
                }
                else {
                    if (p_Option->state & QStyle::State_Sunken) {
                        p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/RadioButton/RadioButton-Pressed.png").arg(GetStyleSizeString())));
                    }
                    else {
                        p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/RadioButton/RadioButton-Enabled.png").arg(GetStyleSizeString())));
                    }
                }
            }
            else {
                if (p_Option->state & QStyle::State_On) {
                    if (!(p_Option->state & QStyle::State_Sunken)) {
                       p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/RadioButton/RadioButton-CheckedDisabled.png").arg(GetStyleSizeString())));
                    }
                }
                else {
                    if (!(p_Option->state & QStyle::State_Sunken)) {
                        p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/RadioButton/RadioButton-Disabled.png").arg(GetStyleSizeString())));
                    }
                }
            }
            break;
        }
        case PE_IndicatorCheckBox:
        case PE_IndicatorItemViewItemCheck:
        {
            if (p_Option->state & QStyle::State_Enabled) {
                if (p_Option->state & QStyle::State_On) {
                    if (p_Option->state & QStyle::State_Sunken) {
                        p_Painter->drawPixmap(p_Option->rect.topLeft(), QPixmap(QString(":/%1/CheckBox/CheckBox-CheckedPressed.png").arg(GetStyleSizeString())));
                    }
                    else {
                        p_Painter->drawPixmap(p_Option->rect.topLeft(), QPixmap(QString(":/%1/CheckBox/CheckBox-Checked.png").arg(GetStyleSizeString())));
                    }
                }
                else {
                    if (p_Option->state & QStyle::State_Sunken) {
                        p_Painter->drawPixmap(p_Option->rect.topLeft(), QPixmap(QString(":/%1/CheckBox/CheckBox-Pressed.png").arg(GetStyleSizeString())));
                    }
                    else {
                        p_Painter->drawPixmap(p_Option->rect.topLeft(), QPixmap(QString(":/%1/CheckBox/CheckBox-Enabled.png").arg(GetStyleSizeString())));
                    }
                }
            }
            else {
                if (p_Option->state & QStyle::State_On) {
                    if (!(p_Option->state & QStyle::State_Sunken)) {
                        p_Painter->drawPixmap(p_Option->rect.topLeft(), QPixmap(QString(":/%1/CheckBox/CheckBox-CheckedDisabled.png").arg(GetStyleSizeString())));
                    }
                }
                else {
                    if (!(p_Option->state & QStyle::State_Sunken)) {
                        p_Painter->drawPixmap(p_Option->rect.topLeft(), QPixmap(QString(":/%1/CheckBox/CheckBox-Disabled.png").arg(GetStyleSizeString())));
                    }
                }
            }
            break;
        }
        case PE_PanelLineEdit:
        {
            p_Painter->setPen(QColor(Qt::black));
            //white frame of the lineEdit
            if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(p_Option)) {

                p_Painter->fillRect(panel->rect.adjusted(panel->lineWidth, panel->lineWidth, -panel->lineWidth, -panel->lineWidth),panel->palette.brush(QPalette::Base));
            }
            //draw black rect frame
            p_Painter->drawRect(0,0,p_Widget->width() - 1 , p_Widget->height() - 1);

            break;
        }
        default:
            QWindowsStyle::drawPrimitive(Which, p_Option, p_Painter, p_Widget);
    }
}


/****************************************************************************/
/*!
 *  \brief Reimplemented function drawing whole widgets
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam p_Painter = Painter of the widget
 *  \iparam p_Widget = Requesting widget
 */
/****************************************************************************/
void CLeicaStyle::drawControl(ControlElement Which, const QStyleOption *p_Option, QPainter *p_Painter, const QWidget *p_Widget) const
{
    switch (Which) {
        case CE_PushButtonBevel:
        {
            const QStyleOptionButton *p_Button = qstyleoption_cast<const QStyleOptionButton *>(p_Option);

            if (!(p_Button->features & QStyleOptionButton::Flat)) {
                QPixmap Source;
                if(p_Widget->minimumHeight() >= 62) {
                    if (p_Option->state & QStyle::State_Enabled) {
                        if (p_Option->state & QStyle::State_On) {
                            if (p_Option->state & QStyle::State_Sunken) {
                                Source = QPixmap(QString(":/%1/IconPushButton/IconPushButton-SelectedPressed.png").arg(GetStyleSizeString()));
                            }
                            else {
                                Source = QPixmap(QString(":/%1/IconPushButton/IconPushButton-Selected.png").arg(GetStyleSizeString()));
                            }
                        }
                        else {
                            if (p_Option->state & QStyle::State_Sunken) {
                                Source = QPixmap(QString(":/%1/IconPushButton/IconPushButton-Pressed.png").arg(GetStyleSizeString()));
                            }
                            else {
                                Source = QPixmap(QString(":/%1/IconPushButton/IconPushButton-Enabled.png").arg(GetStyleSizeString()));
                            }
                        }
                    }
                    else {
                        Source = QPixmap(QString(":/%1/IconPushButton/IconPushButton-Disabled.png").arg(GetStyleSizeString()));
                    }
                    p_Painter->drawPixmap(0, 0, Source);
                }
                else {
                    QPixmap Target(p_Option->rect.size());
                    Target.fill(Qt::transparent);

                    if (p_Option->state & QStyle::State_Enabled) {
                        if (p_Option->state & QStyle::State_On) {
                            if (p_Option->state & QStyle::State_Sunken) {
                                Source = QPixmap(QString(":/%1/TextButton/TextButton-SelectedPressed.png").arg(GetStyleSizeString()));
                            }
                            else {
                                Source = QPixmap(QString(":/%1/TextButton/TextButton-Selected.png").arg(GetStyleSizeString()));
                            }
                        }
                        else {
                            Source = QPixmap(PushButtonPath(p_Widget->palette().color(QPalette::Button), p_Option->state));
                        }
                    }
                    else {
                        Source = QPixmap(QString(":/%1/TextButton/TextButton-Disabled.png").arg(GetStyleSizeString()));
                    }
                    BorderPixmap(&Target, &Source, 18, 0, 18, 0);
                    p_Painter->drawPixmap(0, 0, Target);
                }
            }
            break;
        }
        case CE_TabBarTabLabel:
        {
            const QStyleOptionTab *p_Tab = qstyleoption_cast<const QStyleOptionTab *>(p_Option);

            if (p_Tab->shape == QTabBar::RoundedNorth) {
                QWindowsStyle::drawControl(Which, p_Option, p_Painter, p_Widget);
            }
            else {
                QTransform Transform = QTransform::fromTranslate(0, -12);
                p_Painter->setTransform(Transform);
                QWindowsStyle::drawControl(Which, p_Option, p_Painter, p_Widget);
                Transform = QTransform::fromTranslate(0, 0);
                p_Painter->setTransform(Transform);
            }
            break;
        }
        case CE_TabBarTabShape:
        {
            QPixmap Source;
            QPixmap Target;
            const QStyleOptionTab *pTab = qstyleoption_cast<const QStyleOptionTab *>(p_Option);

            if (p_Option->state & QStyle::State_Selected) {
                Source = QPixmap(QString(":/%1/Tab_Control/Tab_Control-Btn-Down.png").arg(GetStyleSizeString()));
            }
            else {
                if ((pTab->position == QStyleOptionTab::Beginning && pTab->shape == QTabBar::RoundedNorth)
                        || (pTab->position == QStyleOptionTab::End && pTab->shape == QTabBar::RoundedWest)) {
                    if(m_StyleSize==false){
                        Source = QPixmap(QString(":/%1/Tab_Control/Tab_Control-Btn6-Up.png").arg(GetStyleSizeString()));
                    }
                    else{
                        Source = QPixmap(QString(":/%1/Tab_Control/Tab_Control-BtnLast-Up.png").arg(GetStyleSizeString()));
                    }
                }
                else if ((pTab->position == QStyleOptionTab::End && pTab->shape == QTabBar::RoundedNorth)
                        || (pTab->position == QStyleOptionTab::Beginning && pTab->shape == QTabBar::RoundedWest)) {

                    if(m_StyleSize==false){
                        Source = QPixmap(QString(":/%1/Tab_Control/Tab_Control-Btn1-Up.png").arg(GetStyleSizeString()));
                    }
                    else{
                        Source = QPixmap(QString(":/%1/Tab_Control/Tab_Control-BtnFirst-Up.png").arg(GetStyleSizeString()));
                    }
                }
                else {
                    Source = QPixmap(QString(":/%1/Tab_Control/Tab_Control-Btn-Up.png").arg(GetStyleSizeString()));
                }
            }

            if (pTab->shape == QTabBar::RoundedNorth) {
                Target = QPixmap(p_Option->rect.width() - 4, p_Option->rect.height());
            }
            else {
                Target = QPixmap(p_Option->rect.width(), p_Option->rect.height() - 4);
            }
            Target.fill(Qt::transparent);

            if (pTab->shape == QTabBar::RoundedNorth) {
                QTransform Transform;
                Transform.rotate(90.0);
                Source = Source.transformed(Transform);
                BorderPixmap(&Target, &Source, 18, 24, 18, 5);
            }
            else {
                BorderPixmap(&Target, &Source, 24, 18, 5, 18);
            }

            p_Painter->drawPixmap(p_Option->rect.topLeft(), Target);
            break;
        }
        // For now button fonts are not fat
        /*case CE_PushButtonLabel:
        {
            QFont Font = p_Painter->font();
            Font.setBold(true);
            p_Painter->setFont(Font);
            QWindowsStyle::drawControl(Which, p_Option, p_Painter, p_Widget);
            break;
        }*/
        case CE_HeaderSection:
        {
            p_Painter->setPen(QColor(177, 175, 181));
            p_Painter->drawLine(p_Option->rect.topRight(), p_Option->rect.bottomRight());
            p_Painter->drawLine(p_Option->rect.bottomLeft(), p_Option->rect.bottomRight());
            break;
        }
        default:
            QWindowsStyle::drawControl(Which, p_Option, p_Painter, p_Widget);
    }
}

/****************************************************************************/
/*!
 *  \brief Reimplemented function drawing a complex control element
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam p_Painter = Painter of the widget
 *  \iparam p_Widget = Requesting widget
 */
/****************************************************************************/
void CLeicaStyle::drawComplexControl(ComplexControl Which, const QStyleOptionComplex *p_Option, QPainter *p_Painter, const QWidget *p_Widget) const
{
    switch (Which) {
        case CC_Slider:
        {
            const QStyleOptionSlider *p_Slider = qstyleoption_cast<const QStyleOptionSlider *>(p_Option);
            QRect Handle = proxy()->subControlRect(CC_Slider, p_Slider, SC_SliderHandle, p_Widget);

            p_Painter->drawPixmap(0, 0, QPixmap(QString(":/%1/SlideSwitch/SlideSwitch-BG.png").arg(GetStyleSizeString())));

            if (p_Option->state & QStyle::State_Sunken) {
                p_Painter->drawPixmap(Handle.x(), 0, QPixmap(QString(":/%1/SlideSwitch/SliderButton-Pressed.png").arg(GetStyleSizeString())));
            }
            else {
                p_Painter->drawPixmap(Handle.x(), 0, QPixmap(QString(":/%1/SlideSwitch/SliderButton-Enabled.png").arg(GetStyleSizeString())));
            }
            break;
        }
        default:
            QWindowsStyle::drawComplexControl(Which, p_Option, p_Painter, p_Widget);
    }
}

/****************************************************************************/
/*!
 *  \brief Reimplemented function returning the optimal size
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam ContentsSize = Size of the content
 *  \iparam p_Widget = Requesting widget
 *
 *  \return Size of the widget
 */
/****************************************************************************/
QSize CLeicaStyle::sizeFromContents(ContentsType Which, const QStyleOption *p_Option, const QSize &ContentsSize, const QWidget *p_Widget) const
{
    switch (Which) {
        case CT_HeaderSection:
        {
            QSize Size = QWindowsStyle::sizeFromContents(Which, p_Option, ContentsSize, p_Widget);
            Size.setHeight(28);
            return Size;
        }
        case CT_PushButton:
        {
            const QStyleOptionButton *p_Button = qstyleoption_cast<const QStyleOptionButton *>(p_Option);
            if (!(p_Button->features & QStyleOptionButton::Flat)) {
                if(p_Widget->minimumHeight() >= 62) {
                    return QSize(71, 62);
                }
                else {
                    QSize Size = QWindowsStyle::sizeFromContents(Which, p_Option, ContentsSize, p_Widget);
                    Size.setHeight(38);
                    Size.setWidth(Size.width() + 45);
                    return Size;
                }
            }
            else {
                return QWindowsStyle::sizeFromContents(Which, p_Option, ContentsSize, p_Widget);
            }
        }
        case CT_TabBarTab:
        {
            const QStyleOptionTab *p_Tab = qstyleoption_cast<const QStyleOptionTab *>(p_Option);
            if (p_Tab->shape == QTabBar::RoundedNorth) {
                return QSize(ContentsSize.width() + 18, 48);
            }
            else {
                if(m_StyleSize==false){
                    return QSize(79, 92);
                }
                else{
                    return QSize(92, 148);
                }
            }
        }
        case CT_Slider:
            return QSize(149, 37);
        default:
            return QWindowsStyle::sizeFromContents(Which, p_Option, ContentsSize, p_Widget);
    }
}

/****************************************************************************/
/*!
 *  \brief Reimplemented function returning style hints
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam p_Widget = Requesting widget
 *  \oparam p_ReturnData = More detailed return data
 *
 *  \return Style hint
 */
/****************************************************************************/
int CLeicaStyle::styleHint(StyleHint Which, const QStyleOption *p_Option, const QWidget *p_Widget, QStyleHintReturn *p_ReturnData) const
{
    switch (Which) {
        case SH_EtchDisabledText:
            return 0;
        default:
            return QWindowsStyle::styleHint(Which, p_Option, p_Widget, p_ReturnData);
    }
}

/****************************************************************************/
/*!
 *  \brief Reimplemented function returning the outline of subcomponents
 *
 *  \iparam Which = Element indicator
 *  \iparam p_Option = Style parameter
 *  \iparam p_Widget = Requesting widget
 *
 *  \return Outline of the component
 */
/****************************************************************************/
QRect CLeicaStyle::subElementRect(SubElement Which, const QStyleOption *p_Option, const QWidget *p_Widget) const
{
    switch (Which) {
        case SE_RadioButtonIndicator:
            return QRect(0, 0, 37, 37);
        case SE_CheckBoxIndicator:
        case SE_ItemViewItemCheckIndicator:
            return QRect(p_Option->rect.left(), p_Option->rect.top(), 37, 34);
        case SE_TabWidgetTabContents:
        {
            const QStyleOptionTabWidgetFrame *p_TabWidgetFrame = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(p_Option);

            QRect Rect;
            if (p_TabWidgetFrame->shape == QTabBar::RoundedNorth) {
                Rect.setLeft(p_Option->rect.left() + 6);
                Rect.setTop(p_TabWidgetFrame->tabBarSize.height() + p_Option->rect.top() + 4);
            }
            else {
                Rect.setLeft(p_TabWidgetFrame->tabBarSize.width() + p_Option->rect.left() + 6);
                Rect.setTop(p_Option->rect.top() + 4);
            }
            Rect.setRight(p_Option->rect.right() - 4);
            Rect.setBottom(p_Option->rect.bottom() - 1);
            return Rect;
        }
        case SE_CheckBoxFocusRect:
        case SE_PushButtonFocusRect:
        case SE_RadioButtonFocusRect:
            return QRect(0, 0, 0, 0);
        default:
            return QWindowsStyle::subElementRect(Which, p_Option, p_Widget);
    }
}

/****************************************************************************/
/*!
 *  \brief Resizes images, but keeps the scale of the borders
 *
 *  \iparam p_Target = Target image
 *  \iparam p_Source = Source image
 *  \iparam Left = Left border width
 *  \iparam Top = Top border width
 *  \iparam Right = Right border width
 *  \iparam Bottom = Bottom border width
 */
/****************************************************************************/
void CLeicaStyle::BorderPixmap(QPixmap *p_Target, QPixmap *p_Source, qint32 Left, qint32 Top, qint32 Right, qint32 Bottom)
{
    QPainter Painter(p_Target);

    qint32 MiddleWidthTarget = p_Target->width() - Left - Right;
    qint32 MiddleWidthSource = p_Source->width() - Left - Right;
    qint32 MiddleHeightTarget = p_Target->height() - Top - Bottom;
    qint32 MiddleHeightSource = p_Source->height() - Top - Bottom;

    qint32 RightXTarget = p_Target->width() - Right;
    qint32 RightXSource = p_Source->width() - Right;
    qint32 BottomYTarget = p_Target->height() - Bottom;
    qint32 BottomYSource = p_Source->height() - Bottom;

    if (Top != 0) {
        if (Left != 0) {
            Painter.drawPixmap(0, 0, p_Source->copy(0 , 0, Left, Top));
        }

        Painter.drawTiledPixmap(Left, 0, MiddleWidthTarget, Top, p_Source->copy(Left, 0, MiddleWidthSource, Top));

        if (Right != 0) {
            Painter.drawPixmap(RightXTarget, 0, p_Source->copy(RightXSource, 0, Right, Top));
        }
    }

    if (Left != 0) {
        Painter.drawTiledPixmap(0, Top, Left, MiddleHeightTarget, p_Source->copy(0 , Top, Left, MiddleHeightSource));
    }

    Painter.drawTiledPixmap(Left, Top, MiddleWidthTarget, MiddleHeightTarget, p_Source->copy(Left, Top, MiddleWidthSource, MiddleHeightSource));

    if (Right != 0) {
        Painter.drawTiledPixmap(RightXTarget, Top, Right, MiddleHeightTarget, p_Source->copy(RightXSource, Top, Right, MiddleHeightSource));
    }

    if (Bottom != 0) {
        if (Left != 0) {
            Painter.drawPixmap(0, BottomYTarget, p_Source->copy(0, BottomYSource, Left, Bottom));
        }

        Painter.drawTiledPixmap(Left, BottomYTarget, MiddleWidthTarget, Bottom, p_Source->copy(Left, BottomYSource, MiddleWidthSource, Bottom));

        if (Right != 0) {
            Painter.drawPixmap(RightXTarget, BottomYTarget, p_Source->copy(RightXSource, BottomYSource, Right, Bottom));
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Returns the path to a colored button pixmap
 *
 *  \iparam Color = Button color as it is specified in the data manager
 *  \iparam State = State of the button (e.g. pressed or not)
 *
 *  \return Path to the image
 */
/****************************************************************************/
QString CLeicaStyle::PushButtonPath(QColor Color, QStyle::State State)
{
    QString ColorString;

    if (Color == QColor("black")) {
        ColorString = "black";
    }
    else if (Color == QColor("darkblue")) {
        ColorString = "darkblue";
    }
    else if (Color == QColor("green")) {
        ColorString = "green";
    }
    else if (Color == QColor("grey")) {
        ColorString = "grey";
    }
    else if (Color == QColor("lightblue")) {
        ColorString = "lightblue";
    }
    else if (Color == QColor("pink")) {
        ColorString = "pink";
    }
    else if (Color == QColor("red")) {
        ColorString = "red";
    }
    else if (Color == QColor("white")) {
        ColorString = "white";
    }
    else if (Color == QColor("yellow")) {
        ColorString = "yellow";
    }

    if (ColorString.isNull()) {
        if (State & QStyle::State_Sunken) {
            return (QString(":/%1/TextButton/TextButton-Pressed.png").arg(GetStyleSizeString()));
        }
        else {
            return (QString(":/%1/TextButton/TextButton-Enabled.png").arg(GetStyleSizeString()));
        }
    }
    else {
        if (State & QStyle::State_Sunken) {
            return (QString(":/%1/TextButton/TextButtonsColored/ColorButton-").arg(GetStyleSizeString())) + ColorString + "-Pressed.png";
        }
        else {
            return (QString(":/%1/TextButton/TextButtonsColored/ColorButton-") .arg(GetStyleSizeString()))+ ColorString + ".png";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the image set to be used
 *
 *  \iparam Small = Small (true) or large (false) image set
 */
/****************************************************************************/
void CLeicaStyle::SetStyleSize(bool Small)
{
    m_StyleSize = Small;
}

/****************************************************************************/
/*!
 *  \brief Returns which image set is used
 *
 *  \return Small (true) or large (false) image set
 */
/****************************************************************************/
bool CLeicaStyle::GetStyleSize()
{
    return m_StyleSize;
}

/****************************************************************************/
/*!
 *  \brief Returns a string containing "Large" or "Small"
 *
 *  \return "Large" or "Small"
 */
/****************************************************************************/
QString CLeicaStyle::GetStyleSizeString()
{
    if (m_StyleSize == true) {
        return QString("Small");
    }
    else {
        return QString("Large");
    }
}

} // end namespace Application
