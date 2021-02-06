#ifndef lightlymenubardata_imp_h
#define lightlymenubardata_imp_h

//////////////////////////////////////////////////////////////////////////////
// lightlymenubardata_imp.h
// implements menubar data templatized methods
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

namespace Lightly
{

    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::enterEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // if the current action is still active, one does nothing
        if( local->activeAction() == currentAction().data() ) return;

        if( currentAnimation().data()->isRunning() ) currentAnimation().data()->stop();
        clearCurrentAction();
        clearCurrentRect();

    }

    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::leaveEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // if the current action is still active, one does nothing
        if( local->activeAction() == currentAction().data() ) return;

        if( currentAnimation().data()->isRunning() ) currentAnimation().data()->stop();
        if( previousAnimation().data()->isRunning() ) previousAnimation().data()->stop();
        if( currentAction() )
        {
            setPreviousRect( currentRect() );
            clearCurrentAction();
            clearCurrentRect();
            previousAnimation().data()->start();
        }

        // trigger update
        setDirty();

    }

    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::mouseMoveEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // check action
        if( local->activeAction() == currentAction().data() ) return;

        bool hasCurrentAction( currentAction() );

        // check current action
        if( currentAction() )
        {
            if( currentAnimation().data()->isRunning() ) currentAnimation().data()->stop();
            if( previousAnimation().data()->isRunning() ) {
                previousAnimation().data()->setCurrentTime(0);
                previousAnimation().data()->stop();
            }

            // only start fadeout effect if there is no new selected action
            //if( !activeActionValid )
            if( !local->activeAction() )
            {
                setPreviousRect( currentRect() );
                previousAnimation().data()->start();
            }

            clearCurrentAction();
            clearCurrentRect();

        }

        // check if local current actions is valid
        bool activeActionValid( local->activeAction() && local->activeAction()->isEnabled() && !local->activeAction()->isSeparator() );
        if( activeActionValid )
        {
            if( currentAnimation().data()->isRunning() ) currentAnimation().data()->stop();

            setCurrentAction( local->activeAction() );
            setCurrentRect( local->actionGeometry( currentAction().data() ) );
            if( !hasCurrentAction )
            { currentAnimation().data()->start(); }

        }

    }

    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::mousePressEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // check action
        if( local->activeAction() == currentAction().data() ) return;

        // check current action
        bool activeActionValid( local->activeAction() && local->activeAction()->isEnabled() && !local->activeAction()->isSeparator() );
        if( currentAction() && !activeActionValid )
        {

            if( currentAnimation().data()->isRunning() ) currentAnimation().data()->stop();
            if( previousAnimation().data()->isRunning() ) previousAnimation().data()->stop();

            setPreviousRect( currentRect() );
            previousAnimation().data()->start();

            clearCurrentAction();
            clearCurrentRect();

        }

    }

}

#endif
