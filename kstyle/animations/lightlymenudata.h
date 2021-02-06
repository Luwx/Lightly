#ifndef lightlymenu_data_h
#define lightlymenu_data_h

//////////////////////////////////////////////////////////////////////////////
// lightlymenudata.h
// data container for QMenu animations
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
#include <QMenu>

namespace Lightly
{

    //* menubar data
    /*!
    most members are identical to menubar data. The one that are not are
    using templatized versions, because QMenuBar and QMenu API are very similar
    */
    class MenuDataV1: public MenuBarDataV1
    {

        Q_OBJECT

        public:

        //* constructor
        MenuDataV1( QObject* parent, QWidget* target, int duration ):
        MenuBarDataV1( parent, target, duration )
        {}

        protected:

        //* menubar enterEvent
        void enterEvent( const QObject* object ) override
        { MenuBarDataV1::enterEvent<QMenu>( object ); }

        //* menubar enterEvent
        void leaveEvent( const QObject* object ) override
        { MenuBarDataV1::leaveEvent<QMenu>( object ); }

        //* menubar mouseMoveEvent
        void mouseMoveEvent( const QObject* object ) override
        { MenuBarDataV1::mouseMoveEvent<QMenu>( object ); }

        //* menubar mousePressEvent
        void mousePressEvent( const QObject* object ) override
        { MenuBarDataV1::mousePressEvent<QMenu>( object ); }

    };

}

#endif
