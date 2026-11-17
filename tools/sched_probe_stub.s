# The i386 stub that tools/sched_probe.py writes into the compiler (STUB there is this, assembled with
# clang -target i386-pc-linux-gnu -c; stub1 at offset 0, stub2 at 302). Addresses are mwccarm.exe 2.0 sp2p3's:
# 0x4ff6d0 latest-cycle pass, 0x4ff730 select, 0x407f20 vfprintf, 0x40f390 fopen, 0x40f290 fflush,
# 0x6166ba the block's node list, 0x63a36c 'virtual registers active', 0x616680/0x61667c pressure counter/limit,
# 0x5f2f00 the FILE pointer (slack after .data), 0x4f5b00.. the format strings.

.text
.globl stub1
stub1:
 movl $0x4ff6d0, %ecx
 call *%ecx

 pushal
 
 cmpl $0, 0x5f2f00
 jne 1f
 pushl $0x4f5b00
 pushl $0x4f5b78
 movl $0x40f390, %ecx
 call *%ecx

 addl $8, %esp
 movl %eax, 0x5f2f00
1:

 movl 0x63a36c, %eax
 pushl %eax
movl %esp, %eax
 pushl %eax
pushl $0x4f5b02
 pushl 0x5f2f00
movl $0x407f20, %ecx
 call *%ecx
addl $16, %esp

 movl 0x6166ba, %esi
2:
 testl %esi, %esi
 je 9f
 movl 0xc(%esi), %edi
 movzwl 0x1a(%esi), %eax
 pushl %eax
 movzwl 0x18(%esi), %eax
 pushl %eax
 movzwl 0x16(%esi), %eax
 pushl %eax
 movzwl 0x14(%esi), %eax
 pushl %eax
 movzwl 0x12(%esi), %eax
 pushl %eax
 movzwl 0x10(%esi), %eax
 pushl %eax
 movswl 0x28(%edi), %eax
 pushl %eax
 pushl %edi
 pushl %esi
 movl %esp, %eax
 pushl %eax
 pushl $0x4f5b0c
 pushl 0x5f2f00
 movl $0x407f20, %ecx
 call *%ecx

 addl $48, %esp
 movswl 0x2c(%edi), %ebp
 leal 0x2e(%edi), %ebx
3:
 testl %ebp, %ebp
 jle 4f
 movswl 4(%ebx), %eax
 pushl %eax
 movzwl 2(%ebx), %eax
 pushl %eax
 movsbl 1(%ebx), %eax
 pushl %eax
 movzbl 0(%ebx), %eax
 pushl %eax
 movl %esp, %eax
 pushl %eax
 pushl $0x4f5b48
 pushl 0x5f2f00
 movl $0x407f20, %ecx
 call *%ecx

 addl $28, %esp
 addl $0xe, %ebx
 decl %ebp
 jmp 3b
4:
 movl 0x8(%esi), %ebx
5:
 testl %ebx, %ebx
 je 6f
 movswl 8(%ebx), %eax
 pushl %eax
 pushl 4(%ebx)
 movl %esp, %eax
 pushl %eax
 pushl $0x4f5b56
 pushl 0x5f2f00
 movl $0x407f20, %ecx
 call *%ecx

 addl $20, %esp
 movl (%ebx), %ebx
 jmp 5b
6:
 movl %esp, %eax
 pushl %eax
pushl $0x4f5b5e
 pushl 0x5f2f00
movl $0x407f20, %ecx
 call *%ecx
addl $12, %esp

 movl (%esi), %esi
 jmp 2b
9:
 pushl 0x5f2f00
movl $0x40f290, %ecx
 call *%ecx
addl $4, %esp

 popal
 ret

.globl stub2
stub2:
 pushl 8(%esp)
 pushl 8(%esp)
 movl $0x4ff730, %ecx
 call *%ecx

 addl $8, %esp
 pushal
 
 cmpl $0, 0x5f2f00
 jne 1f
 pushl $0x4f5b00
 pushl $0x4f5b78
 movl $0x40f390, %ecx
 call *%ecx

 addl $8, %esp
 movl %eax, 0x5f2f00
1:

 movl 28(%esp), %eax
 movl 40(%esp), %ecx
 movl 0x616680, %edx
 movl 0x61667c, %ebx
 pushl %ebx
pushl %edx
pushl %eax
pushl %ecx
movl %esp, %eax
 pushl %eax
pushl $0x4f5b60
 pushl 0x5f2f00
movl $0x407f20, %ecx
 call *%ecx
addl $28, %esp

 popal
 ret
