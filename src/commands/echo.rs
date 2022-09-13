use crate::Command;
use crate::util::os_msg_ret;

#[derive(Debug)]
pub struct Echo;

impl Command for Echo {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        os_msg_ret!((args.len() - 1).try_into().unwrap_or(u8::MAX), "{}", args[1..args.len()].join(" "))
    }
}