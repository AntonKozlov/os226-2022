use std::collections::LinkedList;
use std::ptr::null;

#[derive(Debug)]
pub struct MemPoolAllocator {
    buf: Vec<u8>,
    chunk_count: usize,
    free_list: LinkedList<ChunkIndex>,
}

pub type ChunkIndex = usize;

impl MemPoolAllocator {
    pub fn new(chunk_size: usize, chunk_count: usize) -> Self {
        let mut pool = MemPoolAllocator {
            buf: vec![0; chunk_size * chunk_count],
            chunk_count,
            free_list: LinkedList::new(),
        };
        pool.free_all();
        return pool;
    }


    pub fn get_chunk_size(&self) -> usize { self.buf.len() / self.chunk_count }

    pub fn get_chunk_count(&self) -> usize { self.chunk_count }

    pub fn get_buf(&self) -> &Vec<u8> { &self.buf }

    pub unsafe fn get_chunk_index(&self, chunk_ptr: *const u8) -> ChunkIndex {
        chunk_ptr.offset_from(self.buf.as_ptr()).unsigned_abs() / self.get_chunk_size()
    }


    pub fn alloc(&mut self) -> Option<&mut [u8]> {
        self.free_list
            .pop_front()
            .map(|chunk_index| {
                let chunk_size = self.get_chunk_size();
                let chunk_range = (chunk_size * chunk_index)..(chunk_size * (chunk_index + 1));
                self.buf[chunk_range].as_mut()
            })
    }

    pub unsafe fn free(&mut self, chunk_ptr: *const u8) {
        if chunk_ptr != null() {
            let chunk_index = self.get_chunk_index(chunk_ptr);
            self.free_list.push_front(chunk_index);
        }
    }

    pub fn free_all(&mut self) {
        self.free_list.clone_from(
            &LinkedList::from_iter((0..self.chunk_count).into_iter())
        )
    }
}