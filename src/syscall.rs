use std::arch::asm;

pub struct SysCall;

macro_rules! define_syscall {
    ($syscall_name: literal, $syscall_num: expr) => {
        paste! {
            macro_rules! [<sys_ $syscall_name>] {
                ($arg1: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, 0, 0, 0, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, 0, 0, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, 0, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr, $arg4: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, $arg4 as u64, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr, $arg4: expr, $arg5: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, $arg4 as u64, $arg5 as u64, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr, $arg4: expr, $arg5: expr, $arg6: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, $arg4 as u64, $arg5 as u64, $arg6 as u64,
                    )
                );
            }
        }
    };
}

pub(crate) use define_syscall;

#[cfg(target_os = "linux")]
impl SysCall {
    #[inline(never)]
    pub unsafe fn syscall(syscall_num: u64, arg1: u64, arg2: u64, arg3: u64, arg4: u64, arg5: u64, arg6: u64) -> i64 {
        let ret;

        asm!(
            "syscall",
            inlateout("rax") syscall_num => ret,
            in("rdi") arg1,
            in("rsi") arg2,
            in("rdx") arg3,
            in("r10") arg4,
            in("r8") arg5,
            in("r9") arg6,
            out("rcx") _,
            out("r11") _,
            options(nostack),
        );

        return ret;
    }
}