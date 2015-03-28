bin_PROGRAMS = xrfile01 xrfile02 xrfile03

CORE_FILES = _xargon.cpp
CORE_FILES += config.cpp
CORE_FILES += gamectrl.cpp
CORE_FILES += graphics.cpp
CORE_FILES += music.cpp
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
CORE_FILES += dbopl.cpp

EXTRA_CORE_FILES = port.h
EXTRA_CORE_FILES += dbopl.h
EXTRA_CORE_FILES += dosbox.h
EXTRA_CORE_FILES += adlib.h
EXTRA_CORE_FILES += include/cfg_win.h
EXTRA_CORE_FILES += include/config.h
EXTRA_CORE_FILES += include/copyfile.h
EXTRA_CORE_FILES += include/gamectrl.h
EXTRA_CORE_FILES += include/gr.h
EXTRA_CORE_FILES += include/keyboard.h
EXTRA_CORE_FILES += include/music.h
EXTRA_CORE_FILES += include/pixwrite.h
EXTRA_CORE_FILES += include/uncrunch.h
EXTRA_CORE_FILES += include/windows.h
EXTRA_CORE_FILES += include/worx.h
EXTRA_CORE_FILES += include/x_obj.def
EXTRA_CORE_FILES += include/x_obj.h
EXTRA_CORE_FILES += include/x_snd.h
EXTRA_CORE_FILES += include/xargon.h

xrfile01_SOURCES = $(CORE_FILES) x_vol1.cpp
xrfile02_SOURCES = $(CORE_FILES) x_vol2.cpp
xrfile03_SOURCES = $(CORE_FILES) x_vol3.cpp

EXTRA_xrfile01_SOURCES = $(EXTRA_CORE_FILES)
EXTRA_xrfile02_SOURCES = $(EXTRA_CORE_FILES)
EXTRA_xrfile03_SOURCES = $(EXTRA_CORE_FILES)

AM_CPPFLAGS = $(sdl_CFLAGS) -Wno-write-strings -fpermissive $(libgamemusic_CFLAGS)
AM_LDFLAGS = $(sdl_LIBS) $(libgamemusic_LIBS)

if USE_MUSIC
AM_CPPFLAGS += -DUSE_MUSIC
endif
