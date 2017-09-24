all: main.o md5.o
	gcc $^ -o crack

clean:
	rm *.o crack

