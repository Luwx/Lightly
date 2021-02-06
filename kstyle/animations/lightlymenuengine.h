#ifndef lightlymenuengine_h
#define lightlymenuengine_h

//////////////////////////////////////////////////////////////////////////////
// lightlymenuengine.h
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

#include "lightlybaseengine.h"
#include "lightlydatamap.h"
#include "lightlymenudata.h"

namespace Lightly
{

    //* stores menu hovered action and timeLine
    class MenuBaseEngine: public BaseEngine
    {
        Q_OBJECT

        public:

        //* constructor
        explicit MenuBaseEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register menubar
        virtual bool registerWidget( QWidget* ) = 0;

        //* true if widget is animated
        virtual bool isAnimated( const QObject*, WidgetIndex )
        { return false; }

        //* opacity
        virtual qreal opacity( const QObject*, WidgetIndex )
        { return -1; }

        //* return 'hover' rect position when widget is animated
        virtual QRect currentRect( const QObject*, WidgetIndex )
        { return QRect(); }

        //* return 'hover' rect position when widget is animated
        virtual QRect animatedRect( const QObject* )
        { return QRect(); }

        //* timer associated to the data
        virtual bool isTimerActive( const QObject* )
        { return false; }

        //* enable state
        void setEnabled( bool value ) override = 0;

        //* duration
        void setDuration( int ) override = 0;

        //* duration
        virtual void setFollowMouseDuration( int )
        {}

    };

    //* stores menu hovered action and timeLine
    class MenuEngineV1: public MenuBaseEngine
    {
        Q_OBJECT

        public:

        //* constructor
        explicit MenuEngineV1( QObject* parent ):
            MenuBaseEngine( parent )
        {}

        //* constructor
        MenuEngineV1( QObject* parent, MenuBaseEngine* other );

        //* register menubar
        bool registerWidget( QWidget* ) override;

        //* true if widget is animated
        bool isAnimated( const QObject* object, WidgetIndex index ) override;

        //* animation opacity
        qreal opacity( const QObject* object, WidgetIndex index ) override
        {
            if( !isAnimated( object, index ) ) return AnimationData::OpacityInvalid;
            else return _data.find(object).data()->opacity( index );
        }

        //* return 'hover' rect position when widget is animated
        QRect currentRect( const QObject* object, WidgetIndex index ) override
        {
            if( !isAnimated( object, index ) ) return QRect();
            else return _data.find(object).data()->currentRect( index );
        }

        //* enable state
        void setEnabled( bool value ) override
        {
            BaseEngine::setEnabled( value );
            _data.setEnabled( value );
        }

        //* duration
        void setDuration( int duration ) override
        {
            BaseEngine::setDuration( duration );
            _data.setDuration( duration );
        }

        //* return list of registered widgets
        WidgetList registeredWidgets( void ) const override;

        public Q_SLOTS:

        //* remove widget from map
        bool unregisterWidget( QObject* object ) override
        { return _data.unregisterWidget( object ); }

        private:

        //* data map
        DataMap<MenuDataV1> _data;

    };

}

#endif
