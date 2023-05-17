mingw:win32:CONFIG(release, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/Win32/ -lokFrontPanel
else:mingw:win32:CONFIG(debug, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/Win32/ -lokFrontPanel
msvc:win32:CONFIG(release, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/x64/ -lokFrontPanel
else:msvc:win32:CONFIG(debug, debug|release): LIBS += -L$$(FRONT_PANEL_LIB)/x64/ -lokFrontPanel
else:linux:CONFIG(release, debug|release): : LIBS += -L$$(FRONT_PANEL_LIB)/ -lokFrontPanel
else:linux:CONFIG(debug, debug|release): : LIBS += -L$$(FRONT_PANEL_LIB)/ -lokFrontPanel

INCLUDEPATH += $$quote($$(FRONT_PANEL_INCLUDE))
DEPENDPATH += $$quote($$(FRONT_PANEL_INCLUDE))
