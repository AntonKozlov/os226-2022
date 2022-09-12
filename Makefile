.PHONY: test
CFLAGS = -g

all : main test

test: main
	./test/run.sh batch && echo -e "\033[1;32mOK\033[39;49m" || echo -e "\033[1;31mFAIL\033[39;49m"
	./test/run.sh pool && echo -e "\033[1;32mOK\033[39;49m" || echo -e "\033[1;31mFAIL\033[39;49m"

main : $(patsubst %.c,%.o,$(wildcard *.c))

clean :
	rm -f *.o main
