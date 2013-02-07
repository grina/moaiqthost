# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

QT       += core opengl

TARGET = moaiqt
TEMPLATE = app

# MODIFY THIS TO INCLUDE DIFFERENT FEATURES
CONFIG += #untz, particle_presets, luaext, audiosampler, fmod_ex, fmod_designer, debugger


win32 {
    LIBS += -L$$PWD/libs -lmoaicore -ladvapi32 -lcomctl32 -loleaut32 -lrpcrt4 -lwinmm -lwldap32 -lws2_32 -lwsock32 -liphlpapi -lpsapi -lshfolder
}

unix:!macx {
    LIBS += -L$$PWD/libs -laku -lmoaicore -lutf8 -ljansson -lzlcore -luslscore -lwhirlpool -ltlsf -lssl -ltinyxml -lBox2D -lcurl -lchipmunk -llua5.1 -ljpeg -lGLEW -lfreetype -lpng
}

macx {
    LIBS += -L$$PWD/libs -lssl -lcurl -lcrypto -lmoai-osx-luaext -lmoai-osx-zlcore -lmoai-osx-3rdparty -lmoai-osx
}

CONFIG(debug, debug|release) {
    DEFINES += QT_HOST_FPS_DEBUG_PRINT
    CONFIG += console
}

audiosampler {
    DEFINES += QT_HOST_USE_AUDIOSAMPLER
}

debugger {
    DEFINES += QT_HOST_USE_DEBUGGER
    LIBS += -L$$PWD/libs -lmoaiext-debugger
}

fmod_designer {
    DEFINES += QT_HOST_USE_FMOD_DESIGNER
    LIBS += -L$$PWD/libs -lmoaiext-fmod-designer -lfmodex_vc
}

fmod_ex {
    DEFINES += QT_HOST_USE_FMOD_EX

    win32 {
        LIBS += -L$$PWD/libs -lmoaiext-fmod-ex -lfmodex_vc
    }
    unix:!macx {
        LIBS += -L$$PWD/libs -lmoaiext-fmod-ex -lfmodex_vc
    }
    macx {
        LIBS += -lfmodex -lmoai-osx-fmod-ex -framework AudioUnit -framework AudioToolbox
    }
}

luaext {
    DEFINES += QT_HOST_USE_LUAEXT

    win32 {
        LIBS += -L$$PWD/libs -lmoaiext-luaext
    }
    unix:!macx {
        LIBS += -L$$PWD/libs -lmoaiext-luaext
    }
    macx {
        #Mac needs luaext always
    }
}

particle_presets {
    DEFINES += QT_HOST_USE_PARTICLE_PRESETS

    SOURCES += ParticlePresets.cpp

    HEADERS  += ParticlePresets.h
}

untz {
    DEFINES += QT_HOST_USE_UNTZ

    win32 {
        LIBS += -L$$PWD/libs -lmoaiext-untz -ldsound -lstrmiids -lole32
    }
    unix:!macx {
        LIBS += -L$$PWD/libs -lmoaiext-untz -luntz -lpulse -lpulse-simple -lvorbisfile -lpthread -lasound
    }
    macx {
        LIBS += -lmoai-osx-untz -framework AudioUnit -framework AudioToolbox -framework CoreFoundation
    }
}


SOURCES += main.cpp\
    qthostwindow.cpp \
    qthostscriptldr.cpp \

HEADERS  += \
    qthostwindow.h \
    qthostscriptldr.h

INCLUDEPATH += ../moaipackage/src/ \
    ./
