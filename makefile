C = object.c misc.c noun.c location.c move.c inventory.c parsexec.c main.c
H = object.h misc.h noun.h location.h move.h inventory.h parsexec.h

all: $(C) $(H)
	gcc *.c -o textAdventure

object.h: object.awk object.txt
	awk -v pass=h -f object.awk object.txt > $@
object.c: object.awk object.txt
	awk -v pass=c1 -f object.awk object.txt > $@
	awk -v pass=c2 -f object.awk object.txt >> $@
map.png: map.gv
	dot -Tpng -o $@ $<
map.gv: map.awk object.txt
	awk -f map.awk object.txt > $@

clean:
	$(RM) object.c object.h textAdventure.* map.gv map.png transcript.txt success.txt