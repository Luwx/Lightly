//////////////////////////////////////////////////////////////////////////////
// lightlymenuengine.cpp
// stores event filters and maps widgets to timelines for animations
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

#include "lightlymenuengine.h"

namespace Lightly
{

    //____________________________________________________________
    MenuEngineV1::MenuEngineV1( QObject* parent, MenuBaseEngine* other ):
        MenuBaseEngine( parent )
    {
        if( other )
        {
            foreach( QWidget* widget,  other->registeredWidgets() )
            { registerWidget( widget ); }
        }
    }

    //____________________________________________________________
    bool MenuEngineV1::registerWidget( QWidget* widget )
    {

        if( !widget ) return false;

        // create new data class
        if( !_data.contains( widget ) ) _data.insert( widget, new MenuDataV1( this, widget, duration() ), enabled() );

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );
        return true;
    }

    //____________________________________________________________
    bool MenuEngineV1::isAnimated( const QObject* object, WidgetIndex index )
    {
        DataMap<MenuDataV1>::Value data( _data.find( object ) );
        if( !data )
        {
            return false;
        }

        if( Animation::Pointer animation = data.data()->animation( index ) ) {

            return animation.data()->isRunning();

        } else return false;
    }

    //____________________________________________________________
    BaseEngine::WidgetList MenuEngineV1::registeredWidgets( void ) const
    {

        WidgetList out;

        // the typedef is needed to make Krazy happy
        typedef DataMap<MenuDataV1>::Value Value;
        foreach( const Value& value, _data )
        { if( value ) out.insert( value.data()->target().data() ); }

        return out;

    }

}
