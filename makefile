CC = gcc
TARGET = textAdventure
SRC = $(wildcard src/*.c)
HDR = $(wildcard src/*.h)

CFLAGS += -Isrc
DEBUG_CFLAGS = -g -O0

LUA_CFLAGS = $(shell pkg-config --cflags lua5.4)
LUA_LIBS   = $(shell pkg-config --libs lua5.4)

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(LUA_CFLAGS) $(SRC) -o $@ $(LUA_LIBS)

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(TARGET)

map.png: map.gv
	dot -Tpng -o $@ $<
map.gv: map.awk object.txt
	awk -f map.awk object.txt > $@
success.txt: $(TARGET) baseline.txt
	./$(TARGET) testscript.txt > transcript.txt
	cmp transcript.txt baseline.txt
	mv -f transcript.txt success.txt

clean:
	$(RM) $(TARGET) $(TARGET).exe map.gv map.png transcript.txt success.txt

.PHONY: all debug clean
