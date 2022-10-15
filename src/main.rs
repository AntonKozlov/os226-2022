use std::cell::RefCell;
use std::collections::HashMap;
use std::io;
use std::process::exit;
use std::rc::Rc;

use crate::command::Command;
use crate::commands::{CoApp, CoSched, Echo, PoolTest, RetCode, SysCallTest};
use crate::sched::Sched;
use crate::syscall::define_syscall;
use crate::util::os_err;

mod command;
mod commands;
mod mem_pool;
mod sched;
mod syscall;
mod util;

fn main() {
    if unsafe { install_signal_handler() } == 0 {
        let sched = Rc::new(RefCell::new(Sched::new()));
        let commands_list: [(&str, Box<dyn Command>); 6] = [
            ("coapp", Box::new(CoApp::new(sched.clone()))),
            ("cosched", Box::new(CoSched::new(sched))),
            ("echo", Box::new(Echo)),
            ("pooltest", Box::new(PoolTest::new())),
            ("retcode", Box::new(RetCode)),
            ("syscalltest", Box::new(SysCallTest)),
        ];
        run_shell(HashMap::from(commands_list));
        exit(0)
    } else {
        exit(1)
    }
}

fn run_shell(mut commands: HashMap<&str, Box<dyn Command>>) {
    let mut ret_code = 0u8;

    for line in io::stdin().lines() {
        let cmd_exprs = line
            .as_ref()
            .expect("Failed to read line")
            .split('#').nth(0).unwrap()
            .split(';')
            .map(|s| s.trim()).filter(|s| !s.is_empty());

        for cmd_expr in cmd_exprs {
            let cmd_args: Vec<&str> = cmd_expr.split_ascii_whitespace().collect();
            let cmd_name = cmd_args[0];

            ret_code = match commands.get_mut(cmd_name) {
                Some(cmd) => cmd.run(cmd_args, ret_code),
                None => os_err!("Unknown command {}", cmd_name),
            };
        }
    }
}


// C FFI

#[link(name = "syscall", kind = "static")]
extern "C" {
    fn install_signal_handler() -> i32;
}