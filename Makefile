CC=g++
CFLAGS=-O3


all: server test
	./bin/server > server.log &
	./bin/test   | tee test.log

server:

test:


clean:
	rm -rf ./bin/server ./bin/test
	rm *.o