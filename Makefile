CC=g++
OBJECTS=crack srv client
LDFLAGS=-lcrypto


all: $(OBJECTS)

crack: main.o md5.o crack.o
	$(CC) -march=native -O2 $^ -o $@  $(LDFLAGS)

srv: srv.o md5.o crack.o
	$(CC) -march=native -O2 $^ -o $@ $(LDFLAGS)

client: client.o crack.o md5.o
	$(CC) -march=native -O2 $^ -o $@ $(LDFLAGS)

clean:
	rm *.o $(OBJECTS)

