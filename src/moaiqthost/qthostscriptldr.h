/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef QTHOSTSCRIPTLDR_H
#define QTHOSTSCRIPTLDR_H

namespace MoaiQtHost
{

class QtHostScriptLdr
{
public:
    QtHostScriptLdr();

    void loadScripts( int argc, char *argv[] );
};

} // MoaiQtHost namespace

#endif // QTHOSTSCRIPTLDR_H
