use crate::Command;
use crate::mem_pool::{ChunkIndex, Pool};
use crate::util::{os_err, os_msg};

#[derive(Debug)]
pub struct PoolTest {
    pool: Pool,
}

impl PoolTest {
    pub fn new() -> Self {
        PoolTest {
            pool: Pool::new(1, 4)
        }
    }
}

impl Command for PoolTest {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        match args.as_slice() {
            [_, "alloc"] => {
                let chunk_addr = self.pool.alloc()
                    .map(|chunk_slice_ref| chunk_slice_ref.first())
                    .flatten()
                    .map(|chunk_ref| chunk_ref as *const u8);
                let chunk_index = chunk_addr.map(
                    |chunk_addr| (
                        unsafe {
                            chunk_addr.offset_from(self.pool.buf.as_ptr())
                        }.unsigned_abs() / self.pool.chunk_size
                    ) as isize
                );
                println!("alloc {}", chunk_index.unwrap_or(-1));
                0
            }
            [_, "free", chunk_index_str] => {
                let chunk_index: ChunkIndex = chunk_index_str.parse().unwrap();
                self.pool.free(&self.pool.buf[chunk_index]);
                os_msg!("free {}", chunk_index)
            }
            _ => os_err! { "Wrong command {}", args.join(" ") }
        }
    }
}