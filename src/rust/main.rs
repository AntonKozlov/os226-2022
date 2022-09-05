extern crate core;

fn main() {
    // TODO
}


fn echo(args: Vec<&str>) -> i32 {
    println!("{}", args.join(" "));
    return args.len() as i32;
}

fn ret_code(args: Vec<&str>) -> i32 {
    return 0;
}