use std::io;
use std::io::Write;

use paste::paste;

use crate::{Command, define_syscall};
use crate::syscall::SysCall;

#[derive(Debug)]
pub struct SysCallTest;

define_syscall!("write", 1);

impl Command for SysCallTest {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        let msg = args[1].to_string() + "\n";

        let stdout_descriptor_num = 1;
        let msg_ptr = msg.as_ptr();
        let msg_len = msg.len();

        let ret = unsafe { sys_write!(stdout_descriptor_num, msg_ptr, msg_len) } as u8 - 1;

        io::stdout().flush().unwrap();

        return ret;
    }
}