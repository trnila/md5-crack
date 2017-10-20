CC:=nvcc
OPT:=-Xcompiler -fPIC -ccbin=/usr/bin/gcc-4.9 -g
BINARY=crack

${BINARY}: md5.o cuda1.o main1.o ../crack.o
	${CC} ${OPT} $^ -o $@

%.o: %.cu
	${CC} ${OPT} -dc -c $^  -o $@

%.o: %.c
	${CC} ${OPT} -c $^  -o $@

clean:
	rm -f *.o ${BINARY}
