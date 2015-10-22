CFLAGS=-g -I. -I../
LDFLAGS=-lm
DEPS = ceed_api.h ../globals.h ../utils.h
OBJ = ceed.o ceed_api.o ../utils.o ../globals.o

all: ceed

debug: DFLAGS = -DDBG
debug: clean ceed

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DFLAGS)

ceed: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ) ceed *.exe

