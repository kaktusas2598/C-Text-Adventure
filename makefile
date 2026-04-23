C = *.c
H = *.h

LUA_CFLAGS = $(shell pkg-config --cflags lua5.4)
LUA_LIBS   = $(shell pkg-config --libs lua5.4)

all: $(C) $(H)
	gcc *.c -o textAdventure $(LUA_CFLAGS) $(LUA_LIBS)

debug: $(C) $(H)
	gcc -g -O0 *.c -o textAdventure $(LUA_CFLAGS) $(LUA_LIBS)

map.png: map.gv
	dot -Tpng -o $@ $<
map.gv: map.awk object.txt
	awk -f map.awk object.txt > $@
success.txt: textAdventure transcript.txt baseline.txt
	./textAdventure testScript.txt > transcript.txt
	cmp transcript.txt baseline.txt
	mv -f transcript.txt success.txt

clean:
	$(RM) textAdventure.* map.gv map.png transcript.txt success.txt
