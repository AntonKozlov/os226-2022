#define _GNU_SOURCE

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <ucontext.h>

uint64_t custom_syscall(
    uint64_t syscall_num,
    uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5
) {
    uint64_t ret;
    __asm__ __volatile__(
        "int $0x81\n"
        : "=a"(ret)
        : "a"(syscall_num), // rax
          "b"(arg1),        // rbx
          "c"(arg2),        // rcx
          "d"(arg3),        // rdx
          "S"(arg4),        // rsi
          "D"(arg5)         // rdi
        :
    );
    return ret;
}

static void sighnd(int sig, siginfo_t *info, void *ctx) {
    ucontext_t *uc = (ucontext_t *) ctx;
    greg_t *regs = uc->uc_mcontext.gregs;

    if (*(uint16_t *) regs[REG_RIP] != 0x81cd) abort();

    regs[REG_RAX] = custom_syscall(
        regs[REG_RAX],
        regs[REG_RBX], regs[REG_RCX], regs[REG_RDX], regs[REG_RSI], regs[REG_RDI]
    );

    regs[REG_RIP] += 2;
}

int install_signal_handler() {
    struct sigaction act = {
        .sa_sigaction = sighnd,
        .sa_flags = SA_RESTART,
    };

    sigemptyset(&act.sa_mask);

    if (-1 == sigaction(SIGSEGV, &act, NULL)) {
        perror("Fail to install signal handlers");
        return 1;
    }

    return 0;
}
