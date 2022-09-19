extern crate core;
extern crate libc;
extern crate nix;

use std::collections::HashMap;
use std::io;
use std::process::exit;

use libc::{REG_R10, REG_R8, REG_R9, REG_RAX, REG_RDI, REG_RDX, REG_RIP, REG_RSI, sigemptyset, sigset_t};
use nix::sys::signal;
use nix::sys::signal::SaFlags;
use signal::{SigAction, SigHandler, SigSet};

use crate::command::Command;
use crate::commands::{Echo, PoolTest, RetCode, SysCallTest};
use crate::syscall::{define_syscall, SysCall};
use crate::util::os_err;

mod command;
mod commands;
mod mem_pool;
mod syscall;
mod util;

#[allow(dead_code)]
extern "C" fn signal_handler(_: libc::c_int, _: *mut libc::siginfo_t, ctx: *mut libc::c_void) {
    unsafe {
        let uc = (ctx as *const libc::ucontext_t).as_ref().unwrap();
        let mut regs = uc.uc_mcontext.gregs;

        let insn: u16 = *(regs[REG_RIP as usize] as *const u16);
        if insn != 0x81cd {
            libc::abort();
        }

        regs[REG_RAX as usize] = SysCall::syscall(
            regs[REG_RAX as usize] as u64,
            regs[REG_RDI as usize] as u64,
            regs[REG_RSI as usize] as u64,
            regs[REG_RDX as usize] as u64,
            regs[REG_R10 as usize] as u64,
            regs[REG_R8 as usize] as u64,
            regs[REG_R9 as usize] as u64,
        );

        regs[REG_RIP as usize] += 2;
    }
}

fn install_signal_handler() -> nix::Result<SigAction> {
    let sig_action = &SigAction::new(
        SigHandler::SigAction(signal_handler),
        SaFlags::from(SaFlags::SA_RESTART),
        SigSet::empty(),
    );
    unsafe {
        sigemptyset(&sig_action.mask() as *const SigSet as *mut sigset_t);
        return signal::sigaction(signal::SIGINT, &sig_action);
    }
}


fn main() {
    let exit_code = match install_signal_handler() {
        Err(errno) => {
            eprintln!("Fail to install signal handlers due to: {} ({}).", errno.desc(), errno);
            1
        }
        Ok(..) => {
            let commands_list: [(&str, Box<dyn Command>); 4] = [
                ("echo", Box::new(Echo)),
                ("retcode", Box::new(RetCode)),
                ("pooltest", Box::new(PoolTest::new())),
                ("syscalltest", Box::new(SysCallTest)),
            ];
            run_shell(HashMap::from(commands_list));
            0
        }
    };
    exit(exit_code);
}

fn run_shell(mut commands: HashMap<&str, Box<dyn Command>>) {
    let mut ret_code = 0u8;

    for line in io::stdin().lines() {
        let cmd_exprs = line.as_ref()
            .expect("Failed to read line")
            .split(';')
            .map(|s| s.trim())
            .filter(|s| !s.is_empty());

        for cmd_expr in cmd_exprs {
            let cmd_args: Vec<&str> = cmd_expr.split_ascii_whitespace().collect();
            let cmd_name = cmd_args[0];

            ret_code = match commands.get_mut(cmd_name) {
                Some(cmd) => cmd.run(cmd_args, ret_code),
                None => os_err!("Unknown command {}", cmd_name)
            };
        }
    }
}