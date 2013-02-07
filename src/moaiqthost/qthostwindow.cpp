/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "qthostwindow.h"

#include "aku/AKU.h"

#ifdef QT_HOST_USE_DEBUGGER
    #include "aku/AKU-debugger.h"
#endif

#ifdef QT_HOST_USE_FMOD_EX
    #include <aku/AKU-fmod-ex.h>
#endif

#ifdef QT_HOST_FPS_DEBUG_PRINT
    #include <QDebug>
    #include <QTime>
    int framesDrawn = 0;
    QTime* fpsClock = new QTime();
#endif

#include <assert.h>
#include <QMouseEvent>
#include <QTimer>

namespace MoaiQtHost
{

namespace InputDeviceID
{
    enum
    {
        DEVICE,
        TOTAL
    };
}

namespace InputDeviceSensorID
{
    enum
    {
        KEYBOARD,
        POINTER,
        MOUSE_LEFT,
        MOUSE_MIDDLE,
        MOUSE_RIGHT,
        MOUSE_WHEEL,
        TOTAL
    };
}

// Buttons not belonging to Latin-1 encoding that we support.
// Range 128-159 is used since Latin-1 has no characters on that range.
namespace ExtraKeys
{
    enum
    {
        ARROW_LEFT = 128,
        ARROW_UP,
        ARROW_RIGHT,
        ARROW_DOWN
    };
}

// Empty namespace for callbacks
namespace
{

void enterFullscreenAKUCB()
{
    QtHostWindow* hostInstance = QtHostWindow::getHostInstance ();
    if ( hostInstance != NULL )
    {
        hostInstance->showFullScreen ();
    }
}

void exitFullscreenAKUCB ()
{
    QtHostWindow* hostInstance = QtHostWindow::getHostInstance ();
    if ( hostInstance != NULL )
    {
        hostInstance->showNormal ();
    }
}

void openWindowAKUCB ( const char* title, int width, int height )
{
    QtHostWindow* hostInstance = QtHostWindow::getHostInstance ();
    if ( hostInstance != NULL )
    {
        hostInstance->openWindow ( title, width, height );
    }
}

void setSimStepAKUCB ( double newStep )
{
    QtHostWindow* hostInstance = QtHostWindow::getHostInstance ();
    if ( hostInstance != NULL )
    {
        hostInstance->setSimStep ( newStep );
    }
}

#if QT_HOST_USE_DEBUGGER
    void AKUErrorTracebackFunc ( const char* message, lua_State* L, int level )
    {
        AKUDebugHarnessHandleError ( message, L, level );
    }
#endif

} // Empty namespace

QtHostWindow* QtHostWindow::hostInstance_ = 0;

QtHostWindow::QtHostWindow ( QWidget* parent ) :
    QGLWidget           ( parent ),
    windowInitialized_  ( false ),
    simStepTimer_       ( 0 )
{
    setMouseTracking ( true );

    // This could be used to reserve initial memory for the host.
    // Size in bytes.
    //AKUInitMemPool ( 100 * 1024 * 1024 );
}

QtHostWindow::~QtHostWindow ()
{
    AKUClearMemPool ();
    AKUFinalize ();

    hostInstance_ = 0;
    simStepTimer_ = 0;
}

QtHostWindow* QtHostWindow::getHostInstance ()
{
    if ( hostInstance_ == NULL )
    {
        hostInstance_ = new QtHostWindow ();
    }
    return hostInstance_;
}

void QtHostWindow::setCallbacks ()
{
    AKUSetFunc_EnterFullscreenMode ( enterFullscreenAKUCB );
    AKUSetFunc_ExitFullscreenMode ( exitFullscreenAKUCB );
    AKUSetFunc_OpenWindow ( openWindowAKUCB );
    AKUSetFunc_SetSimStep ( setSimStepAKUCB );

    #ifdef QT_HOST_USE_DEBUGGER
        AKUSetFunc_ErrorTraceback ( AKUErrorTracebackFunc );
        AKUDebugHarnessInit ();
    #endif
}

void QtHostWindow::setDevices ()
{
    AKUSetInputConfigurationName    ( "AKUQtHost" );

    AKUReserveInputDevices			( InputDeviceID::TOTAL );
    AKUSetInputDevice               ( InputDeviceID::DEVICE, "device" );

    AKUReserveInputDeviceSensors	( InputDeviceID::DEVICE, InputDeviceSensorID::TOTAL );
    AKUSetInputDeviceKeyboard		( InputDeviceID::DEVICE, InputDeviceSensorID::KEYBOARD,		"keyboard" );
    AKUSetInputDevicePointer		( InputDeviceID::DEVICE, InputDeviceSensorID::POINTER,		"pointer" );
    AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputDeviceSensorID::MOUSE_LEFT,	"mouseLeft" );
    AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputDeviceSensorID::MOUSE_MIDDLE,	"mouseMiddle" );
    AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputDeviceSensorID::MOUSE_RIGHT,	"mouseRight" );
    AKUSetInputDeviceWheel			( InputDeviceID::DEVICE, InputDeviceSensorID::MOUSE_WHEEL,  "mouseWheel" );
}

void QtHostWindow::startAkuTimers ()
{
    simStepTimer_ = new QTimer ( this );
    connect( simStepTimer_, SIGNAL ( timeout ()),
             this,          SLOT ( simStep ()));
    connect( simStepTimer_, SIGNAL ( timeout ()),
             this,          SLOT ( updateGL ()));
    simStepTimer_->start ( AKUGetSimStep () * 1000 );
}

void QtHostWindow::openWindow ( const char* title, int width, int height )
{
    show ();
    glInit ();

    AKUDetectGfxContext ();
    windowInitialized_ = true;

    setWindowTitle ( QString ( title ));
    resize ( width, height );
}

void QtHostWindow::setSimStep ( double newStep )
{
    if ( simStepTimer_ )
    {
        simStepTimer_->setInterval ( newStep * 1000 );
    }
}

void QtHostWindow::resizeEvent ( QResizeEvent* )
{
    if ( !windowInitialized_ )
    {
        return;
    }

    AKUSetScreenSize    ( width (), height ());
    AKUSetViewSize      ( width (), height ());
}

void QtHostWindow::keyPressEvent ( QKeyEvent* event )
{
    // Only accept the initial key press.
    if( !event->isAutoRepeat ())
    {
        onKey ( event, true );
    }
}

void QtHostWindow::keyReleaseEvent ( QKeyEvent* event )
{
    // Only accept the initial key press.
    if( !event->isAutoRepeat ())
    {
        onKey ( event, false );
    }
}

void QtHostWindow::mousePressEvent ( QMouseEvent* event )
{
    onMouseButton ( event->button (), true);
}

void QtHostWindow::mouseReleaseEvent ( QMouseEvent* event )
{
    onMouseButton ( event->button (), false);
}

void QtHostWindow::mouseMoveEvent ( QMouseEvent* event )
{
    AKUEnqueuePointerEvent ( InputDeviceID::DEVICE,
                             InputDeviceSensorID::POINTER,
                             event->x (),
                             event->y ());
}

void QtHostWindow::wheelEvent(QWheelEvent* event)
{
    // The delta is given in eights of a degree (same as Qt), so on most wheels, one click is
    // +120 when rolling away and -120 when rolling towards you (1 click is 15 degrees).
    AKUEnqueueWheelEvent ( InputDeviceID::DEVICE,
                           InputDeviceSensorID::MOUSE_WHEEL,
                           static_cast < float >( event->delta ()));
}

void QtHostWindow::paintGL ()
{
    // AKURender will crash the application,
    // if it tries to draw something before
    // AKUDetectGfxContext has been called.
    if (windowInitialized_)
    {
        AKURender ();

        #ifdef QT_HOST_FPS_DEBUG_PRINT
            ++framesDrawn;
            if ( fpsClock->elapsed() > 1000 )
            {
                qDebug() << "FPS: "
                         << framesDrawn * 1000.0/fpsClock->restart ();
                framesDrawn = 0;
            }
        #endif
    }
}

void QtHostWindow::simStep ()
{
    AKUUpdate ();

    #ifdef QT_HOST_USE_FMOD_EX
        AKUFmodExUpdate ();
    #endif

    #ifdef QT_HOST_USE_DEBUGGER
        AKUDebugHarnessUpdate ();
    #endif
}

void QtHostWindow::onMouseButton ( Qt::MouseButton button, bool pressedDown )
{
    if ( button == Qt::LeftButton )
    {
        AKUEnqueueButtonEvent ( InputDeviceID::DEVICE,
                                InputDeviceSensorID::MOUSE_LEFT,
                                pressedDown );
    }
    else if ( button == Qt::MiddleButton )
    {
        AKUEnqueueButtonEvent ( InputDeviceID::DEVICE,
                                InputDeviceSensorID::MOUSE_MIDDLE,
                                pressedDown);
    }
    else if ( button == Qt::RightButton )
    {
        AKUEnqueueButtonEvent ( InputDeviceID::DEVICE,
                                InputDeviceSensorID::MOUSE_RIGHT,
                                pressedDown);
    }
}

void QtHostWindow::onKey ( QKeyEvent* event, bool pressedDown )
{
    int eventKey = event->key ();
    if ( eventKey == Qt::Key_Control )
    {
        AKUEnqueueKeyboardControlEvent ( InputDeviceID::DEVICE,
                                         InputDeviceSensorID::KEYBOARD,
                                         pressedDown );
    }
    else if ( eventKey == Qt::Key_Alt || eventKey == Qt::Key_AltGr )
    {
        AKUEnqueueKeyboardAltEvent ( InputDeviceID::DEVICE,
                                     InputDeviceSensorID::KEYBOARD,
                                     pressedDown );
    }
    else if ( eventKey == Qt::Key_Shift )
    {
        AKUEnqueueKeyboardShiftEvent ( InputDeviceID::DEVICE,
                                       InputDeviceSensorID::KEYBOARD,
                                       pressedDown );
    }
    // Check if we can convert the key to Latin-1.
    else if ( eventKey != Qt::NoModifier && !event->text().isEmpty() )
    {
        int latin1Code = static_cast < int >( static_cast < unsigned char >(
                            event->text ().toLatin1 ().at ( 0 )));

        if ( latin1Code > 0 && latin1Code < MAX_KEYS )
        {
            sendAkuKeyboardEvent ( latin1Code, pressedDown );
        }
    }
    // Check other possible supported buttons.
    else
    {
        switch (eventKey)
        {
            case Qt::Key_Left:
                sendAkuKeyboardEvent ( ExtraKeys::ARROW_LEFT, pressedDown );
                break;
            case Qt::Key_Up:
                sendAkuKeyboardEvent ( ExtraKeys::ARROW_UP, pressedDown );
                break;
            case Qt::Key_Right:
                sendAkuKeyboardEvent ( ExtraKeys::ARROW_RIGHT, pressedDown );
                break;
            case Qt::Key_Down:
                sendAkuKeyboardEvent ( ExtraKeys::ARROW_DOWN, pressedDown );
                break;
            default:
                break;
        }
    }
}

void QtHostWindow::sendAkuKeyboardEvent ( int keyCode, bool pressedDown )
{
    assert ( keyCode > 0 && keyCode < MAX_KEYS );
    AKUEnqueueKeyboardEvent ( InputDeviceID::DEVICE,
                              InputDeviceSensorID::KEYBOARD,
                              keyCode,
                              pressedDown );
}

} // MoaiQtHost namespace end
