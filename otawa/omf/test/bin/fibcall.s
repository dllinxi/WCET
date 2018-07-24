	.file	"fibcall.c"
gcc2_compiled.:
	.section	.debug_abbrev
.Ldebug_abbrev0:
	.section	.text
.Ltext0:
	.section	.debug_info
.Ldebug_info0:
	.section	.debug_line
.Ldebug_line0:
	.section	".text"
	.align 2
	.globl fib
	.type	 fib,@function
fib:
.LFB1:
.LM1:

	stwu 1,-48(1)
	stw 31,44(1)
	mr 31,1
	stw 3,8(31)
.LM2:

.LBB2:
.LM3:

	li 0,1
	stw 0,16(31)
	li 0,0
	stw 0,20(31)
.LM4:

	li 0,2
	stw 0,12(31)
.LM5:

.L3:
	lwz 0,12(31)
	lwz 9,8(31)
	cmpw 0,0,9
	bc 4,1,.L5
	b .L4
.L5:
.LM6:

	lwz 0,16(31)
	stw 0,24(31)
.LM7:

	lwz 0,16(31)
	lwz 9,20(31)
	add 0,0,9
	stw 0,16(31)
.LM8:

	lwz 0,24(31)
	stw 0,20(31)
.LM9:

	lwz 9,12(31)
	addi 0,9,1
	stw 0,12(31)
.LM10:

	b .L3
.L4:
.LM11:

	lwz 0,16(31)
	stw 0,28(31)
.LM12:

	lwz 0,28(31)
	mr 3,0
	b .L2
.LM13:

.LBE2:
.LM14:

.L2:
	lwz 11,0(1)
	lwz 31,-4(11)
	mr 1,11
	blr
.LFE1:
.Lfe1:
	.size	 fib,.Lfe1-fib
	.align 2
	.globl main
	.type	 main,@function
main:
.LFB2:
.LM15:

	stwu 1,-32(1)
	mflr 0
	stw 31,28(1)
	stw 0,36(1)
	mr 31,1
.LM16:

.LBB3:
.LM17:

	li 0,30
	stw 0,8(31)
.LM18:

	lwz 3,8(31)
	crxor 6,6,6
	bl fib
.LM19:

.LBE3:
.LM20:

.L6:
	lwz 11,0(1)
	lwz 0,4(11)
	mtlr 0
	lwz 31,-4(11)
	mr 1,11
	blr
.LFE2:
.Lfe2:
	.size	 main,.Lfe2-main

	.section	.text
.Letext0:

	.section	.debug_line
	.4byte	0xbd
	.2byte	0x2
	.4byte	0x1d
	.byte	0x4
	.byte	0x1
	.byte	-10
	.byte	245
	.byte	10
	.byte	0x0
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x1
	.byte	0x0
	.string	"fibcall.c"

	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM1
	.byte	0x48
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM2
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM3
	.byte	0x16
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM4
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM5
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM6
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM7
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM8
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM9
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM10
	.byte	0x18
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM11
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM12
	.byte	0x18
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM13
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM15
	.byte	0x17
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM16
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM17
	.byte	0x16
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM18
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM19
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.Letext0
	.byte	0x0
	.byte	0x1
	.byte	0x1

	.section	.debug_abbrev
	.byte	0x1
	.byte	0x11
	.byte	0x1
	.byte	0x3
	.byte	0x8
	.byte	0x1b
	.byte	0x8
	.byte	0x25
	.byte	0x8
	.byte	0x13
	.byte	0xb
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x1
	.byte	0x10
	.byte	0x6
	.byte	0,0
	.byte	0x2
	.byte	0x2e
	.byte	0x1
	.byte	0x1
	.byte	0x13
	.byte	0x3f
	.byte	0xc
	.byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x49
	.byte	0x13
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x1
	.byte	0x40
	.byte	0xa
	.byte	0,0
	.byte	0x3
	.byte	0x5
	.byte	0x0
	.byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x49
	.byte	0x13
	.byte	0x2
	.byte	0xa
	.byte	0,0
	.byte	0x4
	.byte	0x34
	.byte	0x0
	.byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x49
	.byte	0x13
	.byte	0x2
	.byte	0xa
	.byte	0,0
	.byte	0x5
	.byte	0x24
	.byte	0x0
	.byte	0x3
	.byte	0x8
	.byte	0xb
	.byte	0xb
	.byte	0x3e
	.byte	0xb
	.byte	0,0
	.byte	0

	.section	.debug_info
	.4byte	0xfc
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.byte	0x1
	.string	"fibcall.c"

	.string	"/home/casse/otawa/gel/test/bin"

	.string	"GNU C 2.95.3 20010315 (release)"

	.byte	0x1
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.byte	0x2
	.4byte	0xd0
	.byte	0x1
	.string	"fib"

	.byte	0x1
	.byte	0x34
	.4byte	0xd0
	.4byte	.LFB1
	.4byte	.LFE1
	.byte	0x1
	.byte	0x6f
	.byte	0x3
	.string	"n"

	.byte	0x1
	.byte	0x34
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0x8
	.byte	0x4
	.string	"i"

	.byte	0x1
	.byte	0x36
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0xc
	.byte	0x4
	.string	"Fnew"

	.byte	0x1
	.byte	0x36
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0x10
	.byte	0x4
	.string	"Fold"

	.byte	0x1
	.byte	0x36
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0x14
	.byte	0x4
	.string	"temp"

	.byte	0x1
	.byte	0x36
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0x18
	.byte	0x4
	.string	"ans"

	.byte	0x1
	.byte	0x36
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0x1c
	.byte	0x0
	.byte	0x5
	.string	"int"

	.byte	0x4
	.byte	0x5
	.byte	0x2
	.4byte	0xff
	.byte	0x1
	.string	"main"

	.byte	0x1
	.byte	0x4b
	.4byte	0xd0
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x6f
	.byte	0x4
	.string	"a"

	.byte	0x1
	.byte	0x4c
	.4byte	0xd0
	.byte	0x2
	.byte	0x91
	.byte	0x8
	.byte	0x0
	.byte	0x0

	.section	.debug_pubnames
	.4byte	0x1f
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x100
	.4byte	0x62
	.string	"fib"

	.4byte	0xd7
	.string	"main"

	.4byte	0x0

	.section	.debug_aranges
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.4byte	0x4
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.ident	"GCC: (GNU) 2.95.3 20010315 (release)"
