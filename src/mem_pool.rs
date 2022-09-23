use std::ptr::null;

#[derive(Debug)]
pub struct MemPoolAllocator<T> {
    buf: Vec<T>,
    bit_scale: u64,
}

impl<T: Default + Clone> MemPoolAllocator<T> {
    pub fn new(chunks_count: usize) -> Self {
        if chunks_count == 0 || chunks_count > u64::BITS as usize {
            panic!(
                "Illegal count of chunks in the memory pool. Allowed counts are {}..{}, but received {}",
                1, u64::BITS,
                chunks_count
            )
        }
        MemPoolAllocator {
            buf: vec![T::default(); chunks_count],
            bit_scale: 0,
        }
    }


    pub fn alloc(&mut self) -> Option<&mut T> {
        let free_chunk_index = self.bit_scale.trailing_ones() as usize;
        if free_chunk_index < self.buf.len() {
            self.bit_scale |= 1 << free_chunk_index;
            return self.buf.get_mut(free_chunk_index);
        }
        return None;
    }

    pub unsafe fn free(&mut self, chunk_ptr: *const T) {
        if chunk_ptr != null() {
            let chunk_index = self.get_chunk_index(chunk_ptr);
            self.bit_scale &= !(1 << chunk_index);
        }
    }

    pub fn free_all(&mut self) {
        self.bit_scale = 0;
    }


    pub fn get_buf(&self) -> &Vec<T> { &self.buf }

    pub unsafe fn get_chunk_index(&self, chunk_ptr: *const T) -> usize {
        chunk_ptr.offset_from(self.buf.as_ptr()).unsigned_abs()
    }
}