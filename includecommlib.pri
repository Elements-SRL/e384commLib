DEFINES += E384COMMLIB_STATIC

INCLUDEPATH += \
        $$(E384COMMLIB_PATH)include \
        $$(E384COMMLIB_PATH)include/model
DEPENDPATH += \
        $$(E384COMMLIB_PATH)include
        $$(E384COMMLIB_PATH)include/model

win32-g++:CONFIG(release, debug|release): LIBS += -L$$(E384COMMLIB_PATH)lib/release/ -le384commlib
else:win32-g++:CONFIG(debug, debug|release): LIBS += -L$$(E384COMMLIB_PATH)lib/debug/ -le384commlibd
else:msvc:win32:CONFIG(release, debug|release): LIBS += -L$$(E384COMMLIB_PATH)lib/release/ -le384commlib
else:msvc:win32:CONFIG(debug, debug|release): LIBS += -L$$(E384COMMLIB_PATH)lib/debug/ -le384commlibd
else:unix:CONFIG(release, debug|release): LIBS += -L$$(E384COMMLIB_PATH)lib/release/ -le384commlib
else:unix:CONFIG(debug, debug|release): LIBS += -L$$(E384COMMLIB_PATH)lib/debug/ -le384commlibd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$(E384COMMLIB_PATH)lib/release/libe384commlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$(E384COMMLIB_PATH)lib/debug/libe384commlibd.a
else:msvc:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$(E384COMMLIB_PATH)lib/release/e384commlib.lib
else:msvc:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$(E384COMMLIB_PATH)lib/debug/e384commlibd.lib

include($$(CY_API_PATH)includecyapi.pri)
include($$(FRONT_PANEL_PATH)includefrontpanel.pri)
include($$(FTDI_UTILS_PATH)includeftdiutils.pri)
