all: main.o md5.o crack.o
	gcc -march=native -O2 $^ -o crack

clean:
	rm *.o crack

