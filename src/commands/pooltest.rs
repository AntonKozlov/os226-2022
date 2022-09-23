use crate::Command;
use crate::mem_pool::MemPoolAllocator;
use crate::util::{os_err, os_msg};

#[derive(Debug)]
pub struct PoolTest {
    pool: MemPoolAllocator<u8>,
}

impl PoolTest {
    pub fn new() -> Self {
        PoolTest {
            pool: MemPoolAllocator::new(4)
        }
    }
}

impl Command for PoolTest {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        match args.as_slice() {
            [_, "alloc"] => {
                let chunk_index = self.pool.alloc()
                    .map(|chunk| chunk as *const u8)
                    .map(|chunk_ptr| unsafe { self.pool.get_chunk_index(chunk_ptr) })
                    .map(|chunk_index| chunk_index as i32);
                os_msg!("alloc {}", chunk_index.unwrap_or(-1))
            }
            [_, "free", chunk_index_str] => {
                let chunk_index: usize = chunk_index_str.parse().unwrap();
                let chunk_ptr: *const u8 = &self.pool.get_buf()[chunk_index];
                unsafe { self.pool.free(chunk_ptr); }
                os_msg!("free {}", chunk_index)
            }
            _ => os_err!("Unknown command {}", args.join(" "))
        }
    }
}