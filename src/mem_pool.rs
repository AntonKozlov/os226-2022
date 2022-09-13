use std::collections::LinkedList;

// Memory Pool Allocation

#[derive(Debug)]
pub struct Pool {
    pub buf: Vec<u8>,
    pub chunk_size: usize,
    pub chunk_count: usize,
    pub buf_len: usize,
    pub free_list: LinkedList<ChunkIndex>,
}

pub type ChunkIndex = usize;

impl Pool {
    pub fn new(chunk_size: usize, chunk_count: usize) -> Self {
        let buf_len = chunk_size * chunk_count;
        let mut pool = Pool {
            buf: vec![0; buf_len],
            chunk_size,
            chunk_count,
            buf_len,
            free_list: LinkedList::new(),
        };
        pool.free_all();
        pool
    }

    pub fn alloc(&mut self) -> Option<&mut [u8]> {
        self.free_list.pop_front().map(|chunk_index| self.buf[chunk_index..].as_mut())
    }

    pub fn free(&mut self, adr: *const u8) {
        let chunk_index = unsafe {
            adr.offset_from(self.buf.as_ptr())
        }.unsigned_abs() / self.chunk_size;
        self.free_list.push_front(chunk_index);
    }

    pub fn free_all(&mut self) {
        let chunk_count = self.buf_len / self.chunk_size;
        self.free_list.clone_from(&LinkedList::from_iter((0..chunk_count).into_iter()))
    }
}