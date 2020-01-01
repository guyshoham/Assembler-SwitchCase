.section .text
.globl switch2
switch2: movq $0,%rbx
.L27:
movq %rsi,%rdi
movq %rbx,%rsi
jmp .DONE:
.L25:
addq %rdi,%rsi
movq %rbx,%rdi
jmp .DONE:
.L29:
.L30:
subq %rdi,$59
subq %rsi,%rdi
jmp .DONE:
.L23:
addq %rdi,$60
.L21:
movq %rbx,%rsi
mulq %rbx,%rbx
jmp .DONE:
.L31:
movq %rbx,%rsi
sarq (%rdi),%rbx
jmp .DONE:
.LD:
movq %rbx,X
shlq $3,%rbx
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
