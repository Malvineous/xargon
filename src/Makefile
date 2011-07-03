CORE = _xargon.o
CORE += config.o
CORE += gamectrl.o
CORE += graphics.o
#CORE += keyboard.o
CORE += music.o
#CORE += routines.o
CORE += shapeman.o
CORE += windows.o
CORE += x_editor.o
CORE += x_info.o
CORE += x_objman.o
CORE += x_obj.o
CORE += x_obj2.o
CORE += x_obj3.o
CORE += x_player.o
CORE += port.o

.PHONY: all

all: xrfile01

SDL_CFLAGS = `pkg-config --cflags sdl`
SDL_LIBS = `pkg-config --libs sdl`

%.o: %.c
	g++ -O0 -g -c -o $@ $< -Wno-write-strings -fpermissive $(SDL_CFLAGS)

xrfile01: $(CORE) x_vol1.o
	g++ -O0 -g -o $@ $+ $(SDL_LIBS)
