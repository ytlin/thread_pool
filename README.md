# thread_pool

## Practice how to use the threads pool to speed up Quiksort(parallel)
#### Objective
* Multithreaded sorting using a thread pool
  * \# of threads in the pool determines the max degree of parallelism
* The problem definition is the same as that in the
  previous assignment, except that the binding of
  sorting jobs to threads is dynamic
* A job performs one of the following
  * Partitioning a large array into two small arrays
    * Left array < pivot < right array
  * Sorting a bottom‐level array
    * Bubble sort, insertion sort, etc.
    

