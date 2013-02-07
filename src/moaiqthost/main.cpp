/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QtGui/QApplication>

#include "qthostwindow.h"
#include "qthostscriptldr.h"

#include "aku/AKU.h"

#ifdef QT_HOST_USE_LUAEXT
    #include <aku/AKU-luaext.h>
#endif

#ifdef QT_HOST_USE_UNTZ
    #include <aku/AKU-untz.h>
#endif

#ifdef QT_HOST_USE_FMOD_EX
    #include <aku/AKU-fmod-ex.h>
#endif

#ifdef QT_HOST_USE_FMOD_DESIGNER
    #include <aku/AKU-fmod-designer.h>
#endif

#ifdef QT_HOST_USE_AUDIOSAMPLER
    #include <aku/AKU-audiosampler.h>
#endif

#ifdef QT_HOST_USE_PARTICLE_PRESETS
    #include <ParticlePresets.h>
#endif

int main ( int argc, char *argv [])
{
    QApplication a ( argc, argv );

    MoaiQtHost::QtHostWindow* window =
        MoaiQtHost::QtHostWindow::getHostInstance ();
    MoaiQtHost::QtHostScriptLdr scriptLoader;

    AKUContextID context = AKUGetContext ();
    // Delete old context
    if ( context != 0 )
    {
        AKUDeleteContext ( context );
    }
    AKUCreateContext ();

    #ifdef QT_HOST_USE_LUAEXT
        AKUExtLoadLuacrypto ();
        AKUExtLoadLuacurl ();
        AKUExtLoadLuafilesystem ();
        AKUExtLoadLuasocket ();
        AKUExtLoadLuasql ();
    #endif

    #ifdef QT_HOST_USE_UNTZ
        AKUUntzInit ();
    #endif

    #ifdef QT_HOST_USE_FMOD_EX
        AKUFmodExInit ();
    #endif

    #ifdef QT_HOST_USE_FMOD_DESIGNER
        AKUFmodDesignerInit ();
    #endif

    #ifdef QT_HOST_USE_AUDIOSAMPLER
        AKUAudioSamplerInit ();
    #endif

    #ifdef QT_HOST_USE_PARTICLE_PRESETS
        ParticlePresets ();
    #endif

    window->setDevices ();
    window->setCallbacks ();

    scriptLoader.loadScripts ( argc, argv );
    window->startAkuTimers ();

    int execReturn = a.exec ();
    delete window;
    return execReturn;
}
