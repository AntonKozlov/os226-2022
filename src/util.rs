macro_rules! os_msg { ($($arg:tt)*) => { { println!($($arg)*); 0 } } }
macro_rules! os_msg_ret { ($retcode:expr, $($arg:tt)*) => { { println!($($arg)*); $retcode } } }
macro_rules! os_err { ($($arg:tt)*) => { { eprintln!($($arg)*); 1 } } }

pub(crate) use os_msg;
pub(crate) use os_msg_ret;
pub(crate) use os_err;