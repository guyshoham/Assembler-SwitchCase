.section .text
.globl switch2
switch2:
movq $0,%rbx
subq $21,%rdx
cmpq $10,%rdx
ja .LD
jmp *.T1(,%rcx,8)
.L27:
movq (%rdi),%rcx
movq %rcx,(%rsi)
movq (%rsi),%rbx
jmp .DONE
.L25:
movq (%rsi),%rcx
addq %rcx,(%rdi)
movq (%rdi),%rbx
jmp .DONE
.L29:
.L30:
subq $59,(%rdi)
movq (%rdi),%rcx
subq %rcx,(%rsi)
jmp .DONE
.L23:
addq $60,(%rdi)
.L21:
movq (%rsi),%rbx
imulq %rbx,%rbx
jmp .DONE
.L31:
movq (%rsi),%rbx
movq (%rdi),%rcx
shr %cl,%rbx
jmp .DONE
.LD:
movq $12,%rbx
shl $3,%rbx
ret
.DONE:
movq %rbx,%rax
ret
.section .rodata
.align 8
.T1:
.quad .L21
.quad .LD
.quad .L23
.quad .LD
.quad .L25
.quad .LD
.quad .L27
.quad .LD
.quad .L29
.quad .L30
.quad .L31
