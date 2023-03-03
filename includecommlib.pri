DEFINES += E384COMMLIB_STATIC

INCLUDEPATH += C:\e384CommLib\include
DEPENDPATH += C:\e384CommLib\include

win32-g++:CONFIG(release, debug|release): LIBS += -LC:/e384CommLib/lib/ -le384commlib
else:win32-g++:CONFIG(debug, debug|release): LIBS += -LC:/e384CommLib/lib/ -le384commlibd
else:msvc:win32:CONFIG(release, debug|release): LIBS += -LC:/e384CommLib/lib/ -le384commlib
else:msvc:win32:CONFIG(debug, debug|release): LIBS += -LC:/e384CommLib/lib/ -le384commlibd
else:unix:CONFIG(release, debug|release): LIBS += -LC:/e384CommLib/lib/ -le384commlib
else:unix:CONFIG(debug, debug|release): LIBS += -LC:/e384CommLib/lib/ -le384commlibd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += C:/e384CommLib/lib/libe384commlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += C:/e384CommLib/lib/libe384commlibd.a
else:msvc:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += C:/e384CommLib/lib/e384commlib.lib
else:msvc:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += C:/e384CommLib/lib/e384commlibd.lib
