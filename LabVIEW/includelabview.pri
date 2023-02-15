INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

msvc:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -LC:/Windows/System32 -llabview -luser32
else:msvc:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -LC:/Windows/System32 -llabview -luser32

msvc:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/labview.lib
else:msvc:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/labview.lib
