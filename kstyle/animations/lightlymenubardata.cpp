//////////////////////////////////////////////////////////////////////////////
// lightlymenubardata.cpp
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

#include "lightlymenubardata.h"

#include <QTextStream>

namespace Lightly
{

    //______________________________________________
    MenuBarData::MenuBarData( QObject* parent, QWidget* target ):
        AnimationData( parent, target ),
        _isMenu( qobject_cast<QMenu*>( target ) ),
        _motions( -1 )
    {}

    //______________________________________________
    MenuBarDataV1::MenuBarDataV1( QObject* parent, QWidget* target, int duration ):
        MenuBarData( parent, target )
    {

        target->installEventFilter( this );

        // setup timeLine
        _current._animation = new Animation( duration, this );
        setupAnimation( currentAnimation(), "currentOpacity" );
        currentAnimation().data()->setDirection( Animation::Forward );
        currentAnimation().data()->setEasingCurve( QEasingCurve::OutBack );

        _previous._animation = new Animation( duration, this );
        setupAnimation( previousAnimation(), "previousOpacity" );
        previousAnimation().data()->setDirection( Animation::Backward );
        previousAnimation().data()->setEasingCurve( QEasingCurve::OutBack );

    }

    //______________________________________________
    bool MenuBarDataV1::eventFilter( QObject* object, QEvent* event )
    {

        if( !( enabled() && object == target().data() ) )
        { return AnimationData::eventFilter( object, event ); }

        // check event type
        switch( event->type() )
        {

            case QEvent::Enter:
            {
                // first need to call proper event processing
                // then implement transition
                object->event( event );
                enterEvent( object );
                if( _isMenu ) _motions = -1;
                break;
            }

            case QEvent::Leave:
            {
                // first need to call proper event processing
                // then implement transition
                object->event( event );
                leaveEvent( object );
                break;
            }

            case QEvent::MouseMove:
            {

                // first need to call proper event processing
                // then implement transition
                if( !_isMenu || _motions++ > 0  ) object->event( event );
                mouseMoveEvent( object );
                break;

            }

            case QEvent::MouseButtonPress:
            {
                // first need to call proper event processing
                // then implement transition
                mousePressEvent( object );
                break;
            }

            default: break;

        }

        // always forward event
        return AnimationData::eventFilter( object, event );

    }

}
