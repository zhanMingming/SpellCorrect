CC=g++
CFLAGS=-Wall -g
BIN=main
OBJS=MutexLock.o Condition.o Configure.o Cache.o Diction.o Index.o MySplit.0 Query.o test_Query.o
LIBS=-std=c++11 -lNLPIR
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@  -std=c++11 -lNLPIR
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
clean:
	rm -f *.o $(BIN)

