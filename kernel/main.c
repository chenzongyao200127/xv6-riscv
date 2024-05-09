#include "types.h"       // Includes basic data types
#include "param.h"       // Kernel parameters
#include "memlayout.h"   // Memory layout of the kernel
#include "riscv.h"       // RISC-V specific definitions
#include "defs.h"        // Definitions of kernel functions and variables

volatile static int started = 0; // Static flag to indicate if the kernel has started

// The main function is the entry point for the kernel.
void main() {
  // Check if the current CPU is the bootstrap processor (BSP)
  if (cpuid() == 0) {
    // Initialize the console for kernel messages
    consoleinit();
    printfinit();

    // Print kernel boot messages
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");

    // Initialize various kernel subsystems
    kinit();         // Initialize physical page allocator
    kvminit();       // Initialize kernel virtual memory
    kvminithart();   // Enable paging
    procinit();      // Initialize process table
    trapinit();      // Initialize trap handling
    trapinithart();  // Install kernel trap vector
    plicinit();      // Initialize the Platform-Level Interrupt Controller (PLIC)
    plicinithart();  // Configure PLIC for device interrupts
    binit();         // Initialize buffer cache
    iinit();         // Initialize inode table
    fileinit();      // Initialize file table
    virtio_disk_init(); // Initialize emulated hard disk
    userinit();      // Start the first user process

    // Synchronize memory accesses
    __sync_synchronize();

    // Indicate that the kernel has started
    started = 1;
  } else {
    // For non-BSP CPUs, wait until the kernel has started
    while (started == 0);

    // Synchronize memory accesses
    __sync_synchronize();

    // Print a message indicating that this CPU (hart) is starting
    printf("hart %d starting\n", cpuid());

    // Initialize CPU-specific settings
    kvminithart();    // Enable paging
    trapinithart();   // Install kernel trap vector
    plicinithart();   // Configure PLIC for device interrupts
  }

  // Start the scheduler
  scheduler();        
}
