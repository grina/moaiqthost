/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "qthostscriptldr.h"

#include "aku/AKU.h"
#include "lua-headers/moai_lua.h"

#include <cstring> // For strcmp

namespace MoaiQtHost
{

QtHostScriptLdr::QtHostScriptLdr()
{
}

void QtHostScriptLdr::loadScripts( int argc, char *argv[] )
{
    AKURunBytecode ( moai_lua, moai_lua_SIZE );

    for ( int i = 1; i < argc; ++i )
    {
        char* arg = argv [ i ];
        // On Glut-host the -e flag is used for reloading Lua-files
        // when they're modified. This is not implemented on QtHost,
        // so we just ignore the flag.
        if ( strcmp( arg, "-e" ) == 0 )
        {
            //sDynamicallyReevaluateLuaFiles = true;
        }
        else if ( strcmp( arg, "-s" ) == 0 && ++i < argc )
        {
            char* script = argv [ i ];
            AKURunString ( script );
        }
        else
        {
            AKURunScript ( arg );
        }
    }
}

} // MoaiQtHost namespace
