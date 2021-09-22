CFLAGS = -g

all : main test

test: main
	for test_ in batch pool syscall cosched timer; do \
		echo -n "$$test_ "; ./test/run.sh $$test_ > /dev/null && echo -e "\033[1;32mOK\033[39;49m" || echo -e "\033[1;31mFAIL\033[39;49m"; \
	done

main : $(patsubst %.c,%.o,$(wildcard *.c))
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean :
	rm -f *.o main
