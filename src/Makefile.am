bin_PROGRAMS = xrfile01

CORE_FILES = _xargon.cpp
CORE_FILES += config.cpp
CORE_FILES += gamectrl.cpp
CORE_FILES += graphics.cpp
#CORE_FILES += keyboard.cpp
CORE_FILES += music.cpp
#CORE_FILES += routines.cpp
CORE_FILES += shapeman.cpp
CORE_FILES += windows.cpp
CORE_FILES += x_editor.cpp
CORE_FILES += x_info.cpp
CORE_FILES += x_objman.cpp
CORE_FILES += x_obj.cpp
CORE_FILES += x_obj2.cpp
CORE_FILES += x_obj3.cpp
CORE_FILES += x_player.cpp
CORE_FILES += port.cpp

xrfile01_SOURCES = $(CORE_FILES) x_vol1.cpp

AM_CPPFLAGS = $(sdl_CFLAGS) -Wno-write-strings -fpermissive
AM_LDFLAGS = $(sdl_LIBS)
