pub struct SysCall;

impl SysCall {
    pub unsafe fn syscall(syscall_num: u64, arg1: u64, arg2: u64, arg3: u64, arg4: u64, arg5: u64) -> i64 {
        syscall(syscall_num, arg1, arg2, arg3, arg4, arg5)
    }
}

macro_rules! define_syscall {
    ($syscall_name: literal, $syscall_num: expr) => {
        paste! {
            macro_rules! [<sys_ $syscall_name>] {
                ($arg1: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, 0, 0, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, 0, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, 0, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr, $arg4: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, $arg4 as u64, 0,
                    )
                );
                ($arg1: expr, $arg2: expr, $arg3: expr, $arg4: expr, $arg5: expr) => (
                    SysCall::syscall(
                        $syscall_num as u64,
                        $arg1 as u64, $arg2 as u64, $arg3 as u64, $arg4 as u64, $arg5 as u64,
                    )
                );
            }
        }
    };
}

pub(crate) use define_syscall;


// C FFI

#[link(name = "syscall", kind = "static")]
extern "C" {
    fn syscall(syscall_num: u64, arg1: u64, arg2: u64, arg3: u64, arg4: u64, arg5: u64) -> i64;
}