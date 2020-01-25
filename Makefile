CC      	= clang
CFLAGS  	= -Wall -Wpedantic -Wextra -Werror

SPLIT 		= split
SPLITSRC 	= split.c galois.c poly.c io.c util.c
SPLITOBJ 	= $(SPLITSRC:%.c=%.o)

JOIN 		= join
JOINSRC 	= join.c galois.c poly.c io.c util.c
JOINOBJ 	= $(JOINSRC:%.c=%.o)

.PHONY: all clean infer

all: $(SPLIT) $(JOIN)

$(SPLIT): $(SPLITOBJ)
	$(CC) -o $@ $^

$(JOIN): $(JOINOBJ)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(SPLIT) $(SPLITOBJ) $(JOIN) $(JOINOBJ) infer-out

infer: clean
	infer-capture -- make
	infer-analyze -- make
