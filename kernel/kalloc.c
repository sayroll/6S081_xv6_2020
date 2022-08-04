// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  for(int i =0;i<NCPU;i++)
  {
    initlock(&kmem[i].lock, "kmem");//名字不变，根据hint可以命名成kmem
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();//先将终端关了
  int cpuNum = cpuid();
  acquire(&kmem[cpuNum].lock);
  r->next = kmem[cpuNum].freelist;
  kmem[cpuNum].freelist = r;
  release(&kmem[cpuNum].lock);

  pop_off();//恢复中断

}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();//关掉中断
  int cpuNum = cpuid();
  acquire(&kmem[cpuNum].lock);
  r = kmem[cpuNum].freelist;
  if(r)//拿到了
    kmem[cpuNum].freelist = r->next;
  release(&kmem[cpuNum].lock);

  //如果当前的cup的freelist上没有空闲的地方了
  if(!r)
  {
    //去拿其他cpu的freelist
    for(int i = 0; i< NCPU;i++)
    {
      if(i == cpuNum)//如果是当前的
        continue;
      acquire(&kmem[i].lock);
      r = kmem[i].freelist;
      //如果成功的stealing
      if(r)
      {
        kmem[i].freelist=r->next;//取一个
        release(&kmem[i].lock);
        break;
      }
      release(&kmem[i].lock);
    }
  }
  pop_off();
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
