CFLAGS=-g -I. -I../
LDFLAGS=-lsqlite3 -lm -ljansson -lcurl -lpthread
DEPS = ../globals.h ../utils.h codi_api.h codi_db.h codi_launcher.h codi_list.h
OBJ = codi.o codi_db.o codi_list.o codi_launcher.o codi_api.o ../utils.o ../globals.o

all: codi

debug: DFLAGS = -DDBG
debug: clean codi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DFLAGS)

codi: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ) codi
