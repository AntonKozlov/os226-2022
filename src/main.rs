extern crate core;

use std::collections::HashMap;
use std::io;

use crate::command::Command;
use crate::commands::{Echo, PoolTest, RetCode};
use crate::util::os_err;

mod command;
mod commands;
mod mem_pool;
mod util;

fn main() {
    let commands_list: [(&str, Box<dyn Command>); 3] = [
        ("echo", Box::new(Echo)),
        ("retcode", Box::new(RetCode)),
        ("pooltest", Box::new(PoolTest::new())),
    ];

    run_shell(HashMap::from(commands_list));
}

fn run_shell(mut commands: HashMap<&str, Box<dyn Command>>) {
    let mut ret_code = 0u8;

    for line in io::stdin().lines() {
        let cmds = line.as_ref()
            .expect("Failed to read line")
            .split(';')
            .map(|s| s.trim())
            .filter(|s| !s.is_empty());

        for cmd in cmds {
            let cmd_args: Vec<&str> = cmd.split_ascii_whitespace().collect();
            let cmd_name = cmd_args[0];

            ret_code = match commands.get_mut(cmd_name) {
                Some(cmd) => cmd.run(cmd_args, ret_code),
                None => os_err!("Unknown command {}", cmd_name)
            };
        }
    }
}