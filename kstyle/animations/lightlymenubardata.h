#ifndef lightlymenubar_data_h
#define lightlymenubar_data_h

//////////////////////////////////////////////////////////////////////////////
// lightlymenubardata.h
// data container for QMenuBar animations
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "lightlyanimationdata.h"
#include "lightly.h"

#include <QMenuBar>
#include <QBasicTimer>

namespace Lightly
{

    //* widget index
    enum WidgetIndex
    {
        Current,
        Previous
    };

    //* menubar data
    class MenuBarData: public AnimationData
    {

        Q_OBJECT

        public:

        //* constructor
        MenuBarData( QObject* parent, QWidget* target );

        protected:

        bool _isMenu = false;
        int _motions = -1;

    };

    //* menubar data
    class MenuBarDataV1: public MenuBarData
    {

        Q_OBJECT

        //* declare opacity property
        Q_PROPERTY( qreal currentOpacity READ currentOpacity WRITE setCurrentOpacity )
        Q_PROPERTY( qreal previousOpacity READ previousOpacity WRITE setPreviousOpacity )

        public:

        //* constructor
        MenuBarDataV1( QObject* parent, QWidget* target, int duration );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        //* animations
        const Animation::Pointer& currentAnimation( void ) const
        { return _current._animation; }

        //* animations
        const Animation::Pointer& previousAnimation( void ) const
        { return _previous._animation; }

        //* return animation matching given point
        Animation::Pointer animation( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentAnimation();
            else if( previousRect().contains( point ) ) return previousAnimation();
            else return Animation::Pointer();
        }

        //* return animation matching given point
        qreal opacity( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentOpacity();
            else if( previousRect().contains( point ) ) return previousOpacity();
            else return OpacityInvalid;
        }

        // return rect matching QPoint
        QRect currentRect( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentRect();
            else if( previousRect().contains( point ) ) return previousRect();
            else return QRect();
        }

        //* animation associated to given Widget index
        const Animation::Pointer& animation( WidgetIndex index ) const
        { return index == Current ? currentAnimation():previousAnimation(); }

        //* opacity associated to given Widget index
        qreal opacity( WidgetIndex index ) const
        { return index == Current ? currentOpacity():previousOpacity(); }

        //* opacity associated to given Widget index
        const QRect& currentRect( WidgetIndex index ) const
        { return index == Current ? currentRect():previousRect(); }

        //* duration
        void setDuration( int duration ) override
        {
            currentAnimation().data()->setDuration( duration );
            previousAnimation().data()->setDuration( duration );
        }

        //* current opacity
        qreal currentOpacity( void ) const
        { return _current._opacity; }

        //* current opacity
        void setCurrentOpacity( qreal value )
        {
            value = digitize( value );
            if( _current._opacity == value ) return;
            _current._opacity = value;
            setDirty();
        }

        //* current rect
        const QRect& currentRect( void ) const
        { return _current._rect; }

        //* previous opacity
        qreal previousOpacity( void ) const
        { return _previous._opacity; }

        //* previous opacity
        void setPreviousOpacity( qreal value )
        {
            value = digitize( value );
            if( _previous._opacity == value ) return;
            _previous._opacity = value;
            setDirty();
        }

        //* previous rect
        const QRect& previousRect( void ) const
        { return _previous._rect; }

        protected:

        //*@name current action handling
        //@{

        //* guarded action pointer
        using ActionPointer = WeakPointer<QAction>;

        //* current action
        virtual const ActionPointer& currentAction( void ) const
        { return _currentAction; }

        //* current action
        virtual void setCurrentAction( QAction* action )
        { _currentAction = ActionPointer( action ); }

        //* current action
        virtual void clearCurrentAction( void )
        { _currentAction = ActionPointer(); }

        //@}

        //*@name rect handling
        //@{

        //* current rect
        virtual void setCurrentRect( const QRect& rect )
        { _current._rect = rect; }

        //* current rect
        virtual void clearCurrentRect( void )
        { _current._rect = QRect(); }

        //* previous rect
        virtual void setPreviousRect( const QRect& rect )
        { _previous._rect = rect; }

        //* previous rect
        virtual void clearPreviousRect( void )
        { _previous._rect = QRect(); }

        //@}

        // leave event
        template< typename T > inline void enterEvent( const QObject* object );

        // leave event
        template< typename T > inline void leaveEvent( const QObject* object );

        //* mouse move event
        template< typename T > inline void mouseMoveEvent( const QObject* object );

        //* mouse move event
        template< typename T > inline void mousePressEvent( const QObject* object );

        //* menubar enterEvent
        virtual void enterEvent( const QObject* object )
        { enterEvent<QMenuBar>( object ); }

        //* menubar enterEvent
        virtual void leaveEvent( const QObject* object )
        { leaveEvent<QMenuBar>( object ); }

        //* menubar mouseMoveEvent
        virtual void mouseMoveEvent( const QObject* object )
        { mouseMoveEvent<QMenuBar>( object ); }

        //* menubar mousePressEvent
        virtual void mousePressEvent( const QObject* object )
        { mousePressEvent<QMenuBar>( object ); }

        private:

        //* container for needed animation data
        class Data
        {
            public:

            //* default constructor
            Data( void ):
                _opacity(0)
                {}

            Animation::Pointer _animation;
            qreal _opacity;
            QRect _rect;
        };

        //* current tab animation data (for hover enter animations)
        Data _current;

        //* previous tab animations data (for hover leave animations)
        Data _previous;

        //* current action
        ActionPointer _currentAction;

    };

}

#include "lightlymenubardata_imp.h"
#endif
