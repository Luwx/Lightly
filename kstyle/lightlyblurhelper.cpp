//////////////////////////////////////////////////////////////////////////////
// lightlyblurhelper.cpp
// handle regions passed to kwin for blurring
// -------------------
//
// Copyright (C) 2018 Alex Nemeth <alex.nemeth329@gmail.com>
//
// Largely rewritten from Oxygen widget style
// Copyright (C) 2007 Thomas Luebking <thomas.luebking@web.de>
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "lightlyblurhelper.h"
#include "lightlystyleconfigdata.h"

#include <KWindowEffects>

#include <QEvent>
#include <QMainWindow>
#include <QMenu>
#include <QPair>
#include <QRegularExpression>
#include <QToolBar>
#include <QVector>
#include <QDebug>
namespace
{
	
    QRegion roundedRegion(const QRect &rect, int radius, bool topLeft, bool topRight, bool bottomLeft, bool bottomRight)
    {
        QRegion region(rect, QRegion::Rectangle);

        if (topLeft) {
            // Round top-left corner.
            const QRegion topLeftCorner(rect.x(), rect.y(), radius, radius, QRegion::Rectangle);
            const QRegion topLeftRounded(rect.x(), rect.y(), 2 * radius, 2 * radius, QRegion::Ellipse);
            const QRegion topLeftEar = topLeftCorner - topLeftRounded;
            region -= topLeftEar;
        }
        
        if (topRight) {
            // Round top-right corner.
            const QRegion topRightCorner(
                rect.x() + rect.width() - radius, rect.y(),
                radius, radius, QRegion::Rectangle);
            const QRegion topRightRounded(
                rect.x() + rect.width() - 2 * radius, rect.y(),
                2 * radius, 2 * radius, QRegion::Ellipse);
            const QRegion topRightEar = topRightCorner - topRightRounded;
            region -= topRightEar;
        }

        if (bottomRight) {
            // Round bottom-right corner.
            const QRegion bottomRightCorner(
                rect.x() + rect.width() - radius, rect.y() + rect.height() - radius,
                radius, radius, QRegion::Rectangle);
            const QRegion bottomRightRounded(
                rect.x() + rect.width() - 2 * radius, rect.y() + rect.height() - 2 * radius,
                2 * radius, 2 * radius, QRegion::Ellipse);
            const QRegion bottomRightEar = bottomRightCorner - bottomRightRounded;
            region -= bottomRightEar;
        }
        
        if (bottomLeft){
            // Round bottom-left corner.
            const QRegion bottomLeftCorner(
                rect.x(), rect.y() + rect.height() - radius,
                radius, radius, QRegion::Rectangle);
            const QRegion bottomLeftRounded(
                rect.x(), rect.y() + rect.height() - 2 * radius,
                2 * radius, 2 * radius, QRegion::Ellipse);
            const QRegion bottomLeftEar = bottomLeftCorner - bottomLeftRounded;
            region -= bottomLeftEar;
        }

        return region;
    }
	
}

namespace Lightly
{
    //___________________________________________________________
    BlurHelper::BlurHelper(QObject* parent):
        QObject(parent)
    {
    }

    //___________________________________________________________
    void BlurHelper::registerWidget(QWidget* widget, const bool isDolphin)
    {
        // install event filter
        addEventFilter(widget);

        // schedule shadow area repaint
        update(widget);
        
        _isDolphin = isDolphin;
    }

    //___________________________________________________________
    void BlurHelper::unregisterWidget(QWidget* widget)
    {
        // remove event filter
        widget->removeEventFilter(this);
    }

    //___________________________________________________________
    bool BlurHelper::eventFilter(QObject* object, QEvent* event)
    {

        switch (event->type()) {
            case QEvent::Hide:
            case QEvent::Show:
            case QEvent::Resize:
            {
                // cast to widget and check
                QWidget* widget(qobject_cast<QWidget*>(object));

                if (!widget)
                    break;

                update(widget);
                break;
            }

            default: break;
        }

        // never eat events
        return false;
    }
    
    //___________________________________________________________
    QRegion BlurHelper::blurRegion (QWidget* widget) const
    {
        if (!widget->isVisible()) return QRegion();

        QRect rect = widget->rect();
        QRegion wMask = widget->mask();

        /* blurring may not be suitable when the available
            painting area is restricted by a widget mask */
        if (!wMask.isEmpty() && wMask != QRegion(rect))
            return QRegion();

        else if ((qobject_cast<QMenu*>(widget)
            && !widget->testAttribute(Qt::WA_X11NetWmWindowTypeMenu)) // not a detached menu
            || widget->inherits("QComboBoxPrivateContainer"))
        {
            return roundedRegion(rect, Metrics::Frame_FrameRadius, true, true, true, true);
        } 
        else 
            {
                // blur entire window
                if( widget->palette().color( QPalette::Window ).alpha() < 255 )
                    return roundedRegion(rect, Metrics::Frame_FrameRadius, false, false, true, true);
                
                // blur specific widgets
                QRegion region;
                
                // toolbar and menubar
                if( _translucentTitlebar )
                {
                    // menubar
                    int menubarHeight = 0;
                    if ( QMainWindow *mw = qobject_cast<QMainWindow*>( widget ) )
                    {
                        if ( QWidget *mb = mw->menuWidget() ) 
                        {
                            if ( mb->isVisible() )
                            {
                                region += mb->rect();
                                menubarHeight = mb->height();
                            }
                        }
                    }
                
                    QList<QToolBar *> toolbars = widget->window()->findChildren<QToolBar *>( QString(), Qt::FindDirectChildrenOnly );
                    QRect mainToolbar = QRect();
                    
                    // just assuming
                    Qt::Orientation orientation = Qt::Vertical;
                    
                    // find which one is the main toolbar
                    for( auto tb : toolbars )
                    {
                        // single toolbar
                        if ( tb && tb->isVisible() && toolbars.length() == 1 ) {
                            region += QRegion( QRect( tb->pos(), tb->rect().size() ) );
                            orientation = tb->orientation();
                        }
                        
                        else 
                        {
                            if( mainToolbar.isNull() ) {
                                mainToolbar = QRect( tb->pos(), tb->rect().size() );
                                orientation = tb->orientation();
                            }
                            
                            // test against the previous best caditate
                            else
                            {
                                if( (tb->y() < mainToolbar.y()) || (tb->y() == mainToolbar.y() && tb->x() < mainToolbar.x()) ) {
                                    mainToolbar = QRect( tb->pos(), tb->rect().size() );
                                    orientation = tb->orientation();
                                }
                            }
                        }
                    }
                    
                    if ( mainToolbar.isValid() )
                    {
    
                        // make adjustments
                        if( orientation == Qt::Horizontal )
                        {
                            // toolbar may be at the top but not ocupy the whole avaliable width
                            // so we blur the whole area instead
                            if( mainToolbar.y() == 0 || mainToolbar.y() == menubarHeight )
                            {
                                mainToolbar.setX( 0 );
                                mainToolbar.setWidth( widget->width() );
                                region += mainToolbar;
                            }
                            
                            // round corners if it is at the bottom
                            else if ( mainToolbar.y() + mainToolbar.height() == widget->height() )
                                region += roundedRegion( mainToolbar, Metrics::Frame_FrameRadius,  false, false, false, true );
                            
                            //else
                            //    region += mainToolbar;
                            
                        } else {
                            
                            // round bottom left
                            if( mainToolbar.x() == 0 ) 
                                region += roundedRegion( mainToolbar, Metrics::Frame_FrameRadius,  false, false, true, false );
                            
                            // round bottom right
                            else if( mainToolbar.x() + mainToolbar.width() == widget->width() ) 
                                region += roundedRegion( mainToolbar, Metrics::Frame_FrameRadius,  false, false, false, true );
                            
                            // no round corners
                            //else region += mainToolbar; //FIXME: is this valid?
                        }
                        
                    }
                }

                // dolphin's sidebar
                if( StyleConfigData::dolphinSidebarOpacity() < 100 && _isDolphin )
                {
                    QList<QWidget *> sidebars = widget->findChildren<QWidget *>( QRegularExpression("^(places|terminal|info|folders)Dock$"), Qt::FindDirectChildrenOnly );

                    for ( auto sb : sidebars )
                    {
                        if ( sb && sb->isVisible() )
                        {
                            if( sb->x() == 0 ) 
                                region += roundedRegion( QRect( sb->pos(), sb->rect().size() ), Metrics::Frame_FrameRadius, false, false, true, false);
                            else if ( sb->x() + sb->width() == widget->width() ) 
                                region += roundedRegion( QRect( sb->pos(), sb->rect().size() ), Metrics::Frame_FrameRadius, false, false, false, true);
                            else region += QRect( sb->pos(), sb->rect().size() );
                        }
                    }
                }
                    
                return region;
            }
        }   

    //___________________________________________________________
    void BlurHelper::update(QWidget* widget) const
    {
        /*
        directly from bespin code. Supposedly prevent playing with some 'pseudo-widgets'
        that have winId matching some other -random- window
        */
        if (!(widget->testAttribute(Qt::WA_WState_Created) || widget->internalWinId()))
            return;
        
        QRegion region = blurRegion(widget);
        if (region.isNull()) return;
        
        KWindowEffects::enableBlurBehind(widget->isWindow() ? widget->winId() : widget->window()->winId(), true, region);
        //KWindowEffects::enableBackgroundContrast (widget->isWindow() ? widget->winId() : widget->window()->winId(), true, 1.0, 1.2, 1.3, region );

        // force update
        if (widget->isVisible()) {
            widget->update();
        }
    }
}
