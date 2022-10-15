use std::cell::RefCell;
use std::ops::Deref;
use std::rc::Rc;

use crate::{Command, Sched};

pub struct CoSched {
    sched: Rc<RefCell<Sched>>,
}

impl CoSched {
    pub fn new(sched: Rc<RefCell<Sched>>) -> Self {
        CoSched {
            sched
        }
    }
}

impl Command for CoSched {
    fn run(&mut self, args: Vec<&str>, _curr_ret_code: u8) -> u8 {
        let policy_index: usize = args[1].parse().unwrap();
        self.sched.deref().borrow_mut().run(policy_index.try_into().unwrap());

        return 0;
    }
}