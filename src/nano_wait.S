
.cpu cortex-m0
.thumb
.syntax unified

//48MHz, the following loop is 4 cycles
//12,000,000 loops per second
// 8.33.3333 ns per loop

.global nano_wait

nano_wait:
		subs r0, #83			//1 cycle
		bge nano_wait
		bx lr



