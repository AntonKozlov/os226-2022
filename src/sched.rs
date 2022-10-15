use std::any::Any;
use std::cell::RefCell;
use std::cmp::Ordering;
use std::collections::VecDeque;
use std::ops::Deref;
use std::rc::Rc;

use crate::mem_pool::MemPoolAllocator;

pub struct Sched {
    time: u32,
    task_cmp: Option<TaskComparator>,
    curr_task: Option<Rc<RefCell<Task>>>,
    running_tasks: VecDeque<Rc<RefCell<Task>>>,
    suspended_tasks: VecDeque<(Rc<RefCell<Task>>, u32)>,
    allocator: MemPoolAllocator<Task>,
}

#[derive(Default)]
struct Task {
    id: usize,
    priority: u32,
    deadline: u32,
    entrypoint: Option<Entrypoint>,
    ctx: Option<Rc<RefCell<Ctx>>>,
}

type Ctx = dyn Any;

type Entrypoint = fn(&mut Sched, usize, Rc<RefCell<Ctx>>);

type TaskComparator = fn(&Task, &Task) -> Ordering;

/// Policy of task execution.
pub enum Policy {
    /// First-In, First-Out.
    /// Run tasks in order of their arrival.
    Fifo,

    /// Highest priority task (highest priority value) should be executed first.
    /// Use round-robin for processes with same priority (task from 1st process, from 2nd, ... nth, 1st, 2nd, ...).
    Priority,

    /// Consider deadline, execute process with Earliest Deadline First.
    /// Fallback to priority policy if deadlines are equal.
    Deadline,
}

impl Sched {
    /// Create new scheduler.
    pub fn new() -> Self {
        Sched {
            time: 0,
            task_cmp: None,
            curr_task: None,
            running_tasks: VecDeque::new(),
            suspended_tasks: VecDeque::new(),
            allocator: MemPoolAllocator::new(16),
        }
    }


    /// Scheduler loop, start executing tasks until all of them finish.
    pub fn run(&mut self, policy: Policy) {
        let task_cmp: TaskComparator = match policy {
            Policy::Fifo => |_a, _b| {
                Ordering::Equal
            },
            Policy::Priority => |a, b| {
                b.priority.cmp(&a.priority)
            },
            Policy::Deadline => |a, b| {
                a.deadline.cmp(&b.deadline)
                    .then(b.priority.cmp(&a.priority))
            },
        };
        self.task_cmp = Some(task_cmp);

        self.running_tasks.make_contiguous().sort_by(|a, b| {
            let a = &*a.deref().borrow();
            let b = &*b.deref().borrow();
            task_cmp(a, b).then(a.id.cmp(&b.id))
        });

        while let Some(task) = self.running_tasks.pop_front() {
            self.curr_task = Some(task.clone());

            let task = task.deref().borrow();
            let entrypoint = task.entrypoint.unwrap();
            let id = task.id;
            let ctx = task.ctx.clone().unwrap();

            drop(task);

            entrypoint(self, id, ctx);
        }
    }

    /// Add new task.
    /// Must be called ONLY before running!
    pub fn add_task(
        &mut self,
        entrypoint: Entrypoint,
        ctx: Rc<RefCell<Ctx>>,
        priority: u32,
        deadline: u32,
    ) {
        let task = self.allocator.alloc().unwrap();

        let mut task_ref = task.deref().borrow_mut();
        task_ref.id = self.running_tasks.len() + 1;
        task_ref.priority = priority;
        task_ref.deadline = deadline;
        task_ref.entrypoint = Some(entrypoint);
        task_ref.ctx = Some(ctx);

        self.running_tasks.push_back(task.clone());
    }


    /// Continue process from function after some amount of time.
    pub fn cont(&mut self, timeout: u32) {
        let curr = self.curr_task.clone().unwrap();
        if timeout == 0 {
            self.push_to_queue(curr);
        } else {
            self.suspend(curr, self.time + timeout);
        }
    }

    /// Notify scheduler that some amount of time passed.
    pub fn time_elapsed(&mut self, amount: u32) {
        self.time += amount;
        while let Some((_, waketime)) = self.suspended_tasks.front() {
            if *waketime <= self.time {
                let (task, _) = self.suspended_tasks.pop_front().unwrap();
                self.push_to_queue(task);
            } else {
                break;
            }
        }
    }


    // Utils

    fn push_to_queue(&mut self, task: Rc<RefCell<Task>>) {
        let cmp = self.task_cmp.unwrap();
        let index = self.running_tasks.iter().position(|t| {
            cmp(&*t.deref().borrow(), &*task.deref().borrow()).is_gt()
        }).unwrap_or(self.running_tasks.len());
        self.running_tasks.insert(index, task);
    }

    fn suspend(&mut self, task: Rc<RefCell<Task>>, awakening_time: u32) {
        let index = self.suspended_tasks.iter().position(|(_, at)| {
            at.cmp(&awakening_time).is_gt()
        }).unwrap_or(self.suspended_tasks.len());
        self.suspended_tasks.insert(index, (task, awakening_time));
    }
}

impl TryFrom<usize> for Policy {
    type Error = ();

    fn try_from(v: usize) -> Result<Self, Self::Error> {
        match v {
            0 => Ok(Policy::Fifo),
            1 => Ok(Policy::Priority),
            2 => Ok(Policy::Deadline),
            _ => Err(()),
        }
    }
}