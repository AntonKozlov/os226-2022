macro_rules! os_msg { ($($arg:tt)*) => { { println!($($arg)*); 0 } } }
macro_rules! os_msg_with_custom_ret_code { ($retcode:expr, $($arg:tt)*) => { { println!($($arg)*); $retcode } } }
macro_rules! os_err { ($($arg:tt)*) => { { eprintln!($($arg)*); 1 } } }

/// Continuous allocation (like calloc).
/// It works like vec!\[expr; n\], but without requiring the [Clone] trait.
macro_rules! cont_vec {
    ($elem:expr; $n:expr) => { {
        let mut vec = Vec::with_capacity($n);
        for _ in 0..$n { vec.push($elem); }
        vec
    } };
}

pub(crate) use os_err;
pub(crate) use os_msg;
pub(crate) use os_msg_with_custom_ret_code;

pub(crate) use cont_vec;