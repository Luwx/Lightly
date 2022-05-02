/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

#include "lightlyhelper.h"

#include "lightly.h"

#include <KColorUtils>
#include <KIconLoader>
#include <KWindowSystem>

#include <QApplication>
#include <QPainter>
#include <QtMath>

#if LIGHTLY_HAVE_X11
#include <QX11Info>
#endif

#include <algorithm>

//#include <QDebug>

namespace Lightly
{

    //* contrast for arrow and treeline rendering
    static const qreal arrowShade = 0.15;

    //____________________________________________________________________
    Helper::Helper( KSharedConfig::Ptr config, QObject *parent ):
        _config( std::move( config ) )
    {
        
        if ( qApp ) {
            connect(qApp, &QApplication::paletteChanged, this, [=]() {
                if (qApp->property("KDE_COLOR_SCHEME_PATH").isValid()) {
                    const auto path = qApp->property("KDE_COLOR_SCHEME_PATH").toString();
                    KConfig config(path, KConfig::SimpleConfig);
                    KConfigGroup group( config.group("WM") );
                    const QPalette palette( QApplication::palette() );
                    _activeTitleBarColor = group.readEntry( "activeBackground", palette.color( QPalette::Active, QPalette::Highlight ) );
                    _activeTitleBarTextColor = group.readEntry( "activeForeground", palette.color( QPalette::Active, QPalette::HighlightedText ) );
                    _inactiveTitleBarColor = group.readEntry( "inactiveBackground", palette.color( QPalette::Disabled, QPalette::Highlight ) );
                    _inactiveTitleBarTextColor = group.readEntry( "inactiveForeground", palette.color( QPalette::Disabled, QPalette::HighlightedText ) );
                }
            });
        }
    }

    //____________________________________________________________________
    KSharedConfig::Ptr Helper::config() const
    { return _config; }

    //____________________________________________________________________
    void Helper::loadConfig()
    {
        _viewFocusBrush = KStatefulBrush( KColorScheme::View, KColorScheme::FocusColor );
        _viewHoverBrush = KStatefulBrush( KColorScheme::View, KColorScheme::HoverColor );
        _viewNegativeTextBrush = KStatefulBrush( KColorScheme::View, KColorScheme::NegativeText );
        _windowAlternateBackgroundBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::AlternateBackground );

        const QPalette palette( QApplication::palette() );
        		
        KConfig config(qApp->property("KDE_COLOR_SCHEME_PATH").toString(), KConfig::SimpleConfig);
        KConfigGroup appGroup( config.group("WM") );
        KConfigGroup globalGroup( _config->group("WM") );
        _activeTitleBarColor = appGroup.readEntry( "activeBackground", globalGroup.readEntry( "activeBackground", palette.color( QPalette::Active, QPalette::Highlight ) ) );
        _activeTitleBarTextColor = appGroup.readEntry( "activeForeground", globalGroup.readEntry( "activeForeground", palette.color( QPalette::Active, QPalette::HighlightedText ) ) );
        _inactiveTitleBarColor = appGroup.readEntry( "inactiveBackground", globalGroup.readEntry( "inactiveBackground", palette.color( QPalette::Disabled, QPalette::Highlight ) ) );
        _inactiveTitleBarTextColor = appGroup.readEntry( "inactiveForeground", globalGroup.readEntry( "inactiveForeground", palette.color( QPalette::Disabled, QPalette::HighlightedText ) ) );
        
    }

    //____________________________________________________________________
    QColor Helper::frameOutlineColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        QColor outline( KColorUtils::mix( palette.color( QPalette::Window ), palette.color( QPalette::WindowText ), 0.25 ) );
        //QColor outline( palette.color( QPalette::QPalette::AlternateBase ) );

        // focus takes precedence over hover
        if( mode == AnimationFocus )
        {

            const QColor focus( focusColor( palette ) );
            const QColor hover( hoverColor( palette ) );

            if( mouseOver ) outline = KColorUtils::mix( hover, focus, opacity );
            else outline = KColorUtils::mix( outline, focus, opacity );

        } else if( hasFocus ) {

            outline = focusColor( palette );

        } else if( mode == AnimationHover ) {

            const QColor hover( hoverColor( palette ) );
            outline = KColorUtils::mix( outline, hover, opacity );

        } else if( mouseOver ) {

            outline = hoverColor( palette );

        }

        return outline;

    }

    //____________________________________________________________________
    QColor Helper::hoverOutlineColor( const QPalette& palette ) const
    { return KColorUtils::mix( hoverColor( palette ), palette.color( QPalette::WindowText ), 0.15 ); }

    //____________________________________________________________________
    QColor Helper::buttonFocusOutlineColor( const QPalette& palette ) const
    { return KColorUtils::mix( focusColor( palette ), palette.color( QPalette::ButtonText ), 0.15 ); }

    //____________________________________________________________________
    QColor Helper::buttonHoverOutlineColor( const QPalette& palette ) const
    { return KColorUtils::mix( hoverColor( palette ), palette.color( QPalette::ButtonText ), 0.15 ); }

    //____________________________________________________________________
    QColor Helper::sidePanelOutlineColor( const QPalette& palette, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        QColor outline( qGray(palette.color( QPalette::Window ).rgb()) > 150 ? QColor(0,0,0,20) : QColor(0,0,0,50) );
        return outline;

    }

    //____________________________________________________________________
    QColor Helper::frameBackgroundColor( const QPalette& palette, QPalette::ColorGroup group ) const
    { return KColorUtils::mix( palette.color( group, QPalette::Window ), palette.color( group, QPalette::Base ), 0.3 ); }

    //____________________________________________________________________
    QColor Helper::arrowColor( const QPalette& palette, QPalette::ColorGroup group, QPalette::ColorRole role ) const
    {
        switch( role )
        {
            case QPalette::Text: return KColorUtils::mix( palette.color( group, QPalette::Text ), palette.color( group, QPalette::Base ), arrowShade );
            case QPalette::WindowText: return KColorUtils::mix( palette.color( group, QPalette::WindowText ), palette.color( group, QPalette::Window ), arrowShade );
            case QPalette::ButtonText: return KColorUtils::mix( palette.color( group, QPalette::ButtonText ), palette.color( group, QPalette::Button ), arrowShade );
            default: return palette.color( group, role );
        }

    }

    //____________________________________________________________________
    QColor Helper::arrowColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        QColor outline( arrowColor( palette, QPalette::WindowText ) );
        if( mode == AnimationHover )
        {

            const QColor focus( focusColor( palette ) );
            const QColor hover( hoverColor( palette ) );
            if( hasFocus ) outline = KColorUtils::mix( focus, hover, opacity );
            else outline = KColorUtils::mix( outline, hover, opacity );

        } else if( mouseOver ) {
            // fix skanlite arrow color bug (mouseOver shows dark color (focusColor), not light color (hoverColor))
            outline = focusColor( palette );

        } else if( mode == AnimationFocus ) {

            const QColor focus( focusColor( palette ) );
            outline = KColorUtils::mix( outline, focus, opacity );

        } else if( hasFocus ) {

            outline = focusColor( palette );

        }

        return outline;

    }

    //____________________________________________________________________
    QColor Helper::buttonOutlineColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        QColor outline( KColorUtils::mix( palette.color( QPalette::Button ), palette.color( QPalette::ButtonText ), 0.3 ) );
        if( mode == AnimationHover )
        {

            if( hasFocus )
            {
                const QColor focus( buttonFocusOutlineColor( palette ) );
                const QColor hover( buttonHoverOutlineColor( palette ) );
                outline = KColorUtils::mix( focus, hover, opacity );

            } else {

                const QColor hover( hoverColor( palette ) );
                outline = KColorUtils::mix( outline, hover, opacity );

            }

        } else if( mouseOver ) {

            if( hasFocus ) outline = buttonHoverOutlineColor( palette );
            else outline = hoverColor( palette );

        } else if( mode == AnimationFocus ) {

            const QColor focus( buttonFocusOutlineColor( palette ) );
            outline = KColorUtils::mix( outline, focus, opacity );

        } else if( hasFocus ) {

            outline = buttonFocusOutlineColor( palette );

        }

        return outline;

    }

    //____________________________________________________________________
    QColor Helper::buttonBackgroundColor( const QPalette& palette, bool mouseOver, bool hasFocus, bool sunken, qreal opacity, AnimationMode mode ) const
    {

        QColor background( sunken ?
            KColorUtils::mix( palette.color( QPalette::Button ), palette.color( QPalette::ButtonText ), 0.2 ):
            palette.color( QPalette::Button ) );

        if( mode == AnimationHover )
        {

            const QColor focus( focusColor( palette ) );
            const QColor hover( focusColor( palette ).lighter(115) );
            if( hasFocus ) background = KColorUtils::mix( focus, hover, opacity );

        } else if( mouseOver && hasFocus ) {

            background = focusColor( palette ).lighter(115);

        } else if( mode == AnimationFocus ) {

            const QColor focus( focusColor( palette ) );
            background = KColorUtils::mix( background, focus, opacity );

        } else if( hasFocus ) {

            background = focusColor( palette );

        }

        return background;

    }

    //____________________________________________________________________
    QColor Helper::toolButtonColor( const QPalette& palette, bool mouseOver, bool hasFocus, bool sunken, qreal opacity, AnimationMode mode ) const
    {

        QColor outline;
        const QColor hoverColor( this->hoverColor( palette ) );
        const QColor focusColor( this->focusColor( palette ) );
        const QColor sunkenColor = alphaColor( palette.color( QPalette::WindowText ), 0.2 );

        // hover takes precedence over focus
        if( mode == AnimationHover )
        {

            if( hasFocus ) outline = KColorUtils::mix( focusColor, hoverColor, opacity );
            else if( sunken ) outline = sunkenColor;
            else outline = alphaColor( hoverColor, opacity );

        } else if( mouseOver ) {

            outline = hoverColor;

        } else if( mode == AnimationFocus ) {

            if( sunken ) outline = KColorUtils::mix( sunkenColor, focusColor, opacity );
            else outline = alphaColor( focusColor, opacity );

        } else if( hasFocus ) {

            outline = focusColor;

        } else if( sunken ) {

            outline = sunkenColor;

        }

        return outline;

    }

    //____________________________________________________________________
    QColor Helper::sliderOutlineColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        QColor outline( KColorUtils::mix( palette.color( QPalette::Window ), palette.color( QPalette::WindowText ), 0.4 ) );

        // hover takes precedence over focus
        if( mode == AnimationHover )
        {

            const QColor hover( hoverColor( palette ) );
            const QColor focus( focusColor( palette ) );
            if( hasFocus ) outline = KColorUtils::mix( focus, hover, opacity );
            else outline = KColorUtils::mix( outline, hover, opacity );

        } else if( mouseOver ) {

            outline = hoverColor( palette );

        } else if( mode == AnimationFocus ) {

            const QColor focus( focusColor( palette ) );
            outline = KColorUtils::mix( outline, focus, opacity );

        } else if( hasFocus ) {

            outline = focusColor( palette );

        }
        
        if ( hasFocus || mouseOver ) outline = QColor( 255, 255, 255, 30 );
        else outline = QColor();
        return outline;

    }

    //____________________________________________________________________
    QColor Helper::scrollBarHandleColor( const QPalette& palette, bool mouseOver, bool hasFocus, qreal opacity, AnimationMode mode ) const
    {

        QColor color( alphaColor( palette.color( QPalette::WindowText ), 0.5 ) );

        // hover takes precedence over focus
        if( mode == AnimationHover )
        {

            const QColor hover( hoverColor( palette ) );
            const QColor focus( focusColor( palette ) );
            if( hasFocus ) color = KColorUtils::mix( focus, hover, opacity );
            else color = KColorUtils::mix( color, hover, opacity );

        } else if( mouseOver ) {

            color = hoverColor( palette );

        } else if( mode == AnimationFocus ) {

            const QColor focus( focusColor( palette ) );
            color = KColorUtils::mix( color, focus, opacity );

        } else if( hasFocus ) {

            color = focusColor( palette );

        }

        return color;

    }

    //______________________________________________________________________________
    QColor Helper::checkBoxIndicatorColor( const QPalette& palette, bool mouseOver, bool active, qreal opacity, AnimationMode mode ) const
    {

        QColor color( KColorUtils::mix( palette.color( QPalette::Window ), palette.color( QPalette::WindowText ), 0.6 ) );
        if( mode == AnimationHover )
        {

            const QColor focus( focusColor( palette ) );
            const QColor hover( hoverColor( palette ) );
            if( active ) color =  KColorUtils::mix( focus, hover, opacity );
            else color = KColorUtils::mix( color, hover, opacity );

        } else if( mouseOver ) {

            color = hoverColor( palette );

        } else if( active ) {

            color = focusColor( palette );

        }

        return color;

    }

    //______________________________________________________________________________
    QColor Helper::separatorColor( const QPalette& palette ) const
    { return isDarkTheme( palette ) ? QColor(255, 255, 255, 16) : QColor(0, 0, 0, 16); }

    //______________________________________________________________________________
    QPalette Helper::disabledPalette( const QPalette& source, qreal ratio ) const
    {

        QPalette copy( source );

        const QList<QPalette::ColorRole> roles = { QPalette::Background, QPalette::Highlight, QPalette::WindowText, QPalette::ButtonText, QPalette::Text, QPalette::Button };
        foreach( const QPalette::ColorRole& role, roles )
        { copy.setColor( role, KColorUtils::mix( source.color( QPalette::Active, role ), source.color( QPalette::Disabled, role ), 1.0-ratio ) ); }

        return copy;
    }

    //____________________________________________________________________
    QColor Helper::alphaColor( QColor color, qreal alpha ) const
    {
        if( alpha >= 0 && alpha < 1.0 )
        { color.setAlphaF( alpha*color.alphaF() ); }
        return color;
    }

    //______________________________________________________________________________
    void Helper::renderDebugFrame( QPainter* painter, const QRect& rect ) const
    {
        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );
        painter->setBrush( Qt::NoBrush );
        painter->setPen( Qt::red );
        painter->drawRect( strokedRect( rect ) );
        painter->restore();
    }

    //______________________________________________________________________________
    void Helper::renderFocusLine( QPainter* painter, const QRect& rect, const QColor& color ) const
    {
        if( !color.isValid() ) return;

        painter->save();
        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->setBrush( Qt::NoBrush );
        painter->setPen( color );

        painter->translate( 0, 2 );
        painter->drawLine( rect.bottomLeft(), rect.bottomRight() );
        painter->restore();
    }

    //______________________________________________________________________________
    void Helper::renderFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QPalette& palette, const bool windowActive, const bool enabled ) const
    {

        painter->setRenderHint( QPainter::Antialiasing );

        //QRectF frameRect( rect.adjusted( 1, 1, -1, -1 ) );
        QRectF frameRect( rect.adjusted( Metrics::Frame_FrameWidth, Metrics::Frame_FrameWidth, -Metrics::Frame_FrameWidth, -Metrics::Frame_FrameWidth ) );
        //QRectF frameRect(rect);
        qreal radius( frameRadius( PenWidth::NoPen, -1 ) );
        painter->setPen( Qt::NoPen );
        
        
        if( enabled ){ 
            /*if( isDarkTheme( palette ) )
                renderRectShadow(painter, frameRect, QColor( Qt::black ), 6, 2, 1, 0, 1, radius-1, true, 10);
            else*/ 
                renderBoxShadow(painter, frameRect, 0, 1, 5, QColor(0,0,0,215), radius, windowActive);

        }
        else renderOutline(painter, frameRect, radius, 10);

        // set pen
        painter->setPen( Qt::NoPen );

        // set brush
        /*if( color.isValid() ) painter->setBrush( color );
        else*/ painter->setBrush( color );
        
        

        // render
        painter->drawRoundedRect( frameRect.adjusted(-1, -1, 1, 1), radius, radius );

    }

    //______________________________________________________________________________
    void Helper::renderSidePanelFrame( QPainter* painter, const QRect& rect, const QColor& outline, Side side ) const
    {

        // check color
        if( !outline.isValid() ) return;

        // adjust rect
        QRectF frameRect( strokedRect( rect ) );

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing );
        painter->setPen( outline );

        // render
        switch( side )
        {
            default:
            case SideLeft:
            painter->drawLine( frameRect.topRight(), frameRect.bottomRight() );
            break;

            case SideTop:
            painter->drawLine( frameRect.topLeft(), frameRect.topRight() );
            break;

            case SideRight:
            painter->drawLine( frameRect.topLeft(), frameRect.bottomLeft() );
            break;

            case SideBottom:
            painter->drawLine( frameRect.bottomLeft(), frameRect.bottomRight() );
            break;

            case AllSides:
            {
                const qreal radius( frameRadius( PenWidth::Frame, -1 ) );
                painter->drawRoundedRect( frameRect, radius, radius );
                break;
            }

        }

    }

    //______________________________________________________________________________
    void Helper::renderMenuFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QColor& outline, bool roundCorners ) const
    {


        // set brush
        if( color.isValid() ) painter->setBrush( color );
        else painter->setBrush( Qt::NoBrush );

        if( roundCorners )
        {

            painter->setRenderHint( QPainter::Antialiasing );
            QRectF frameRect( rect );
            qreal radius( frameRadius( PenWidth::NoPen ) );

           painter->setPen( Qt::NoPen );

            // render
            painter->drawRoundedRect( frameRect, radius, radius );
            
            //outline
            if( outline.isValid() )
            {
                painter->setPen( outline );
                painter->setBrush( Qt::NoBrush );
                frameRect = strokedRect( frameRect );
                radius += 0.5;  // enhance pixel aligment
                
                painter->drawRoundedRect( frameRect, radius, radius );
            }

        } else {

            painter->setRenderHint( QPainter::Antialiasing, false );
            QRect frameRect( rect );
            if( outline.isValid() )
            {

                painter->setPen( outline );
                frameRect.adjust( 0, 0, -1, -1 );

            } else painter->setPen( Qt::NoPen );

            painter->drawRect( frameRect );

        }

    }
    
    //______________________________________________________________________________
    void Helper::renderOutline(
        QPainter* painter, const QRectF& rect, const int radius, const int outlineStrength ) const
    {
        
            painter->setPen( QColor( 0, 0, 0, outlineStrength ) );
            painter->setBrush( Qt::NoBrush );
            QRectF frameRect (QRectF( rect.left() - 1, rect.top() - 1, rect.width() + 2, rect.height() + 2 ));
            frameRect.adjust( 0.5, 0.5, -0.5, -0.5 );
            painter -> drawRoundedRect( frameRect, radius, radius );
            painter->setPen(Qt::NoPen);

    }
    
    //______________________________________________________________________________
    void Helper::renderBoxShadow(
        QPainter* painter, const QRect& rect, const int xOffset, const int yOffset, const int size, const QColor& color, const int cornerRadius, const bool active, TileSet::Tiles tiles) const
    {
        
        if( !StyleConfigData::widgetDrawShadow() ) return;
        Q_UNUSED(active)
        //if (!active) {renderOutline(painter, rect, cornerRadius, 30);return;}
        CustomShadowParams params = CustomShadowParams( QPoint(xOffset, yOffset), size, color );
        TileSet shadow = ShadowHelper::shadowTiles( cornerRadius, params );
        shadow.render( rect.adjusted(-params.radius, -params.radius, params.radius + params.offset.x(), params.radius + params.offset.y() ) , painter, tiles);
        //qDebug() << "shadow on: " << rect.adjusted(-params.radius, -params.radius, params.radius, params.radius);
        
    }
    
    //______________________________________________________________________________
    void Helper::renderEllipseShadow(
        QPainter* painter, const QRectF& rect, QColor color,
        const int size, const float param1, const float param2, 
        const int xOffset, const int yOffset, const bool outline, const int outlineStrength ) const
    {
        painter->setPen( Qt::NoPen );
        
        if (outline) { 
            painter->setBrush( QColor( 0, 0, 0, outlineStrength ) );
            painter -> drawEllipse( QRect( rect.left() - 1, rect.top() - 1, rect.width() + 2, rect.height() + 2 ) );
        }
        
        if ( size < 1 ) return;
        if (color.alphaF() < 0.01) return;
        
        // temporaty values
        int tx = rect.left() - size + xOffset;
        int ty = rect.top() - size + yOffset;
        int tw = rect.width() + size * 2;
        int th = rect.height() + size * 2;
        float alpha = color.alphaF();
        
        while (tx <= rect.left() + qMax(xOffset, yOffset)) {
            
            color.setAlpha( alpha );
            painter->setBrush( color );
            
            painter->drawEllipse( QRect(tx, ty, tw, th) );
            
            tx++;
            ty++;
            tw -= 2;
            th -= 2;
            alpha += param1 + alpha/param2;
        }
    }
    
    //______________________________________________________________________________
    void Helper::topHighlight( QPainter* painter, const QRectF& rect, const int radius, const QColor& color ) const
    {
        QPixmap pixmap = QPixmap(rect.width(), rect.height());
        pixmap.fill( Qt::transparent );
        QPainter p( &pixmap );
        
        p.setRenderHint( QPainter::Antialiasing );
        
        p.setPen( Qt::NoPen );
        p.setBrush( color );
        p.drawRoundedRect( QRect( 0, 0, rect.width(), rect.height() ), radius, radius );
        
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush( Qt::black );
        p.drawRoundedRect( QRect( 0, 1, rect.width(), rect.height() ), radius, radius );
        
        painter->drawPixmap( QRect( rect.x(), rect.y(), rect.width(), rect.height() ), pixmap );
        
    }

    //______________________________________________________________________________
    void Helper::renderButtonFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, const QPalette& palette,
        const bool hasFocus, const bool sunken, const  bool mouseOver, const bool enabled, const bool windowActive, const AnimationMode mode, const qreal opacity ) const
     {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );
        
        painter->setPen( Qt::NoPen );

        // copy rect
        QRectF frameRect( rect );
        
        // reduce the size of the actual button, the rest will be the shadow
        frameRect.adjust( 5, 5, -5, -5 );
        
        qreal radius( frameRadius() - 1 );

        if( sunken ) {

            frameRect.translate( 0, 1 );
            renderBoxShadow( painter, frameRect, 0, 1, 2, QColor( 0, 0, 0, 120 ), radius, windowActive );

        } else if ( enabled && color.alphaF() == 1 ) { // shadow
            
            if ( mouseOver || hasFocus ){
                //frameRect.translate( 0, -1 ); feels cheap without animations
                
                QColor shadowColor = hasFocus ? color.darker(220) : QColor( 0 ,0 ,0 ,170 );
                renderBoxShadow( painter, frameRect, 0, 1, 6, shadowColor, radius, windowActive );
                
            } else 
                renderBoxShadow( painter, frameRect, 0, 1, 3, QColor( 0, 0, 0, 120 ), radius, windowActive );
        }

        // content
        if( color.isValid() )
            painter->setBrush( sunken ? focusColor(palette).darker(110) : mouseOver ? color.lighter( hasFocus ? 102 : 105 ) : color );

        else painter->setBrush( Qt::NoBrush ); 

        // render
        painter->drawRoundedRect( frameRect, radius, radius );

        if( isDarkTheme( palette ) && enabled ) topHighlight( painter, frameRect, StyleConfigData::cornerRadius() );
        
        // pressed animation
        if (mode == AnimationPressed){
            
            QRegion oldRegion( painter->clipRegion() );
            // constrain ripple effect area
            painter->setClipRect( frameRect, Qt::IntersectClip );
            
            // ripple color
            painter->setBrush(alphaColor( color.darker(200), sunken ? 0.5 : 0.5*(1-opacity) ) );
            
            // Pythagorean theorem
            int finalRadius = qCeil( qSqrt( qPow(frameRect.width()/2, 2) + qPow(frameRect.height()/2, 2) ) );
            
            // the animaiton looks choppy if the initial radius is 0, we choose something else
            int initRadius = qCeil( frameRect.height()/2 );
            painter->drawEllipse(frameRect.center(), initRadius + (finalRadius-initRadius)*opacity, initRadius + (finalRadius-initRadius)*opacity);
            painter->setClipRegion( oldRegion );

        }
        
        // pressed button background when animation is done
        else if(sunken && mouseOver) {
            // fix kcharselect combobox sunken: use the same background color when combobox is sunken
            const auto fc = focusColor(palette);
            painter->setBrush(alphaColor( fc.darker(200), 0.5 ) );
             painter->drawRoundedRect( frameRect, radius, radius );
        }
        
    }

    //______________________________________________________________________________
    void Helper::renderToolButtonFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, bool sunken ) const
    {

        // do nothing for invalid color
        if( !color.isValid() ) return;

        // setup painter
        painter->setRenderHints( QPainter::Antialiasing );

        const QRectF baseRect( rect.adjusted( 1, 1, -1, -1 ) );

        if( sunken )
        {

            const qreal radius( frameRadius( PenWidth::NoPen ) );

            painter->setPen( Qt::NoPen );
            painter->setBrush( color );

            painter->drawRoundedRect( baseRect, radius, radius );

        } else {

            const qreal radius( frameRadius( PenWidth::Frame ) );

            painter->setPen( color );
            painter->setBrush( Qt::NoBrush );
            const QRectF outlineRect( strokedRect( baseRect ) );
            painter->drawRoundedRect( outlineRect, radius, radius );

        }

    }

    //______________________________________________________________________________
    void Helper::renderToolBoxFrame(
        QPainter* painter, const QRect& rect, int tabWidth,
        const QColor& outline ) const
    {

        if( !outline.isValid() ) return;

        // round radius
        const qreal radius( frameRadius( PenWidth::Frame ) );
        const QSizeF cornerSize( 2*radius, 2*radius );

        // if rect - tabwidth is even, need to increase tabWidth by 1 unit
        // for anti aliasing
        if( !((rect.width() - tabWidth)%2) ) ++tabWidth;

        // adjust rect for antialiasing
        QRectF baseRect( strokedRect( rect ) );

        // create path
        QPainterPath path;
        path.moveTo( 0, baseRect.height()-1 );
        path.lineTo( ( baseRect.width() - tabWidth )/2 - radius, baseRect.height()-1 );
        path.arcTo( QRectF( QPointF( ( baseRect.width() - tabWidth )/2 - 2*radius, baseRect.height()-1 - 2*radius ), cornerSize ), 270, 90 );
        path.lineTo( ( baseRect.width() - tabWidth )/2, radius );
        path.arcTo( QRectF( QPointF( ( baseRect.width() - tabWidth )/2, 0 ), cornerSize ), 180, -90 );
        path.lineTo( ( baseRect.width() + tabWidth )/2 -1 - radius, 0 );
        path.arcTo( QRectF(  QPointF( ( baseRect.width() + tabWidth )/2  - 1 - 2*radius, 0 ), cornerSize ), 90, -90 );
        path.lineTo( ( baseRect.width() + tabWidth )/2 -1, baseRect.height()-1 - radius );
        path.arcTo( QRectF( QPointF( ( baseRect.width() + tabWidth )/2 -1, baseRect.height()-1 - 2*radius ), cornerSize ), 180, 90 );
        path.lineTo( baseRect.width()-1, baseRect.height()-1 );

        // render
        painter->setRenderHints( QPainter::Antialiasing );
        painter->setBrush( Qt::NoBrush );
        painter->setPen( outline );
        painter->translate( baseRect.topLeft() );
        painter->drawPath( path );

    }

    //______________________________________________________________________________
    void Helper::renderTabWidgetFrame(
        QPainter* painter, const QRect& rect,
        const QColor& color, Corners corners, const bool windowActive ) const
    {

        painter->setRenderHint( QPainter::Antialiasing );

        //QRectF frameRect( rect.adjusted( 1, 1, -1, -1 ) );
        QRectF frameRect( rect.adjusted( Metrics::Frame_FrameWidth, Metrics::Frame_FrameWidth, - Metrics::Frame_FrameWidth, - Metrics::Frame_FrameWidth ) );
        qreal radius( frameRadius( PenWidth::NoPen, -1 ) );
        
        //shadow
        renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,115) , radius, windowActive ); 

        painter->setPen( Qt::NoPen );

        // set brush
        if( color.isValid() ) painter->setBrush( color );
        else painter->setBrush( Qt::NoBrush );

        // render
        QPainterPath path( roundedPath( frameRect, corners, radius ) );
        painter->drawPath( path );
    }


    //______________________________________________________________________________
    void Helper::renderSelection(
        QPainter* painter, const QRect& rect,
        const QColor& color, Corners corners ) const
    {

        painter->setRenderHint( QPainter::Antialiasing );
        painter->setPen( Qt::NoPen );
        painter->setBrush( color );
        
        QPainterPath path( roundedPath( rect, corners, StyleConfigData::cornerRadius() ) );
        painter->drawPath( path );

    }

    //______________________________________________________________________________
    void Helper::renderSeparator(
        QPainter* painter, const QRect& rect,
        const QColor& color, bool vertical ) const
    {

        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->setBrush( Qt::NoBrush );
        painter->setPen( color );

        if( vertical )
        {

            painter->translate( rect.width()/2, 0 );
            painter->drawLine( rect.topLeft(), rect.bottomLeft() );

        } else {

            painter->translate( 0, rect.height()/2 );
            painter->drawLine( rect.topLeft(), rect.topRight() );


        }

        
    }
    
    //______________________________________________________________________________
    void Helper::renderLineEdit(
        QPainter* painter, const QRect& rect,
        const QColor& background, const QColor& outline, const bool hasFocus, const bool mouseOver, bool enabled, const bool windowActive, const AnimationMode mode, const qreal opacity ) const
    {

        painter->setRenderHint( QPainter::Antialiasing );

        QRectF frameRect( rect.adjusted( Metrics::Frame_FrameWidth, Metrics::Frame_FrameWidth, -Metrics::Frame_FrameWidth, -Metrics::Frame_FrameWidth ) );
        qreal radius( frameRadius( PenWidth::NoPen, -1 ) );
        
        painter->setPen( Qt::NoPen );
        if (enabled)
        {
            // draw shadow
            if( hasFocus )
            { 
                frameRect.adjust(1, 1, -1, -1);
                // focus in animation 
                if( mode == 2 && opacity > 0 && opacity < 1) {
                    
                    // shadow opacity animation
                    //old shadow
                    //renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,84*(1-opacity)), radius, windowActive );
                    //renderOutline(painter, frameRect, radius, 6*(1-opacity));
                    //painter->setPen( Qt::NoPen );
                    // new shadow
                    //renderBoxShadow( painter, frameRect, 0, 1, 6, alphaColor(outline.darker(120), opacity) , radius, windowActive ); 
                    //renderBoxShadow( painter, frameRect, 0, 1, 4, alphaColor(outline.darker(120), opacity) , radius, windowActive );

                    const qreal finalRadius ((frameRect.width()+Metrics::Frame_FrameWidth)*opacity);
                    
                    QPixmap mask = QPixmap(rect.width(), rect.height());	
                    mask.fill( Qt::transparent );

                    QPainter pmask( &mask );	
                    pmask.setRenderHint( QPainter::Antialiasing );
                    pmask.fillRect(rect, Qt::black);
                    pmask.setPen( Qt::NoPen );
                    pmask.setBrush( Qt::black );
                    pmask.setCompositionMode(QPainter::CompositionMode_SourceOut);
                    pmask.drawEllipse(QPointF(frameRect.x(), frameRect.y() + frameRect.height()/2), finalRadius, finalRadius);
                    pmask.end();
                    
                    QPixmap pixmap = QPixmap(rect.width(), rect.height());	
                    pixmap.fill( Qt::transparent );
                    QPainter p( &pixmap );	
                    p.setOpacity(0.3 + 0.7*opacity);
                    p.setRenderHint( QPainter::Antialiasing );
                    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    p.setPen(Qt::NoPen);
                    renderBoxShadow( &p, frameRect, 0, 1, 6, outline.darker(120) , radius, windowActive ); // comment this out for only the outline animation
                    renderBoxShadow( &p, frameRect, 0, 1, 4, outline.darker(130) , radius, windowActive ); // comment this out for only the outline animation
                    renderBoxShadow( &p, frameRect, 0, 1, 4, outline.darker(140) , radius, windowActive ); // comment this out for only the outline animation
                    p.setBrush( alphaColor( outline, 0.6 ) ) ;
                    QRectF focusFrame = frameRect.adjusted( -2, -2, 2, 2 );
                    p.drawRoundedRect( focusFrame, radius + 1, radius + 1); // outline around lineedit
                    
                    // mask
                    //p.setOpacity(1);  // uncomment this for only outline animation
                    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
                    p.drawPixmap(rect, mask);
                    p.end();
                    
                    painter->drawPixmap( rect, pixmap );
                }
                
                // focus animation done
                else {
                    renderBoxShadow( painter, frameRect, 0, 1, 7, outline.darker(120) , radius, windowActive ); 
                    renderBoxShadow( painter, frameRect, 0, 1, 5, outline.darker(130) , radius, windowActive );
                    renderBoxShadow( painter, frameRect, 0, 1, 4, outline.darker(140) , radius, windowActive );
                    painter->setBrush( alphaColor( outline, 0.6 ) ) ;
                    QRectF focusFrame = frameRect.adjusted( -2, -2, 2, 2 );
                    painter->drawRoundedRect( focusFrame, radius + 1, radius + 1);
                }
            }
            
            // mouse over or normal state
            else {
                
                // focus out animation
                if( mode == 2 && opacity > 0 && opacity < 1) {
                    
                    const qreal finalRadius ((frameRect.width()+Metrics::Frame_FrameWidth)*opacity);
                    
                    QPixmap mask = QPixmap(rect.width(), rect.height());	
                    mask.fill( Qt::transparent );

                    QPainter pmask( &mask );
                    pmask.setOpacity(1);
                    pmask.setRenderHint( QPainter::Antialiasing );
                    pmask.fillRect(rect, Qt::black);
                    pmask.setPen( Qt::NoPen );
                    pmask.setBrush( Qt::black );
                    pmask.setCompositionMode(QPainter::CompositionMode_SourceOut);
                    pmask.drawEllipse(QPointF(frameRect.x(), frameRect.y() + frameRect.height()/2), finalRadius, finalRadius);
                    pmask.end();
                    
                    QPixmap pixmap = QPixmap(rect.width(), rect.height());	
                    pixmap.fill( Qt::transparent );
                    QPainter p( &pixmap );	
                    //p.setOpacity(0.3 + 0.6*opacity);
                    p.setRenderHint( QPainter::Antialiasing );
                    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    p.setPen(Qt::NoPen);
                    renderBoxShadow( &p, frameRect, 0, 1, 6, outline.darker(120) , radius, windowActive ); 
                    renderBoxShadow( &p, frameRect, 0, 1, 4, outline.darker(120) , radius, windowActive );
                    p.setBrush( alphaColor( outline, 0.6 ) ) ;
                    QRectF focusFrame = frameRect.adjusted( -1, -1, 1, 1 );
                    p.drawRoundedRect( focusFrame, radius + 1, radius + 1);
                    
                    // mask
                    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
                    p.drawPixmap(rect, mask);
                    p.end();
                    
                    painter->drawPixmap( rect, pixmap );
                    
                    // unfocused lineedit shadow effect
                    renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,84*(1-opacity)), radius, windowActive );
                    renderOutline(painter, frameRect, radius, 6*(1-opacity));
                    painter->setPen( Qt::NoPen );
                    
                }
                
                // normal or mouse over 
                else {
                    if ( mouseOver && !hasFocus ) renderBoxShadow( painter, frameRect, 0, 1, 6, QColor(0,0,0,160), radius, windowActive );
                    else {
                        renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,84), radius, windowActive );
                        renderOutline(painter, frameRect, radius, 6);
                        painter->setPen( Qt::NoPen );
                    }
                }
                
            }
        }
            

        // set brush
        if( background.isValid() ) painter->setBrush( background );
        else painter->setBrush( Qt::NoBrush );
        
        if( hasFocus ) radius--;

        // render
        painter->drawRoundedRect( frameRect, radius, radius );
    }
    
    //______________________________________________________________________________
    void Helper::renderGroupBox(
        QPainter* painter, const QRect& rect,
        const QColor& color, const bool mouseOver ) const
    {
        Q_UNUSED(mouseOver)
        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->setPen( Qt::NoPen );
        
        
        QRectF frameRect( rect.adjusted( Metrics::Frame_FrameWidth, Metrics::Frame_FrameWidth, -Metrics::Frame_FrameWidth, -Metrics::Frame_FrameWidth ) );
        qreal radius( frameRadius( PenWidth::NoPen, -1 ) );

        // draw shadow
        /*if( mouseOver ) {
            
            renderBoxShadow( painter, frameRect, CustomShadowParams( QPoint( 0, 1 ), 7, QColor(0,0,0,125) ), radius );
            renderOutline( painter, frameRect, radius, 6);
        }
        else {*/
            renderBoxShadow( painter, frameRect, 0, 1, 4, QColor(0,0,0,70), radius, true );
            renderOutline( painter, frameRect, radius, 6);
        //}
        painter->setPen(Qt::NoPen);

        // set brush
        if( color.isValid() ) painter->setBrush( color );
        else painter->setBrush( Qt::NoBrush ); 

        // render
        painter->drawRoundedRect( frameRect, radius, radius );
        
        /*if( mouseOver ) {
            painter->setBrush(QColor(255, 255, 255, 30));
            painter->drawRoundedRect( frameRect, radius, radius );
        }*/

    }

    //______________________________________________________________________________
    void Helper::renderCheckBox(
        QPainter* painter, const QRect& rect, const QPalette& palette, const bool isInMenu,
        bool sunken, const bool mouseOver, CheckBoxState state, const bool windowActive, qreal animation ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->setPen( Qt::NoPen );

        // copy rect and radius
        QRectF frameRect( rect );
        frameRect.adjust( Metrics::Frame_FrameWidth - 1, Metrics::Frame_FrameWidth - 1, - Metrics::Frame_FrameWidth + 1, -Metrics::Frame_FrameWidth + 1 );
        qreal radius( qRound( frameRadius( PenWidth::NoPen )/2 ) );
        
        // setup colors
        const bool darkTheme( isDarkTheme( palette ) );
        const QColor color ( palette.color( QPalette::HighlightedText ) );
        QColor background (state == CheckOn ? palette.color( QPalette::Highlight ) : palette.color( QPalette::Button ));
        if( isInMenu ) background = background.lighter(115);

        // float and sunken effect
        if( sunken ) {
            frameRect.translate(1, 1);
            background = background.darker(115);
        }
        else if( state == CheckOn || (state == CheckOff && mouseOver) ) frameRect.translate(-1, -1);
        
        if( state == CheckOff)
        {
            // shadow
            if( mouseOver && !sunken){ 
                renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,120), 1, windowActive );
                renderBoxShadow( painter, frameRect, 0, 1, 2, QColor(0,0,0,90), radius, windowActive );
            }
            else {
                renderBoxShadow( painter, frameRect, 0, 1, 2, QColor(0,0,0,160), radius, windowActive );
                renderOutline( painter, frameRect, radius, 4 );
            }
            painter->setBrush( mouseOver ? background.lighter(110) : background );
            painter->drawRoundedRect( frameRect, radius, radius );
            
        } else if( state == CheckOn ) { //mark
            
            if ( darkTheme ) renderBoxShadow( painter, frameRect, 0, 1, 4, mouseOver ? background.darker(140):background.darker(200), radius, windowActive );
            else {
                renderBoxShadow( painter, frameRect, 0, 1, 4, background.darker(220), radius, windowActive );
                renderOutline( painter, frameRect, radius, 4 );
            }
            painter->setBrush( mouseOver ? background.lighter(110) : background );
            painter->drawRoundedRect( frameRect, radius, radius );
            
            //draw check mark
            const int x = frameRect.x();
            const int y = frameRect.y();
            
            QPen pen = QPen();
            pen.setWidth(2);
            pen.setCapStyle( Qt::RoundCap );
            pen.setJoinStyle( Qt::RoundJoin );
            pen.setColor( QColor( 0, 0, 0, 100 ) );
            
            painter->setPen( pen );
            painter->setBrush( Qt::NoBrush );
            
            QPainterPath checkShadow;
            checkShadow.moveTo(5+x, 8+y);
            checkShadow.lineTo(6+x, 12+y);
            checkShadow.lineTo(12+x, 6+y);
            painter->drawPath( checkShadow );

            QPainterPath check;
            pen.setColor( color );
            painter->setPen( pen );
            check.moveTo(5+x, 7+y);
            check.lineTo(6+x, 11+y);
            check.lineTo(12+x, 5+y);
            painter->drawPath( check );

        } else if( state == CheckPartial ) {

            if ( darkTheme ) {
                renderBoxShadow( painter, frameRect, 0, 1, 4, mouseOver ? background.darker(140):background.darker(200), radius, windowActive );
            }
            else {
                renderBoxShadow( painter, frameRect, 0, 1, 4, background.darker(220), radius, windowActive );
                renderOutline( painter, frameRect, radius, 4 );
            }
            painter->setBrush( mouseOver ? background.lighter(110) : background );
            painter->drawRoundedRect( frameRect, radius, radius );
            
            const int x = frameRect.x();
            const int y = frameRect.y();
            
            painter->setBrush( color );
            painter->drawEllipse(3+x, 7+y, 2, 2);
            painter->drawEllipse(7+x, 7+y, 2, 2);
            painter->drawEllipse(11+x, 7+y, 2, 2);
            
            

        } else if( state == CheckAnimated ) {
            
            if( animation == 0 ) {

                    // small shadow
                    if( mouseOver && !sunken){ 
                        renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,120), 1, windowActive );
                        renderBoxShadow( painter, frameRect, 0, 1, 2, QColor(0,0,0,90), radius, windowActive );
                    }
                    else {
                        renderBoxShadow( painter, frameRect, 0, 1, 2, QColor(0,0,0,160), radius, windowActive );
                        renderOutline( painter, frameRect, radius, 4 );
                    }
                    painter->setBrush( mouseOver ? background.lighter(115) : background );
                    painter->drawRoundedRect( frameRect, radius, radius );
                
            }
            else if( (animation > 0 && animation < 1) || animation == -1) {
                    
                    if( animation == -1 ) animation = 1.0;
                    frameRect.translate(-1, -1);
                    if ( darkTheme ) renderBoxShadow( painter, frameRect, 0, 1, 4, mouseOver ? background.darker(140):background.darker(200), radius, windowActive );
                    else {
                        renderBoxShadow( painter, frameRect, 0, 1, 5, QColor(0,0,0,120), 1, windowActive );
                        renderBoxShadow( painter, frameRect, 0, 1, 2, QColor(0,0,0,90), radius, windowActive );
                    }
                    painter->setBrush( mouseOver ? background.lighter(110) : background );
                    //painter->setBrush(background);
                    painter->drawRoundedRect( frameRect, radius, radius );
                    
                    background = mouseOver ? palette.color( QPalette::Highlight ).lighter(110) : palette.color( QPalette::Highlight ); 
                    painter->setBrush( alphaColor(background, animation) );
                    painter->drawRoundedRect( frameRect, radius, radius );
                            
                    //draw check mark
                    const int x = frameRect.x();
                    const int y = frameRect.y();
                    
                    QPen pen = QPen();
                    pen.setWidth(2);
                    pen.setCapStyle( Qt::RoundCap );
                    pen.setColor( QColor( 0, 0, 0, 100*animation ) );
                    
                    painter->setPen( pen );
                    painter->setBrush( Qt::NoBrush );
                    
                    QPainterPath checkShadow;
                    checkShadow.moveTo(animation*5+x, 8+y);
                    checkShadow.lineTo(animation*6+x, 12+y);
                    checkShadow.lineTo(animation*12+x, 6+y);
                    painter->drawPath( checkShadow );

                    QPainterPath check;
                    pen.setColor( alphaColor(color, 1.0*animation) );
                    painter->setPen( pen );
                    check.moveTo(animation*5+x, 7+y);
                    check.lineTo(animation*6+x, 11+y);
                    check.lineTo(animation*12+x, 5+y);
                    painter->drawPath( check );
                } 
            }

        if( darkTheme ) topHighlight( painter, frameRect, radius );

    }

    //______________________________________________________________________________
    void Helper::renderRadioButton(
        QPainter* painter, const QRect& rect,
        const QPalette& palette, const bool mouseOver,
        bool sunken, RadioButtonState state, const bool isInMenu, qreal animation ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->setPen( Qt::NoPen );
        
         // setup colors
        const bool darkTheme( isDarkTheme( palette ) );
        const QColor color ( palette.color( QPalette::HighlightedText ) );
        QColor background (state == RadioOn ? palette.color( QPalette::Highlight ) : palette.color( QPalette::Button ));
        if( isInMenu ) background = background.lighter(115);

        // copy rect
        QRectF frameRect( rect );
        frameRect.adjust( Metrics::Frame_FrameWidth - 1, Metrics::Frame_FrameWidth - 1, - Metrics::Frame_FrameWidth + 1, -Metrics::Frame_FrameWidth + 1 );

        // float and sunken effect
        if( sunken ) frameRect.translate( 1, 1 );
        else if( state == RadioOn || (state == RadioOff && mouseOver) ) frameRect.translate(-1, -1);

        // mark
        if( state == RadioOn )
        {
            
            // strong shadows don't look good with light themes
            if( darkTheme ) renderEllipseShadow(painter, frameRect, mouseOver ? background.darker(110) : background.darker(200), 4, 8, 5, 0, 1, true, 15);
            else renderEllipseShadow(painter, frameRect, mouseOver ? background.darker(110) : background.darker(200), 4, 4, 6, 0, 1, true, 8);

            painter->setBrush( mouseOver ? background.lighter(110) : background );
            painter->drawEllipse( frameRect );

            // inner ellipse
            const QRectF markerRect( frameRect.adjusted( 4, 4, -4, -4 ) );

            renderOutline(painter, markerRect, markerRect.width()/2, 35);
            
            painter->setBrush( color );
            painter->drawEllipse( markerRect );

        } else if ( state == RadioOff ) 
        {
            
            if( mouseOver ) renderEllipseShadow(painter, frameRect, QColor(0,0,0), 5, 1, 4, 0, 1, true, 15);
            else renderEllipseShadow(painter, frameRect, QColor(0,0,0), 2, 12, 3, 0, 1, true, 15);
            painter->setBrush( mouseOver ? background.lighter(115) : background );
            painter->drawEllipse( frameRect );
            
        } else if( state == RadioAnimated ) {
            
            if( animation > 0 /*&& animation < 1*/ ) {
                
                // make it more elastic
                if(animation > 1) animation *= 1.1;
                
                frameRect.translate(-1*animation, -1*animation);

                // radioOff shadow fade
                if( mouseOver ) renderEllipseShadow(painter, frameRect.adjusted(1, 1, 1, 1), alphaColor(QColor(0,0,0), 1-animation), 5, 1, 4, 0, 1, true, 15*(1-animation));
                else renderEllipseShadow(painter, frameRect.adjusted(1, 1, 1, 1), alphaColor(QColor(0,0,0), 1-animation), 2, 12, 3, 0, 1, true, 15*(1-animation));
                
                // strong shadows don't look good with light themes
                QColor shadowColor( mouseOver ? alphaColor(background.darker(110), qMin( animation, 1.0)) : alphaColor(background.darker(200), qMin( animation, 1.0)) );
                if( darkTheme ) renderEllipseShadow(painter, frameRect, shadowColor, 4, 8, 5, 0, 1, true, 15*animation);
                else renderEllipseShadow(painter, frameRect, shadowColor, 4, 4, 6, 0, 1, true, 8*animation);
                
                painter->setBrush(palette.color( QPalette::Button ));
                painter->drawEllipse( frameRect );
                
                painter->setBrush( alphaColor( (mouseOver ? palette.color( QPalette::Highlight ).lighter(110) : palette.color( QPalette::Highlight )), qMin( animation, 1.0)) );
                painter->drawEllipse( frameRect );
                
                painter->setPen(Qt::NoPen);

                // inner ellipse
                QRectF markerRect( frameRect.adjusted( 4, 4, -4, -4 ) );
                markerRect.adjust((markerRect.width()/2)*(1-animation), 
                                (markerRect.height()/2)*(1-animation), 
                                -(markerRect.width()/2)*(1-animation), 
                                -(markerRect.height()/2)*(1-animation));
                
                
                painter->setBrush( color );
                painter->drawEllipse( markerRect );
                
            }
        }
        if ( darkTheme ) topHighlight( painter, frameRect, frameRect.width()/2 );

    }

    //______________________________________________________________________________
    void Helper::renderSliderGroove(
        QPainter* painter, const QRect& rect,
        const QColor& color ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5*Metrics::Slider_GrooveThickness );

        // content
        if( color.isValid() )
        {
            if ( color.alpha() == 255 )
            {
                QColor c = color;
                QLinearGradient gradient( rect.topLeft(), rect.bottomRight() );
                
                c.setHsv( color.hue() < 5 ? 0 : color.hue() - 5, color.saturation(), color.value() );
                gradient.setColorAt(0, c);
                
                c.setHsv(color.hue() + 5, color.saturation(), color.value() );
                gradient.setColorAt(1, c);
                
                painter->setBrush( gradient );
            } else 
                painter->setBrush( color );
            
            
            painter->setPen( Qt::NoPen );
            painter->drawRoundedRect( baseRect, radius, radius );
        }

        
    }

    //______________________________________________________________________________
    void Helper::renderDialGroove(
        QPainter* painter, const QRect& rect,
        const QColor& color,
        qreal first, qreal last ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );

        // content
        if( color.isValid() )
        {
            const qreal penWidth( Metrics::Slider_GrooveThickness - 2);
            const QRectF grooveRect( rect.adjusted( penWidth/2, penWidth/2, -penWidth/2, -penWidth/2 ) );

            // setup angles
            const int angleStart( first * 180 * 16 / M_PI );
            const int angleSpan( (last - first ) * 180 * 16 / M_PI );
            
            // setup pen
            if( angleSpan != 0 )
            {
                QPen pen( color, penWidth );
                pen.setCapStyle( Qt::RoundCap );
                painter->setPen( pen );
                painter->setBrush( Qt::NoBrush );
                painter->drawArc( grooveRect, angleStart, angleSpan );
            }
        }

        
    }

    //______________________________________________________________________________
    void Helper::renderDialContents(
        QPainter* painter, const QRect& rect,
        const QColor& color,
        qreal first, qreal second ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );

        // content
        if( color.isValid() )
        {

            // setup groove rect
            const qreal penWidth( Metrics::Slider_GrooveThickness );
            const QRectF grooveRect( rect.adjusted( penWidth/2, penWidth/2, -penWidth/2, -penWidth/2 ) );

            // setup angles
            const int angleStart( first * 180 * 16 / M_PI );
            const int angleSpan( (second - first ) * 180 * 16 / M_PI );

            // setup pen
            if( angleSpan != 0 )
            {
                QPen pen( color, penWidth );
                pen.setCapStyle( Qt::RoundCap );
                painter->setPen( pen );
                painter->setBrush( Qt::NoBrush );
                painter->drawArc( grooveRect, angleStart, angleSpan );
            }

        }

        
    }

    //______________________________________________________________________________
    void Helper::renderSliderHandle(
        QPainter* painter, const QRect& rect,
        const QColor& color,
        const bool focus,
        bool sunken ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        // copy rect
        QRectF frameRect( rect );
        frameRect.adjust( 3, 3, -3, -3 );

        // shadow
        if( !sunken )
        {

            renderEllipseShadow( painter, frameRect, focus ? QColor(0,0,0) : QColor( 0, 0, 0 ), 5, 2, 5, 0, 1, true, 20 );
            
        }

        painter->setPen( Qt::NoPen );

        // set brush
        if( color.isValid() ) painter->setBrush( color );
        else painter->setBrush( Qt::NoBrush );

        // render
        painter->drawEllipse( frameRect );
        
        topHighlight( painter, frameRect, frameRect.width()/2 );


    }

    //______________________________________________________________________________
    void Helper::renderProgressBarGroove(
        QPainter* painter, const QRect& rect,
        const QColor& color, const bool isContent ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        
        int thickness = Metrics::ProgressBar_Thickness;
        if( !isContent ) thickness = qMax(Metrics::ProgressBar_Thickness-2, 0);
        const qreal radius( 0.5*Metrics::ProgressBar_Thickness);

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRoundedRect( baseRect, radius, radius );
        }

        
    }


    //______________________________________________________________________________
    void Helper::renderProgressBarBusyContents(
        QPainter* painter, const QRect& rect,
        const QColor& first,
        const QColor& second,
        bool horizontal,
        bool reverse,
        int progress
        ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5*Metrics::ProgressBar_Thickness );

        // setup brush
        QPixmap pixmap( horizontal ? 2*Metrics::ProgressBar_BusyIndicatorSize : 1, horizontal ? 1:2*Metrics::ProgressBar_BusyIndicatorSize );
        pixmap.fill( second );
        if( horizontal )
        {

            QPainter painter( &pixmap );
            painter.setBrush( first );
            painter.setPen( Qt::NoPen );

            progress %= 2*Metrics::ProgressBar_BusyIndicatorSize;
            if( reverse ) progress = 2*Metrics::ProgressBar_BusyIndicatorSize - progress - 1;
            painter.drawRect( QRect( 0, 0, Metrics::ProgressBar_BusyIndicatorSize, 1 ).translated( progress, 0 ) );

            if( progress > Metrics::ProgressBar_BusyIndicatorSize )
            { painter.drawRect( QRect( 0, 0, Metrics::ProgressBar_BusyIndicatorSize, 1 ).translated( progress - 2*Metrics::ProgressBar_BusyIndicatorSize, 0 ) ); }

        } else {

            QPainter painter( &pixmap );
            painter.setBrush( first );
            painter.setPen( Qt::NoPen );

            progress %= 2*Metrics::ProgressBar_BusyIndicatorSize;
            progress = 2*Metrics::ProgressBar_BusyIndicatorSize - progress - 1;
            painter.drawRect( QRect( 0, 0, 1, Metrics::ProgressBar_BusyIndicatorSize ).translated( 0, progress ) );

            if( progress > Metrics::ProgressBar_BusyIndicatorSize )
            { painter.drawRect( QRect( 0, 0, 1, Metrics::ProgressBar_BusyIndicatorSize ).translated( 0, progress - 2*Metrics::ProgressBar_BusyIndicatorSize ) ); }

        }

        painter->setPen( Qt::NoPen );
        painter->setBrush( pixmap );
        painter->drawRoundedRect( baseRect, radius, radius );

    }

    //______________________________________________________________________________
    void Helper::renderScrollBarHandle(
        QPainter* painter, const QRectF& rect,
        const QColor& color ) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        const QRectF baseRect( rect );
        const qreal radius( 0.5 * std::min( {baseRect.width(), baseRect.height()}) );

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRoundedRect( baseRect, radius, radius );
        }

        
    }


    //______________________________________________________________________________
    void Helper::renderScrollBarBorder(
        QPainter* painter, const QRect& rect,
        const QColor& color ) const
    {

        // content
        if( color.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawRect( rect );
        }

    }

    //______________________________________________________________________________
    void Helper::renderTabBarTab( QPainter* painter, const QRect& rect, const QColor& color, Corners corners) const
    {

        // setup painter
        painter->setRenderHint( QPainter::Antialiasing, true );

        QRectF frameRect( rect );
        qreal radius( frameRadius( PenWidth::NoPen, -1 ) );

        painter->setPen( Qt::NoPen );

        // brush
        if( color.isValid() ) painter->setBrush( color );
        else painter->setBrush( Qt::NoBrush );

        // render
        QPainterPath path( roundedPath( frameRect, corners, radius ) );
        painter->drawPath( path );

    }

    //______________________________________________________________________________
    void Helper::renderArrow( QPainter* painter, const QRect& rect, const QColor& color, ArrowOrientation orientation ) const
    {
        // define polygon
        QPolygonF arrow;
        switch( orientation )
        {
            /* The inner points of the normal arrows are not on half pixels because
             * they need to have an even width (up/down) or height (left/right).
             * An even width/height makes them easier to align with other UI elements.
             */
            case ArrowUp: arrow = QVector<QPointF>{QPointF( -4.5, 1.5 ), QPointF( 0, -3 ), QPointF( 4.5, 1.5 )}; break;
            case ArrowDown: arrow = QVector<QPointF>{QPointF( -4.5, -1.5 ), QPointF( 0, 3 ), QPointF( 4.5, -1.5 )}; break;
            case ArrowLeft: arrow = QVector<QPointF>{QPointF( 1.5, -4.5 ), QPointF( -3, 0 ), QPointF( 1.5, 4.5 )}; break;
            case ArrowRight: arrow = QVector<QPointF>{QPointF( -1.5, -4.5 ), QPointF( 3, 0 ), QPointF( -1.5, 4.5 )}; break;
            case ArrowDown_Small: arrow = QVector<QPointF>{QPointF( 1.5, 3.5 ), QPointF( 3.5, 5.5 ), QPointF( 5.5, 3.5 )}; break;
            default: break;
        }

        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );
        painter->translate( QRectF( rect ).center() );
        painter->setBrush( Qt::NoBrush );
        QPen pen( color, PenWidth::Symbol );
        pen.setCapStyle(Qt::SquareCap);
        pen.setJoinStyle(Qt::MiterJoin);
        painter->setPen( pen );
        painter->drawPolyline( arrow );
        painter->restore();
   }

    //______________________________________________________________________________
    void Helper::renderDecorationButton( QPainter* painter, const QRect& rect, const QColor& color, ButtonType buttonType, bool inverted ) const
    {

        painter->save();
        painter->setViewport( rect );
        painter->setWindow( 0, 0, 18, 18 );
        painter->setRenderHints( QPainter::Antialiasing );

        // initialize pen
        QPen pen;
        pen.setCapStyle( Qt::RoundCap );
        pen.setJoinStyle( Qt::MiterJoin );

        if( inverted )
        {
            // render circle
            painter->setPen( Qt::NoPen );
            painter->setBrush( color );
            painter->drawEllipse( QRectF( 0, 0, 18, 18 ) );

            // take out the inner part
            painter->setCompositionMode( QPainter::CompositionMode_DestinationOut );
            painter->setBrush( Qt::NoBrush );
            pen.setColor( Qt::black );

        } else {

            painter->setBrush( Qt::NoBrush );
            pen.setColor( color );

        }

        pen.setCapStyle( Qt::RoundCap );
        pen.setJoinStyle( Qt::MiterJoin );
        pen.setWidthF( PenWidth::Symbol*qMax(1.0, 18.0/rect.width() ) );
        painter->setPen( pen );

        switch( buttonType )
        {
            case ButtonClose:
            {
                painter->drawLine( QPointF( 5, 5 ), QPointF( 13, 13 ) );
                painter->drawLine( 13, 5, 5, 13 );
                break;
            }

            case ButtonMaximize:
            {
                painter->drawPolyline( QVector<QPointF>{
                    QPointF( 4, 11 ),
                    QPointF( 9, 6 ),
                    QPointF( 14, 11 )});
                break;
            }

            case ButtonMinimize:
            {

                painter->drawPolyline(QVector<QPointF>{
                    QPointF( 4, 7 ),
                    QPointF( 9, 12 ),
                    QPointF( 14, 7 )} );
                break;
            }

            case ButtonRestore:
            {
                pen.setJoinStyle( Qt::RoundJoin );
                painter->setPen( pen );
                painter->drawPolygon( QVector<QPointF>{
                    QPointF( 4.5, 9 ),
                    QPointF( 9, 4.5 ),
                    QPointF( 13.5, 9 ),
                    QPointF( 9, 13.5 )});
                break;
            }

            default: break;
        }

        painter->restore();

    }
    
    //______________________________________________________________________________
    void Helper::renderTransparentArea( QPainter* painter, const QRect& rect ) const
    {
        painter->setCompositionMode( QPainter::CompositionMode_DestinationOut );
        painter->fillRect( rect, Qt::black );
        painter->setCompositionMode( QPainter::CompositionMode_SourceOver ); 
    }
    
    //______________________________________________________________________________
    bool Helper::isX11()
    {
        #if LIGHTLY_HAVE_X11
        static const bool s_isX11 = KWindowSystem::isPlatformX11();
        return s_isX11;
        #endif

        return false;

    }

    //______________________________________________________________________________
    bool Helper::isWayland()
    {
        static const bool s_isWayland = KWindowSystem::isPlatformWayland();
        return s_isWayland;
    }

    //______________________________________________________________________________
    QRectF Helper::strokedRect( const QRectF &rect, const int penWidth ) const
    {
        /* With a pen stroke width of 1, the rectangle should have each of its
         * sides moved inwards by half a pixel. This allows the stroke to be
         * pixel perfect instead of blurry from sitting between pixels and
         * prevents the rectangle with a stroke from becoming larger than the
         * original size of the rectangle.
         */
        qreal adjustment = 0.5 * penWidth;
        return QRectF( rect ).adjusted( adjustment, adjustment, -adjustment, -adjustment );
    }
    
    QRectF Helper::strokedRect( const QRect &rect, const int penWidth ) const
    {
        return strokedRect(QRectF(rect), penWidth);
    }

    //______________________________________________________________________________
    QPainterPath Helper::roundedPath( const QRectF& rect, Corners corners, qreal radius ) const
    {

        QPainterPath path;

        // simple cases
        if( corners == 0 )
        {

            path.addRect( rect );
            return path;

        }

        if( corners == AllCorners ) {

            path.addRoundedRect( rect, radius, radius );
            return path;

        }

        const QSizeF cornerSize( 2*radius, 2*radius );

        // rotate counterclockwise
        // top left corner
        if( corners & CornerTopLeft )
        {

            path.moveTo( rect.topLeft() + QPointF( radius, 0 ) );
            path.arcTo( QRectF( rect.topLeft(), cornerSize ), 90, 90 );

        } else path.moveTo( rect.topLeft() );

        // bottom left corner
        if( corners & CornerBottomLeft )
        {

            path.lineTo( rect.bottomLeft() - QPointF( 0, radius ) );
            path.arcTo( QRectF( rect.bottomLeft() - QPointF( 0, 2*radius ), cornerSize ), 180, 90 );

        } else path.lineTo( rect.bottomLeft() );

        // bottom right corner
        if( corners & CornerBottomRight )
        {

            path.lineTo( rect.bottomRight() - QPointF( radius, 0 ) );
            path.arcTo( QRectF( rect.bottomRight() - QPointF( 2*radius, 2*radius ), cornerSize ), 270, 90 );

        } else path.lineTo( rect.bottomRight() );

        // top right corner
        if( corners & CornerTopRight )
        {

            path.lineTo( rect.topRight() + QPointF( 0, radius ) );
            path.arcTo( QRectF( rect.topRight() - QPointF( 2*radius, 0 ), cornerSize ), 0, 90 );

        } else path.lineTo( rect.topRight() );

        path.closeSubpath();
        return path;

    }

    //________________________________________________________________________________________________________
    bool Helper::compositingActive() const
    {

        #if LIGHTLY_HAVE_X11
        if( isX11() )
        { return QX11Info::isCompositingManagerRunning( QX11Info::appScreen() ); }
        #endif

        // use KWindowSystem
        return KWindowSystem::compositingActive();

    }

    //____________________________________________________________________
    bool Helper::hasAlphaChannel( const QWidget* widget ) const
    { return compositingActive() && widget && widget->testAttribute( Qt::WA_TranslucentBackground ); }
    
    //____________________________________________________________________
    bool Helper::shouldWindowHaveAlpha( const QPalette& palette, bool isDolphin ) const
    { 
        if( _activeTitleBarColor.alphaF() < 1.0
            || ( StyleConfigData::dolphinSidebarOpacity() < 100 && isDolphin )
            || palette.color( QPalette::Window ).alpha() < 255 )
        {
            return true;
        }
        return false;
    }

    //______________________________________________________________________________________
    qreal Helper::devicePixelRatio( const QPixmap& pixmap ) const
    {
        return pixmap.devicePixelRatio();
    }

    QPixmap Helper::coloredIcon(const QIcon& icon,  const QPalette& palette, const QSize &size, QIcon::Mode mode, QIcon::State state)
    {
        const QPalette activePalette = KIconLoader::global()->customPalette();
        const bool changePalette = activePalette != palette;
        if (changePalette) {
            KIconLoader::global()->setCustomPalette(palette);
        }
        const QPixmap pixmap = icon.pixmap(size, mode, state);
        if (changePalette) {
            if (activePalette == QPalette()) {
                KIconLoader::global()->resetPalette();
            } else {
                KIconLoader::global()->setCustomPalette(activePalette);
            }
        }
        return pixmap;
    }
}
