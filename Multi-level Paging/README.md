# Multi-level Paging
A simulation of an operating system's hierarchical memory management with address mapping from virtual memory pages to physical memory.
Trace.tr is a log of memory references throughout a process that is passed as a command line argument, the number of subsequent arguments will indicate the number of
levels used, with the arguments themselves being the number of bits used at that level.

Optional command line arguments, -n = number of addresses from trace.tr to process, -c = TLB cache capacity

`./pagingwithtlb trace.tr 4 2 2 -n 100 -c 5`

This will call the executable file created by the Makefile with 3 levels of size 4, 2, 2 and a TLB cache of size 5 that will process the first 100 addresses in trace.tr.
