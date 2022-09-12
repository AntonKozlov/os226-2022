extern crate core;

use std::io;

fn main() {
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

            ret_code = match cmd_name {
                "echo" => echo(cmd_args, ret_code),
                "retcode" => get_ret_code(cmd_args, ret_code),
                &_ => {
                    eprintln!("Unknown command {}", cmd_name);
                    1
                }
            };
        }
    }
}


fn echo(args: Vec<&str>, _curr_ret_code: u8) -> u8 {
    println!("{}", args[1..args.len()].join(" "));
    return (args.len() - 1).try_into().unwrap_or(u8::MAX);
}

fn get_ret_code(_args: Vec<&str>, curr_ret_code: u8) -> u8 {
    println!("{}", curr_ret_code);
    return 0;
}