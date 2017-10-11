all: crack srv client

crack: main.o md5.o crack.o
	gcc -march=native -O2 $^ -o $@

srv: srv.o md5.o crack.o
	gcc -march=native -O2 $^ -o $@

client: client.o crack.o md5.o
	gcc -march=native -O2 $^ -o $@

clean:
	rm *.o crack

