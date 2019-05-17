We utilized a "doubly-doubly-linked" list along with a hash **map**. The base unit of the list, our **struct page**, has 4 pointers, and the **memory** address it represents.

Two of the pointers (**prev**, **next**) represent the doubly-linked list of the whole TLB, i.e. iterating via **next** would take you through every single entry.

The two other pointers (**prevHash**, **nextHash**) represent a subsequence of the linked list of the whole TLB. The subsequence is all of the elements in the list that hash to the same value. So iterating via **nextHash** would take you through all the entries with a particular hash value.

The hash **map** has 256 buckets, and each entry is a pointer to the **struct page** that is closest to the front of the whole list.

Using linked-lists is an appropriate choice for the task, as once an element is found (or not), the amount of operations needed to insert at the head, extract from the middle, or remove the tail are all in **O(1)**.

Doing a sequential iteration through a large part of a *regular* linked-list for every reference would be quite inefficient, and this is where the hash **map** comes in.

For every incoming memory access, we divide the number by the **pagesize**, and then modulo by 256. Then, we look at the entry in the **map** at the appropriate index, and iterate via **nextHash**. In an ideal scenario, each hash bucket would be populated equally, and would roughly be faster than a naive sequential search by a factor of 256. 256 was chosen as we looked at various runs of the program, and saw that 256 had a high percentage of its domain covered by the memory accesses, while still being a relatively large number.

For LRU, the benefit of using a linked list as opposed to an array or circular buffer is that bringing a page to the front of the list is done in **O(1)** as a few pointer assignments is all that is needed. An array/circular buffer would require shifting by 1 all the elements traversed.

Finally, the space used for the linked list is allocated as required. This prevents the need to allocate a humongous space right off the get-go.

We used gnuplot with a pre-configured gnuplot script **plotter.gp** to generate the performance result plots. We plotted page sizes on the x-axis, hit-rate on the y-axis, and multiple lines as different TLB sizes. A script **run.sh** runs valtlb379 on 288 different inputs to generate the plots we have. Each plot needs 16 runs (4 TLB sizes \* 4 page sizes), and there are 18 plots (2 policies \* 3 input sizes \* 3 programs) -> 16\*18 = 288.
