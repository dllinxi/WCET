#
# Switch from protected mode to real mode and jump to setup.S
# image located at %cx:0.
#
# This module must be placed into physical memory at 0:7C00h.
# EFI has some real mode thunking code at 2000:0h.
#
# Processor and non-maskable interrupts should be disabled
# before control is passed to this module.
#

#define __KERNEL_CS 0x38
#define MSR_EFER    0xc0000080
#define _EFER_LME 8  /* Long mode enable */

.global _start

.text
_start:
        #
        # Load identity mapped GDT & real mode IDT.
        # Add 7C00h to the addresses since this is linked to start
        # at 0h and it is being placed at 7C00h.
        #

        # fixme here EFI bios cs segment is 0x38
        # switch to IA32e compatible mode
	# esi point to kernel parameter
	# ebx point to kernel entry point
.code64
	cli
	cld
        lgdt    gdt_64
        lidt    idt_64
	ljmp	*exit_ia32e

 	/* switch to IA32e compatible mode */
.code32
mode32:
        #
        # Turn off PG bit in CR0 and set CR3 to zero.
        #
        movl    %cr0, %eax
        andl    $0x7FFFFFFF, %eax
        movl    %eax, %cr0

        /* Setup EFER (Extended Feature Enable Register) */
	nop
	nop
        movl    $0xc0000080, %ecx
        rdmsr
        /* Enable Long Mode */
        andl    $0xFFFFFEFF, %eax
        /* Make changes effective */
        wrmsr

        /* Disable PAE */
        xorl    %eax, %eax
        movl    %eax, %cr4

        jmp    2f
2:
      	jmp     *%ebx
//jmp to kernel entry

.align 8
exit_ia32e:
        .long mode32
        .long 0x10

gdt_64: .word   0x08 * 9 -1
        .quad   gdt

	.word	0, 0, 0
idt_64: .word   0
        .quad   0
        #
        # GDT & IDT stuff for switching into 32 bit protected mode.
        #
	.word	0,0,0
gdt:
        .quad   0x0000000000000000      /* NULL descriptor 0*/
        .quad   0x0                     /* unused 0x08 */
        .quad   0x00cf9a000000ffff      /* __KERNEL_CS 0x10 */
        .quad   0x00cf92000000ffff      /* __KERNEL_DS 0x18 */
        .quad   0x00cf92000000ffff
        .quad   0x00cf92000000ffff
        .quad   0x00cf92000000ffff
        .quad   0x00cf9a000000ffff      /* __KERNEL32_CS 0x38*/
        .quad   0                       /* unused */
.end

