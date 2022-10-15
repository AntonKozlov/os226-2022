use std::any::Any;
use std::cell::RefCell;
use std::ops::Deref;
use std::rc::Rc;

use crate::Command;
use crate::mem_pool::MemPoolAllocator;
use crate::sched::Sched;

pub struct CoApp {
    sched: Rc<RefCell<Sched>>,
    entrypoints: Vec<Entrypoint>,
    allocator: MemPoolAllocator<CoAppCtx>,
}

#[derive(Default)]
pub struct CoAppCtx {
    cnt: i32,
}

pub type Ctx = dyn Any;

pub type Entrypoint = fn(&mut Sched, usize, Rc<RefCell<Ctx>>);

impl CoApp {
    pub fn new(sched: Rc<RefCell<Sched>>) -> Self {
        CoApp {
            sched,
            entrypoints: vec![CoApp::task, CoApp::rt],
            allocator: MemPoolAllocator::new(16),
        }
    }


    // Entrypoints

    fn task(sched: &mut Sched, id: usize, ctx: Rc<RefCell<Ctx>>) {
        let mut ctx_ref = ctx.deref().borrow_mut();
        let co_app_ctx: &mut CoAppCtx = ctx_ref.downcast_mut().unwrap();

        println!("{:>16} id {} cnt {}", "coapp_task", id, co_app_ctx.cnt);

        if co_app_ctx.cnt > 0 {
            sched.cont(2);
        }

        co_app_ctx.cnt -= 1;
    }

    fn rt(sched: &mut Sched, id: usize, ctx: Rc<RefCell<Ctx>>) {
        let mut ctx_ref = ctx.deref().borrow_mut();
        let co_app_ctx: &mut CoAppCtx = ctx_ref.downcast_mut().unwrap();

        println!("{:>16} id {} cnt {}", "coapp_rt", id, co_app_ctx.cnt);

        sched.time_elapsed(1);

        if co_app_ctx.cnt > 0 {
            sched.cont(0);
        }

        co_app_ctx.cnt -= 1;
    }
}

impl Command for CoApp {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        let entrypoint = {
            let entry_id = args[1].parse::<usize>().unwrap() - 1;
            self.entrypoints[entry_id]
        };
        let ctx = {
            let ctx = self.allocator.alloc().unwrap();
            let mut ctx_ref = ctx.deref().borrow_mut();
            ctx_ref.cnt = args[2].parse().unwrap();
            ctx.clone()
        };
        let priority: u32 = args[3].parse().unwrap();
        let deadline: u32 = args[4].parse::<i32>().unwrap() as u32;

        self.sched.deref().borrow_mut().add_task(entrypoint, ctx, priority, deadline);

        return 0;
    }
}