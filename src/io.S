//Nanowait function from the lecture slides


.cpu cortex-m0
.thumb
.syntax unified

.global micro_wait
micro_wait:
	nop // 1 cycle
	nop // 1 cycle
	nop // 1 cycle
	nop // 1 cycle
	subs r0, #1 // 1 cycle
	bne micro_wait // 3 cycles
	mov pc,lr	// Replace with proper push/pop if needed.
