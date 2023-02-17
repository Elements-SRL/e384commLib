mingw:win32:CONFIG(release, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/Win32/ -lokFrontPanel
else:mingw:win32:CONFIG(debug, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/Win32/ -lokFrontPanel
msvc:win32:CONFIG(release, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/x64/ -lokFrontPanel
else:msvc:win32:CONFIG(debug, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/x64/ -lokFrontPanel

INCLUDEPATH += $$(FRONT_PANEL_INCLUDE)
DEPENDPATH += $$(FRONT_PANEL_INCLUDE)
