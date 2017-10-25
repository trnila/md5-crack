CC:=nvcc
OPT:=-Xcompiler -fPIC -g
BINARY=crack

${BINARY}: md5.o cuda1.o main1.o
	${CC} ${OPT} $^ -o $@

%.o: %.cu
	${CC} ${OPT} -dc -c $^  -o $@

%.o: %.c
	${CC} ${OPT} -c $^  -o $@

clean:
	rm -f *.o ${BINARY}
