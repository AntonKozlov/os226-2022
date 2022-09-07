extern crate core;

use std::io;

fn main() {
    let mut retcode_value = 0;

    for line in io::stdin().lines() {
        let cmds = line.as_ref()
            .expect("Failed to read line")
            .split(';')
            .map(|s| s.trim())
            .filter(|s| !s.is_empty());

        for cmd in cmds {
            let cmd_parts: Vec<&str> = cmd.split_ascii_whitespace().collect();

            let cmd_name = cmd_parts[0];
            let cmd_args = cmd_parts[1..cmd_parts.len()].to_vec();

            retcode_value = match cmd_name {
                "echo" => echo(cmd_args, retcode_value),
                "retcode" => retcode(cmd_args, retcode_value),
                &_ => {
                    eprintln!("Unknown command {}", cmd_name);
                    1
                }
            };
        }
    }
}


fn echo(args: Vec<&str>, _curr_ret_code: i32) -> i32 {
    println!("{}", args.join(" "));
    return args.len() as i32;
}

fn retcode(_args: Vec<&str>, curr_ret_code: i32) -> i32 {
    println!("{}", curr_ret_code);
    return 0;
}