all: main.o md5.o
	gcc -march=native -O2 $^ -o crack

clean:
	rm *.o crack

