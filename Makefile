CC=g++
CPPFLAGS=-O3 -I ./header
LD_FLAGS=-levent
OBJ=main.o httpparser.o requestHandler.o

all: server test
	./bin/server 
	# > server.log
	@#./bin/test   | tee test.log

server: ${OBJ}
	${CC} ${CPPFLAGS} $^ -o ./bin/server ${LD_FLAGS} 

test:

%.o:src/%.cpp
	${CC} ${CPPFLAGS} -c src/$*.cpp ${LD_FLAGS} 

clean:
	rm -rf ./bin/server ./bin/test
	rm *.o *.log