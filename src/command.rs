pub trait Command {
    fn run(&mut self, args: Vec<&str>, curr_ret_code: u8) -> u8;
}