CFLAGS=-g -I. -I../
LDFLAGS=-lm
DEPS = turff_api.h ../globals.h ../utils.h
OBJ = turff.o turff_api.o ../utils.o ../globals.o

all: turff

debug: DFLAGS=-DDBG
debug: clean turff

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DFLAGS)

turff: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ) turff
