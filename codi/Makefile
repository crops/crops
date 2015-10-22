CFLAGS=-g -I. -I../
LDFLAGS=-lm
DEPS = codi.h codi_list.h ../globals.h ../utils.h
OBJ = codi.o codi_list.o codi_api.o ../utils.o ../globals.o

all: codi

debug: DFLAGS=-DDBG
debug: clean codi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DFLAGS)

codi: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ) codi
