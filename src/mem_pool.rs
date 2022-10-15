use std::cell::RefCell;
use std::rc::Rc;

use crate::util::cont_vec;

#[derive(Debug)]
pub struct MemPoolAllocator<T> {
    buf: Vec<Rc<RefCell<T>>>,
    bit_scale: u64,
}

impl<T: Default> MemPoolAllocator<T> {
    pub fn new(chunks_count: usize) -> Self {
        if chunks_count == 0 || chunks_count > u64::BITS as usize {
            panic!(
                "Illegal count of chunks in the memory pool. Allowed counts are {}..{}, but received {}",
                1, u64::BITS,
                chunks_count
            )
        }
        MemPoolAllocator {
            buf: cont_vec![Rc::new(RefCell::new(T::default())); chunks_count],
            bit_scale: 0,
        }
    }


    pub fn alloc(&mut self) -> Option<Rc<RefCell<T>>> {
        let free_chunk_index = self.bit_scale.trailing_ones() as usize;
        if free_chunk_index < self.buf.len() {
            self.bit_scale |= 1 << free_chunk_index;
            return self.buf.get(free_chunk_index).cloned();
        }
        return None;
    }

    pub unsafe fn free(&mut self, chunk: &Rc<RefCell<T>>) {
        if let Some(chunk_index) = self.get_chunk_position(chunk) {
            self.bit_scale &= !(1 << chunk_index);
        }
    }

    pub fn free_all(&mut self) {
        self.bit_scale = 0;
    }


    pub fn get_buf(&self) -> &Vec<Rc<RefCell<T>>> { &self.buf }

    pub unsafe fn get_chunk_position(&self, chunk: &Rc<RefCell<T>>) -> Option<usize> {
        self.buf.iter().position(|ch| Rc::ptr_eq(ch, chunk))
    }
}