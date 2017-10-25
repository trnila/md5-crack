CC=g++
OBJECTS=crack srv client


all: $(OBJECTS)

crack: main.o md5.o crack.o
	$(CC) -march=native -O2 $^ -o $@

srv: srv.o md5.o crack.o
	$(CC) -march=native -O2 $^ -o $@

client: client.o crack.o md5.o
	$(CC) -march=native -O2 $^ -o $@

clean:
	rm *.o $(OBJECTS)

