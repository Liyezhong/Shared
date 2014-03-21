/****************************************************************************/
/*! \file LeicaStyle.h
 *
 *  \brief LeicaStyle definition.
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

#ifndef APPLICATION_LEICASTYLE_H
#define APPLICATION_LEICASTYLE_H

#include <QWindowsStyle>

namespace Application {

typedef enum {
    COLORADO_PROJECT,
    SEPIA_PROJECT,
    HIMALAYA_PROJECT
} ProjectId_t;

/****************************************************************************/
/**
 * \brief This class defines the custom look and feel of Leica user
 *        interfaces
 */
/****************************************************************************/
class CLeicaStyle : public QWindowsStyle
{    
public:
    void polish(QPalette &Palette);
    int pixelMetric(PixelMetric Which, const QStyleOption *p_Option, const QWidget *p_Widget = 0) const;
    void drawPrimitive(PrimitiveElement Which, const QStyleOption *p_Option, QPainter *p_Painter, const QWidget *p_Widget = 0) const;
    void drawControl(ControlElement Which, const QStyleOption *p_Option, QPainter *p_Painter, const QWidget *p_Widget = 0) const;
    void drawComplexControl(ComplexControl Which, const QStyleOptionComplex *p_Option, QPainter *p_Painter, const QWidget *p_Widget) const;
    QSize sizeFromContents(ContentsType Which, const QStyleOption *p_Option, const QSize &ContentsSize, const QWidget *p_Widget = 0) const;
    int styleHint(StyleHint Which, const QStyleOption *p_Option, const QWidget *p_Widget, QStyleHintReturn *p_ReturnData) const;
    QRect subElementRect(SubElement Which, const QStyleOption *p_Option, const QWidget *p_Widget) const;

    static void BorderPixmap(QPixmap *p_Target, QPixmap *p_Source, qint32 Left, qint32 Top, qint32 Right, qint32 Bottom);
    static QString PushButtonPath(QColor Color, QStyle::State State);

    static void SetProjectId(Application::ProjectId_t eProj);
    static Application::ProjectId_t GetProjectId();
    static QString GetProjectNameString();

private:
    static Application::ProjectId_t m_ProjId;    //!< Differentiating factor for the different projects under Leica

};

} // end namespace Application

#endif // APPLICATION_LEICASTYLE_H
