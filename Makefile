CFLAGS = -g -MMD -MT $@ -MF $@.d

all : main test

test: main
	for  test_ in batch pool syscall preempt; do \
        echo -n "$$test_ "; ./test/run.sh $$test_ > /dev/null && echo -e "\033[1;32mOK\033[39;49m" || echo -e "\033[1;31mFAIL\033[39;49m"; \
    done

main : $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(wildcard *.[cS])))
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean :
	rm -f *.o{.d,} main

-include $(patsubst %,%.d,$(OBJ))

