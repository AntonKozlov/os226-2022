use crate::command::Command;
use crate::util::os_msg;

#[derive(Debug)]
pub struct RetCode;

impl Command for RetCode {
    fn run(&mut self, _args: Vec<&str>, curr_ret_code: u8) -> u8 {
        os_msg!("{}", curr_ret_code)
    }
}