CFLAGS =-g -I. -I../
LDFLAGS =-lm

ifeq ($(CC),x86_64-w64-mingw32-gcc)
LDFLAGS += -lws2_32
endif

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

windows:
	docker ps -a -q --filter "name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rm -f {}
	docker images -q --filter "label=name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rmi {}
	cd ../dockerfiles; docker build -t crops/ceed:windows -f Dockerfile.ceed.win ../
	docker run --name=ceed-windows crops/ceed:windows; mkdir -p ../outdir
	docker cp ceed-windows:/usr/local/crops/ceed/ceed ../outdir/ceed.exe
	docker ps -a -q --filter "name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rm -f {}
	docker images -q --filter "label=name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rmi {}

linux:
	docker ps -a -q --filter "name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rm -f {}
	docker images -q --filter "label=name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rmi {}
	cd ../dockerfiles; docker build -t crops/ceed:linux -f Dockerfile.ceed.linux ../
	docker run --name=ceed-linux crops/ceed:linux; mkdir -p ../outdir
	docker cp ceed-linux:/usr/local/crops/ceed/ceed ../outdir/ceed
	docker ps -a -q --filter "name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rm -f {}
	docker images -q --filter "label=name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rmi {}
