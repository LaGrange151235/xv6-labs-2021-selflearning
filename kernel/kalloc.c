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

// TODO: make a record for page reference supporting COW
#define NPAGE 32723
char  reference[NPAGE];

int // get the index in reference[] for physical address
get_reference_index(void *pa) {
  int index = ((char *)pa - (char *)PGROUNDUP((uint64)end)) / PGSIZE;
  return index;
}

int // get the reference content for physical address
get_reference(void *pa) {
  return reference[get_reference_index(pa)];
}

void // add the record bit
add_reference(void *pa) {
  ++reference[get_reference_index(pa)];
}

void // sub the record bit
sub_reference(void *pa) {
  if(reference[get_reference_index(pa)] > 0)
    --reference[get_reference_index(pa)];
}


struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    reference[get_reference_index(p)] = 0; // initialize reference record
    kfree(p);
  }
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

  sub_reference((void *)pa);
  if(get_reference(pa) == 0){ // only truly free memory when the reference number is zero
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);
    r = (struct run*)pa;
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
  {
    memset((char*)r, 5, PGSIZE); // fill with junk
    reference[get_reference_index((void *)r)] = 1; // update reference record
  }
  return (void*)r;
}
