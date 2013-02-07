/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef QTHOSTWINDOW_H
#define QTHOSTWINDOW_H

#include <QGLWidget>

class QTimer;

namespace MoaiQtHost
{

class QtHostWindow : public QGLWidget
{
    Q_OBJECT
public:
    virtual ~QtHostWindow ();

    static QtHostWindow* getHostInstance ();

    // Set necessary callback functions for AKU-interface.
    void setCallbacks ();
    // Tell Moai SDK, which input devices and sensors we're using.
    void setDevices ();
    // Start update and render timers.
    void startAkuTimers ();

    void openWindow ( const char* title, int width, int height );
    void setSimStep ( double newStep );
    
protected:
    virtual void resizeEvent ( QResizeEvent* );

    virtual void keyPressEvent ( QKeyEvent* event );
    virtual void keyReleaseEvent ( QKeyEvent* event );
    virtual void mousePressEvent ( QMouseEvent* event );
    virtual void mouseReleaseEvent ( QMouseEvent* event );
    virtual void mouseMoveEvent ( QMouseEvent* event );
    virtual void wheelEvent ( QWheelEvent* event );

    virtual void paintGL ();

private slots:
    void simStep();

private:
    explicit QtHostWindow ( QWidget* parent = 0 );

    void onMouseButton ( Qt::MouseButton button, bool pressedDown );
    void onKey ( QKeyEvent* event, bool pressedDown );
    void sendAkuKeyboardEvent ( int keyCode, bool pressedDown );

    // Moai SDK can only accept keys with ID smaller than this.
    // Defined by MOAIKeyCodes in MOAIKeyboardSensor.h
    static const unsigned int MAX_KEYS = 256;

    static QtHostWindow* hostInstance_;

    // True, when it's safe to render.
    bool    windowInitialized_;
    // Timer for update functions.
    // Kept here because the interval may change.
    QTimer* simStepTimer_;
};

} // MoaiQtHost namespace

#endif // QTHOSTWINDOW_H
