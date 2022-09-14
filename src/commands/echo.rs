use crate::Command;
use crate::util::os_msg_with_custom_ret_code;

#[derive(Debug)]
pub struct Echo;

impl Command for Echo {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        let ret_code = (args.len() - 1).try_into().unwrap_or(u8::MAX);
        os_msg_with_custom_ret_code!(ret_code, "{}", args[1..args.len()].join(" "))
    }
}