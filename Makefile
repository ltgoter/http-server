CC=g++
CPPFLAGS=-O3
# LD_FLAGS=
OBJ=main.o 

all: server test
	./bin/server > server.log &
	@#./bin/test   | tee test.log

server: ${OBJ}
	${CC} ${CPPFLAGS} $^ -o ./bin/server

test:

%.o:src/%.cpp
	${CC} ${CPPFLAGS} -c src/$*.cpp

clean:
	rm -rf ./bin/server ./bin/test
	rm *.o *.log